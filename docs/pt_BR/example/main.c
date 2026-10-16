/**
 * @file example/main.c
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @brief Exemplo de uso da libclosedcaption: lista os PIDs de um arquivo
 *        MPEG-TS (ISDB-T/SBTVD) ou exibe as legendas de um PID.
 *
 * Uso:
 * @code
 * ./example/example video.ts          # lista os PIDs e indica o da legenda
 * ./example/example video.ts <pid>    # exibe as legendas do <pid>
 * ./example/example video.ts <pid> --units   # mostra também as data units de DRCS, bitmap,
 *                                            # geométrico, som e superimpose do <pid>
 * @endcode
 */
#include <closedcaption/caption.h>

/** Imprime uma linha de legenda e conta quantas foram impressas. */
static void
on_line (const CaptionLine_t *line,
         void *userdata)
{
  unsigned *count = userdata;
  uint32_t ms = line->start_ms;

  printf ("[%02u:%02u:%02u.%03u] %s\n",
          ms / 3600000, (ms / 60000) % 60, (ms / 1000) % 60, ms % 1000, line->text);

  (*count)++;
}

/** O que o on_pes() precisa: a tela e se deve mostrar as data units que não são texto. */
typedef struct {
  CaptionScreen_t *screen;
  bool units;
} Context_t;

/** Imprime um padrão DRCS em ASCII art: '.' é o fundo, os outros valores são as gradações. */
static void
drcs_print (const CaptionSuperImpose1ByteDrcs_t *d)
{
  for (; d != NULL; d = d->next)
  {
    unsigned x, y;

    printf ("  codigo=0x%04x fonte=%u modo=%u", d->character_code, d->font_id, d->mode);

    if (d->mode > CAPTION_DRCS_MODE_MULTI_GRAD)
    {
      printf (" geometrico=%u byte(s) regiao=%ux%u\n", d->geometric_len, d->region_x, d->region_y);
      continue;
    }

    printf (" %ux%u gradacoes=%u\n", d->width, d->height, d->depth + 2);

    for (y = 0; d->pixels != NULL && y < d->height; y++)
    {
      printf ("    ");
      for (x = 0; x < d->width; x++)
      {
        uint8_t v = d->pixels[y * d->width + x];

        putchar (v == 0 ? '.' : (v < 10 ? '0' + v : 'a' + v - 10));
      }
      putchar ('\n');
    }
  }
}

/** Mostra o que tem numa data unit que não é o texto da legenda. */
static void
unit_print (const CaptionManagement_t *m)
{
  switch (caption_management_get_type (m))
  {
    case CAPTION_HANDLER_TYPE_1BYTE_DRCS:
    case CAPTION_HANDLER_TYPE_2BYTE_DRCS:
    {
      bool one = (caption_management_get_type (m) == CAPTION_HANDLER_TYPE_1BYTE_DRCS);
      CaptionSuperImpose1ByteDrcs_t *d = one ? caption_superimpose_1bytedrcs_new (m)
                                             : caption_superimpose_2bytedrcs_new (m);

      printf ("[unidade] DRCS de %d byte(s)%s\n", one ? 1 : 2, d == NULL ? ": invalido" : "");
      drcs_print (d);
      caption_superimpose_1bytedrcs_free (d);
      break;
    }

    case CAPTION_HANDLER_TYPE_BITMAP:
    {
      CaptionSuperImposeBitmap_t *b = caption_superimpose_bitmap_new (m);

      if (b == NULL)
        printf ("[unidade] bitmap: invalido\n");
      else
        printf ("[unidade] bitmap: x=%d y=%d PNG %ux%u (%zu bytes) cores piscando=%u\n",
                b->x, b->y, b->width, b->height, b->png_len, b->num_flc_colors);

      caption_superimpose_bitmap_free (b);
      break;
    }

    case CAPTION_HANDLER_TYPE_GEOMETRIC:
    {
      CaptionSuperImposeGeometric_t *g = caption_superimpose_geometric_new (m), *i;

      printf ("[unidade] geometrico%s\n", g == NULL ? ": sem comandos" : "");

      for (i = g; i != NULL; i = i->next)
        printf ("  %-12s opcode=0x%02x operandos=%zu\n",
                caption_superimpose_geometric_get_name (i->opcode), i->opcode, i->operands_len);

      caption_superimpose_geometric_free (g);
      break;
    }

    case CAPTION_HANDLER_TYPE_SYNTHESIZED_SOUND:
    {
      CaptionSuperImposeSound_t *s = caption_superimpose_sound_new (m);

      printf ("[unidade] som sintetizado: %zu byte(s)\n", s == NULL ? 0 : s->len);
      caption_superimpose_sound_free (s);
      break;
    }

    default:
      break;
  }
}

/** Mostra todas as data units de um PES e as mensagens de superimpose das de texto. */
static void
units_print (const uint8_t *raw,
             size_t len)
{
  PacketPES_t pes;
  CaptionData_t data;
  CaptionManagement_t m;
  CaptionSuperImpose_t *msgs, *i;
  unsigned n;

  memset (&pes, 0, sizeof (pes));

  if (!packet_pes_load (raw, len, &pes) || !caption_data_load (&pes, &data))
    return;

  for (n = 0; caption_management_load_unit (&data, n, &m); n++)
    unit_print (&m);

  msgs = caption_superimpose_new (&data);

  for (i = msgs; i != NULL; i = i->next)
    printf ("[superimpose] %s\n", i->text_utf8);

  caption_superimpose_free (msgs);
}

/** Repassa cada PES completo do PID da legenda para a tela. */
static void
on_pes (const uint8_t *pes,
        size_t len,
        void *userdata)
{
  Context_t *ctx = userdata;

  if (ctx->units)
    units_print (pes, len);

  caption_screen_push_pes (ctx->screen, pes, len);
}

/** Imprime a tabela com todos os PIDs encontrados no arquivo. */
static void
pids_list (const TsDemux_t *demux,
           const char *filename)
{
  uint16_t caption_pid = ts_demux_get_caption_pid (demux);
  unsigned pid;

  printf ("PIDs encontrados em '%s':\n", filename);
  printf ("  %-6s  %8s  %-8s  %s\n", "PID", "Pacotes", "Programa", "Tipo");

  for (pid = 0; pid < TS_PID_MAX; pid++)
  {
    const TsPidInfo_t *info = ts_demux_get_pid_info (demux, pid);
    char program[16] = "-";
    char desc[160];

    if (info->packets == 0)
      continue;

    if (info->program != 0)
      snprintf (program, sizeof (program), "%u", info->program);

    printf ("  0x%04x  %8u  %-8s  %s%s\n", pid, info->packets, program,
            ts_demux_describe_pid (demux, pid, desc, sizeof (desc)),
            (pid == caption_pid) ? "  <- legenda" : "");
  }
}

/** Converte o PID informado na linha de comando (decimal ou hexadecimal). */
static bool
parse_pid (const char *arg,
           uint16_t *pid)
{
  char *end;
  unsigned long value = strtoul (arg, &end, 0);

  if (*arg == '\0' || *end != '\0' || value >= TS_PID_MAX)
    return FALSE;

  *pid = (uint16_t)value;
  return TRUE;
}

/**
 * Sem o PID, lista os PIDs do arquivo; com o PID, exibe as legendas dele.
 *
 * @param argc  Quantidade de argumentos (2 ou 3).
 * @param argv  Nome do programa, arquivo .ts e, opcionalmente, o PID.
 * @return EXIT_SUCCESS, ou EXIT_FAILURE em caso de erro ou se nenhuma
 *         legenda for encontrada.
 */
int
main (int argc,
      char *argv[])
{
  TsReaderError_t error;
  TsReader_t *reader;
  TsDemux_t *demux;
  CaptionScreen_t *screen = NULL;
  Context_t ctx = { NULL, FALSE };
  const uint8_t *packet;
  uint16_t pid = TS_PID_NONE;
  unsigned lines = 0;
  int ret = EXIT_SUCCESS;

  if (argc == 4 && strcmp (argv[3], "--units") == 0)
  {
    ctx.units = TRUE;
    argc--;
  }

  if (argc < 2 || argc > 3 || (ctx.units && argc != 3))
  {
    fprintf (stderr, "Uso: %s <arquivo.ts> [pid da legenda [--units]]\n"
                     "  Sem o [pid], lista os PIDs encontrados no arquivo.\n"
                     "  Com --units, mostra tambem as data units de DRCS, bitmap, geometrico,\n"
                     "  som e as mensagens de superimpose.\n", argv[0]);
    return EXIT_FAILURE;
  }

  if (argc == 3 && !parse_pid (argv[2], &pid))
  {
    fprintf (stderr, "PID inválido: '%s' (use decimal ou hexadecimal, ex: 0x3ed)\n", argv[2]);
    return EXIT_FAILURE;
  }

  reader = ts_reader_open (argv[1], &error);
  if (reader == NULL)
  {
    if (error == TS_READER_ERROR_OPEN)
      fprintf (stderr, "Erro ao abrir '%s': %s\n", argv[1], strerror (errno));
    else if (error == TS_READER_ERROR_FORMAT)
      fprintf (stderr, "'%s' não parece ser um MPEG-TS válido.\n", argv[1]);
    else
      fprintf (stderr, "Memória insuficiente.\n");
    return EXIT_FAILURE;
  }

  demux = ts_demux_new ();
  if (demux == NULL)
  {
    fprintf (stderr, "Memória insuficiente.\n");
    ts_reader_close (reader);
    return EXIT_FAILURE;
  }

  if (pid != TS_PID_NONE)
  {
    screen = caption_screen_new (on_line, &lines);
    if (screen == NULL)
    {
      fprintf (stderr, "Memória insuficiente.\n");
      ts_demux_free (demux);
      ts_reader_close (reader);
      return EXIT_FAILURE;
    }

    ctx.screen = screen;
    ts_demux_set_pes_callback (demux, pid, on_pes, &ctx);
  }

  while ((packet = ts_reader_next (reader)) != NULL)
    ts_demux_push (demux, packet);

  ts_reader_close (reader);

  if (screen == NULL)
  {
    // Sem PID: apenas lista os PIDs.
    pids_list (demux, argv[1]);

    if (ts_demux_get_caption_pid (demux) == TS_PID_NONE)
    {
      printf ("\nNenhum PID de legenda encontrado na PMT.\n");
      ret = EXIT_FAILURE;
    }
    else
    {
      printf ("\nPara exibir as legendas:\n  %s %s 0x%04x\n",
              argv[0], argv[1], ts_demux_get_caption_pid (demux));
    }
  }
  else
  {
    ts_demux_flush (demux);
    caption_screen_flush (screen);
    fflush (stdout);

    if (ts_demux_get_pid_info (demux, pid)->packets == 0)
    {
      fprintf (stderr, "O PID 0x%04x não existe em '%s'.\n", pid, argv[1]);
      ret = EXIT_FAILURE;
    }
    else if (lines == 0)
    {
      fprintf (stderr, "Nenhuma legenda encontrada no PID 0x%04x.\n", pid);
      ret = EXIT_FAILURE;
    }
    else
    {
      fprintf (stderr, "\n%u linha(s) de legenda extraída(s) do PID 0x%04x.\n", lines, pid);
    }

    caption_screen_free (screen);
  }

  ts_demux_free (demux);

  return ret;
}
