/**
 * @file ts-demux.c
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @brief Demultiplexador MPEG-TS: tabelas PAT/PMT, estatísticas por PID e
 *        remontagem dos pacotes PES de um PID.
 */
#include "ts-demux.h"

#define STREAM_TYPE_PRIVATE 0x06 /* PES com dados privados (legendas ISDB-T) */
#define DESC_STREAM_ID      0x52 /* stream_identifier_descriptor */

#define PES_MAX_LEN         (64 * 1024 + 6) /* PES_packet_length (16 bits) + 6 bytes de header */
#define SECTION_MAX_LEN     4096            /* maior seção PSI permitida */
#define PROGRAMS_MAX        64

typedef struct {
  uint8_t data[SECTION_MAX_LEN];
  size_t len;
} Section_t;

typedef struct {
  uint16_t pid;    /* PID da PMT */
  uint16_t number; /* program_number */
  Section_t section;
} Program_t;

typedef struct {
  uint16_t pid;
  TsDemuxPesCallback_t callback;
  void *userdata;
  uint8_t data[PES_MAX_LEN];
  size_t len;
  int8_t last_cc; /* último continuity_counter, -1 quando desconhecido */
} PesBuffer_t;

struct TsDemux_s {
  Section_t pat;
  Program_t programs[PROGRAMS_MAX];
  unsigned programs_count;

  PesBuffer_t pes;

  uint16_t caption_pid; /* sugerido pela PMT */
  uint8_t caption_tag;

  TsPidInfo_t pids[TS_PID_MAX];
};

/* <seções PSI> */

/**
 * Acumula uma seção PSI (PAT/PMT) que pode ocupar mais de um pacote TS.
 * Retorna TRUE quando a seção está completa.
 */
static bool
section_push (Section_t *s,
              const uint8_t *p,
              size_t len,
              bool pusi)
{
  if (pusi)
  {
    uint8_t pointer = p[0];

    if ((size_t)pointer + 1 >= len)
      return FALSE;

    p   += pointer + 1;
    len -= pointer + 1;
    s->len = 0;
  }
  else if (s->len == 0)
  {
    return FALSE; // continuação de uma seção cujo início não foi recebido
  }

  if (s->len + len > sizeof (s->data))
    len = sizeof (s->data) - s->len;

  memcpy (&s->data[s->len], p, len);
  s->len += len;

  return (s->len >= 3 && s->len >= (size_t)(((s->data[1] & 0x0f) << 8 | s->data[2]) + 3));
}

/**
 * Retorna a posição final dos dados da seção (antes do CRC32), ou 0 se a
 * seção for menor que o mínimo esperado.
 */
static size_t
section_end (const Section_t *s,
             size_t min_len)
{
  size_t end = ((s->data[1] & 0x0f) << 8 | s->data[2]) + 3;

  if (end < min_len + 4 || end > s->len)
    return 0;

  return end - 4; // -4 do CRC32
}

static void
parse_pat (TsDemux_t *demux)
{
  const uint8_t *d = demux->pat.data;
  size_t end = section_end (&demux->pat, 8);
  size_t i;

  // table_id 0x00; considera apenas a primeira PAT recebida.
  if (d[0] != 0x00 || end == 0 || demux->programs_count > 0)
    return;

  for (i = 8; i + 4 <= end && demux->programs_count < PROGRAMS_MAX; i += 4)
  {
    uint16_t number = (d[i] << 8 | d[i+1]);
    uint16_t pid = ((d[i+2] & 0x1f) << 8 | d[i+3]);
    Program_t *prog;

    if (number == 0) // network_PID (NIT)
      continue;

    prog = &demux->programs[demux->programs_count++];
    prog->pid = pid;
    prog->number = number;

    demux->pids[pid].is_pmt = TRUE;
    demux->pids[pid].program = number;
  }
}

static void
parse_pmt (TsDemux_t *demux,
           const Program_t *prog)
{
  const uint8_t *d = prog->section.data;
  size_t end = section_end (&prog->section, 12);
  uint16_t pcr_pid;
  size_t i;

  // table_id 0x02
  if (d[0] != 0x02 || end == 0)
    return;

  pcr_pid = ((d[8] & 0x1f) << 8 | d[9]);
  i = 12 + ((d[10] & 0x0f) << 8 | d[11]); // pula o program_info

  if (pcr_pid != TS_PID_NULL)
  {
    demux->pids[pcr_pid].is_pcr = TRUE;
    demux->pids[pcr_pid].program = prog->number;
  }

  while (i + 5 <= end)
  {
    uint8_t type = d[i];
    uint16_t pid = ((d[i+1] & 0x1f) << 8 | d[i+2]);
    size_t es_end = i + 5 + ((d[i+3] & 0x0f) << 8 | d[i+4]);
    TsPidInfo_t *info = &demux->pids[pid];
    size_t j;

    if (es_end > end)
      es_end = end;

    info->program = prog->number;
    info->is_es = TRUE;
    info->stream_type = type;
    info->component_tag = -1;

    for (j = i + 5; j + 2 <= es_end && j + 2 + d[j+1] <= es_end; j += 2 + d[j+1])
    {
      uint8_t tag;

      if (d[j] != DESC_STREAM_ID || d[j+1] < 1)
        continue;

      tag = d[j+2];
      info->component_tag = tag;

      // component_tag: 0x30-0x37 legenda (full-seg), 0x87 legenda (one-seg).
      // Sugere a full-seg, e o menor component_tag.
      if (type == STREAM_TYPE_PRIVATE && ((tag >= 0x30 && tag <= 0x37) || tag == 0x87))
      {
        if (demux->caption_pid == TS_PID_NONE || tag < demux->caption_tag)
        {
          demux->caption_pid = pid;
          demux->caption_tag = tag;
        }
      }
    }

    i = es_end;
  }
}

/* </seções PSI> */

/* <PES> */

static void
pes_deliver (PesBuffer_t *b,
             size_t len)
{
  if (len > 0 && b->callback != NULL)
    b->callback (b->data, len, b->userdata);

  b->len = 0;
}

static void
pes_push (PesBuffer_t *b,
          const uint8_t *p,
          size_t len,
          bool pusi,
          uint8_t cc)
{
  // Pacote perdido? descarta o PES incompleto.
  if (!pusi && b->last_cc >= 0 && cc != ((b->last_cc + 1) & 0x0f))
    b->len = 0;

  b->last_cc = cc;

  if (pusi)
    pes_deliver (b, b->len);
  else if (b->len == 0)
    return; // continuação de um PES cujo início não foi recebido

  if (b->len + len > sizeof (b->data))
  {
    b->len = 0;
    return;
  }

  memcpy (&b->data[b->len], p, len);
  b->len += len;

  // PES completo? (quando o PES_packet_length é informado)
  if (b->len >= 6)
  {
    size_t total = (size_t)(b->data[4] << 8 | b->data[5]) + 6;

    if (total > 6 && b->len >= total)
      pes_deliver (b, total);
  }
}

/* </PES> */

TsDemux_t *
ts_demux_new (void)
{
  TsDemux_t *demux = calloc (1, sizeof (TsDemux_t));
  unsigned pid;

  if (demux == NULL)
    return NULL;

  demux->caption_pid = TS_PID_NONE;
  demux->pes.pid = TS_PID_NONE;
  demux->pes.last_cc = -1;

  for (pid = 0; pid < TS_PID_MAX; pid++)
    demux->pids[pid].component_tag = -1;

  return demux;
}

void
ts_demux_free (TsDemux_t *demux)
{
  free (demux);
}

void
ts_demux_set_pes_callback (TsDemux_t *demux,
                           uint16_t pid,
                           TsDemuxPesCallback_t callback,
                           void *userdata)
{
  assert (demux != NULL);

  demux->pes.pid = (pid < TS_PID_MAX) ? pid : TS_PID_NONE;
  demux->pes.callback = callback;
  demux->pes.userdata = userdata;
  demux->pes.len = 0;
  demux->pes.last_cc = -1;
}

void
ts_demux_push (TsDemux_t *demux,
               const uint8_t *pkt)
{
  uint16_t pid;
  bool pusi;
  uint8_t afc, cc;
  size_t pos = 4;
  unsigned i;

  assert (demux != NULL);
  assert (pkt != NULL);

  if (pkt[0] != TS_SYNC_BYTE || (pkt[1] & 0x80)) // transport_error_indicator
    return;

  pid  = ((pkt[1] & 0x1f) << 8 | pkt[2]);
  pusi = (pkt[1] & 0x40) != 0; // payload_unit_start_indicator
  afc  = (pkt[3] & 0x30) >> 4; // adaptation_field_control
  cc   = (pkt[3] & 0x0f);      // continuity_counter

  demux->pids[pid].packets++;

  if (!(afc & 0x01)) // sem payload
    return;

  if (afc & 0x02) // pula o adaptation_field
    pos += 1 + pkt[4];

  if (pos >= TS_PACKET_SIZE)
    return;

  if (pid == demux->pes.pid)
  {
    pes_push (&demux->pes, &pkt[pos], TS_PACKET_SIZE - pos, pusi, cc);
    return;
  }

  if (pid == TS_PID_PAT)
  {
    if (section_push (&demux->pat, &pkt[pos], TS_PACKET_SIZE - pos, pusi))
      parse_pat (demux);
    return;
  }

  for (i = 0; i < demux->programs_count; i++)
  {
    Program_t *prog = &demux->programs[i];

    if (pid == prog->pid && section_push (&prog->section, &pkt[pos], TS_PACKET_SIZE - pos, pusi))
      parse_pmt (demux, prog);
  }
}

void
ts_demux_flush (TsDemux_t *demux)
{
  assert (demux != NULL);

  pes_deliver (&demux->pes, demux->pes.len);
}

const TsPidInfo_t *
ts_demux_get_pid_info (const TsDemux_t *demux,
                       uint16_t pid)
{
  assert (demux != NULL);

  return (pid < TS_PID_MAX) ? &demux->pids[pid] : NULL;
}

uint16_t
ts_demux_get_caption_pid (const TsDemux_t *demux)
{
  assert (demux != NULL);

  return demux->caption_pid;
}

const char *
ts_demux_describe_pid (const TsDemux_t *demux,
                       uint16_t pid,
                       char *buf,
                       size_t len)
{
  const TsPidInfo_t *info = ts_demux_get_pid_info (demux, pid);
  const char *name = ts_system_pid_name (pid);

  assert (buf != NULL);

  if (len == 0)
    return buf;

  if (info == NULL)
    snprintf (buf, len, "PID inválido");
  else if (name != NULL)
    snprintf (buf, len, "%s", name);
  else if (info->is_pmt)
    snprintf (buf, len, "PMT");
  else if (info->is_es)
  {
    size_t n = snprintf (buf, len, "0x%02x %s", info->stream_type,
                         ts_stream_type_name (info->stream_type));

    if (info->component_tag >= 0 && n < len)
    {
      const char *tag_name = ts_component_tag_name (info->component_tag);

      n += snprintf (&buf[n], len - n, " (component_tag 0x%02x%s%s)",
                     info->component_tag, tag_name ? ", " : "", tag_name ? tag_name : "");
    }

    if (info->is_pcr && n < len)
      snprintf (&buf[n], len - n, " + PCR");
  }
  else if (info->is_pcr)
    snprintf (buf, len, "PCR");
  else
    snprintf (buf, len, "fora da PAT/PMT");

  return buf;
}

const char *
ts_stream_type_name (uint8_t stream_type)
{
  switch (stream_type)
  {
    case 0x01: return "MPEG-1 video";
    case 0x02: return "MPEG-2 video";
    case 0x03: return "MPEG-1 audio";
    case 0x04: return "MPEG-2 audio";
    case 0x06: return "PES privado";
    case 0x0b: return "DSM-CC (data carousel)";
    case 0x0d: return "DSM-CC";
    case 0x0f: return "AAC audio";
    case 0x11: return "AAC LATM audio";
    case 0x1b: return "H.264 video";
    case 0x24: return "HEVC video";
    case 0x81: return "AC-3 audio";
    default:   return "desconhecido";
  }
}

const char *
ts_component_tag_name (int16_t component_tag)
{
  if (component_tag >= 0x30 && component_tag <= 0x37)
    return "legenda";
  if (component_tag >= 0x38 && component_tag <= 0x3f)
    return "superimpose";
  if (component_tag == 0x87)
    return "legenda one-seg";
  return NULL;
}

const char *
ts_system_pid_name (uint16_t pid)
{
  switch (pid)
  {
    case 0x0000: return "PAT";
    case 0x0001: return "CAT";
    case 0x0010: return "NIT";
    case 0x0011: return "SDT/BAT";
    case 0x0012: return "EIT";
    case 0x0013: return "RST";
    case 0x0014: return "TDT/TOT";
    case 0x0023: return "SDTT";
    case 0x0024: return "BIT";
    case 0x0029: return "CDT";
    case 0x1fff: return "nulo (stuffing)";
    default:     return NULL;
  }
}
