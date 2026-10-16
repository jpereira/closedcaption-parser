/**
 * @file packet-pes.c
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @date Thu Aug 12 13:49:21 BRT 2010
 * @brief Leitura do cabeçalho de um pacote PES de legenda.
 */
#include "packet-pes.h"
#include "caption.h"

/**
 * Localiza o 'payload' (data_group) dentro do PES_data_packet, conforme
 * ARIB STD-B24, Vol 3, Tabela 5-1:
 *   [header PES][data_identifier][private_stream_id][PES_data_packet_header_length][...][data_group]
 */
static bool
packet_pes_set_payload (const uint8_t *pack,
                        const size_t raw_len,
                        PacketPES_t *pes)
{
  size_t pos = 9 + pes->header.len; // posição do 'PES_data_packet'
  size_t end = (size_t)pes->pack_len + 6;

  if (end > raw_len)
    end = raw_len;

  if (pos + 3 > end)
    return FALSE;

  pes->payload.header = (pack[pos] << 16) | (pack[pos+1] << 8) | pack[pos+2];
  pos += 3 + (pack[pos+2] & 0x0f); // 3 bytes do header + PES_data_private_data_byte

  if (pos >= end)
    return FALSE;

  pes->payload.data = &pack[pos];
  pes->payload.len  = end - pos;

  return TRUE;
}

bool
packet_pes_load (const uint8_t *pack_raw,
                 const size_t raw_len,
                 PacketPES_t *out)
{
  PacketPES_t *pes = out;
  const uint8_t *pack = pack_raw;
  uint32_t header = 0;
  
  assert (pack_raw != NULL);
  assert (out != NULL);
  assert (pes != NULL);

  if (raw_len < 9) // Tamanho mínimo do cabeçalho PES
    return FALSE;

  header = ((pack[0] << 16 | pack[1] << 8 | pack[2]) & 0xFFFFFF);
  if (header != PACKET_PES_PREFIX) // packet_start_code_prefix (00 00 01)
    return FALSE;

  // Início do pacote
  pes->stream_id = pack[3];
  pes->pack_len  = ((pack[4] << 8) | pack[5]);

  // Cabeçalho opcional do PES
  pes->header.flags.raw = ((pack[6] << 8) | pack[7]);
  pes->header.len       = pack[8];

  pes->payload.data = NULL;
  pes->payload.len  = 0;

  // +6: os bytes antes do PES_packet_length.
  DTV_DEBUG ("Lendo o pacote PES, len=%d (0x%04x)", pes->pack_len+6, pes->pack_len+6);

  // O cabeçalho opcional começa com o marcador '10'; sem ele (ex: stream_id
  // 0xBF, legendas assíncronas) ou com o PES embaralhado, não há como ler os dados.
  if (pes->header.flags.bit.emulation != 0x02 || pes->header.flags.bit.scrambling != 0)
  {
    DTV_DEBUG ("PES sem cabeçalho opcional ou embaralhado, ignorando.");
    return FALSE;
  }

  {
    switch (pes->header.flags.bit.PTS_DTS_flag)
    {
      case PTS_is_FORBIDEN: /* Proibido */
      {
        DTV_DEBUG("PTS_DTS_flags com valor proibido (01).");
        return FALSE;
      }
      break;

      case PTS_is_BOTH: /* PTS e DTS (Presentation/Decode Time Stamp) */
      case PTS_is_ONLY_PTS: /* apenas PTS (Presentation Time Stamp) */
      {
        // O DTS (quando existir) vem logo após o PTS e não é utilizado;
        // o início do payload é calculado a partir de 'header.len'.
        if (raw_len < 14 || pes->header.len < 5)
          return FALSE;

        if ((pack[9] & 0x20) != 0x20) /* é um PTS válido? */
        {
          DTV_DEBUG ("PTS inválido, ignorando o pacote.");
          return FALSE;
        }
        else
        {
          // O PTS tem 33 bits; o bit mais significativo é descartado.
          uint32_t pts = ((uint32_t)(pack[9] & 0x0E) << 29) | (pack[10] << 22) | ((pack[11] >> 1) << 15) | (pack[12] << 7) | (pack[13] >> 1);

          packet_pes_set_pts (&pts, &pes->pts);

          if (!packet_pes_set_payload (pack, raw_len, pes))
            return FALSE;
        }
      } 
      break;

      case PTS_is_NONE: /* Nenhum */
      {
        uint32_t pts = 0;

        packet_pes_set_pts (&pts, &pes->pts);

        if (!packet_pes_set_payload (pack, raw_len, pes))
          return FALSE;
      }
      break;

      default:
        DTV_DEBUG ("PTS_DTS_flags inválido: %d (%#x)",
          pes->header.flags.bit.PTS_DTS_flag, pes->header.flags.bit.PTS_DTS_flag);
        return FALSE;
    }
  }

#ifdef DEBUG_PACKET_PES // apenas para ajudar na depuração
  DTV_HEXDUMP (pack, ((size_t)pes->pack_len + 6 < raw_len) ? (size_t)pes->pack_len + 6 : raw_len);
  DTV_DEBUG("\n"
            "<PacketPES_t>\n"
            "  stream_id = %#x\n"
            "  pack_len  = %#x\n"
            "  struct header {\n"
            "    len = %d (%#x)\n"
            "    raw = %#x\n"
            "    struct flags {\n"
            "      rights              = %#x\n"
            "      copyright           = %#x\n"
            "      aligment            = %#x\n"
            "      priority            = %#x\n"
            "      scrambling          = %#x\n"
            "      emulation           = %#x\n"
            "      PES_extension_flag  = %#x\n"
            "      PES_CRC_flag        = %#x\n"
            "      copy_info_flag      = %#x\n"
            "      DSM_trick_mode_flag = %#x\n"
            "      ES_rate_flag        = %#x\n"
            "      ESCR_flag           = %#x\n"
            "      PTS_DTS_flag        = %#x\n"
            "    };\n"
            "  };\n"
            "  struct payload {\n"
            "    header = %#x\n"
            "    data   = %p\n"
            "    len    = %d (%#x)\n"
            "  };\n"
            "</PacketPES_t>",
            pes->stream_id, pes->pack_len, pes->header.len, pes->header.len, pes->header.flags.raw,
            pes->header.flags.bit.rights, pes->header.flags.bit.copyright, pes->header.flags.bit.alignment, 
            pes->header.flags.bit.priority, pes->header.flags.bit.scrambling, pes->header.flags.bit.emulation,
            pes->header.flags.bit.PES_extension_flag, pes->header.flags.bit.PES_CRC_flag, pes->header.flags.bit.copy_info_flag, 
            pes->header.flags.bit.DSM_trick_mode_flag, pes->header.flags.bit.ES_rate_flag, pes->header.flags.bit.ESCR_flag, 
            pes->header.flags.bit.PTS_DTS_flag, pes->payload.header, pes->payload.data, pes->payload.len, pes->payload.len
  );
#endif

  return TRUE;
}

void
packet_pes_set_pts (uint32_t *ts,
                    PES_pts_t *pts)
{
  uint64_t p = 0;
  
  assert (pts != NULL);
  assert (ts != NULL);

  pts->ts = *ts;
  p       = pts->ts / 9;
  pts->h  = (p/(10000L*60*60));
  pts->m  = (p/(10000L*60))-(pts->h*60);
  pts->s  = (p/10000L)-(pts->h*3600)-(pts->m*60);
  pts->u  = (p-(pts->h*10000L*60*60)-(pts->m*10000L*60)-(pts->s*10000L));
}

