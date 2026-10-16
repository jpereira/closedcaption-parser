/*
 * Testes da API: versão, entrada inválida e TS/PES quebrado.
 * A ideia é não crashar nunca (rodar com ASan/UBSan: make test-sanitize).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <closedcaption/caption.h>

static int failures;

#define CHECK(cond) do { \
    if (!(cond)) { fprintf (stderr, "FALHOU %s:%d: %s\n", __FILE__, __LINE__, #cond); failures++; } \
  } while (0)

static unsigned lines;

static void on_line (const CaptionLine_t *line, void *userdata)
{
  (void) line;
  (void) userdata;
  lines++;
}

static void on_pes (const uint8_t *pes, size_t len, void *userdata)
{
  caption_screen_push_pes (userdata, pes, len);
}

static void test_version (void)
{
  char want[32];

  snprintf (want, sizeof (want), "%d.%d.%d", CLOSEDCAPTION_VERSION_MAJOR,
            CLOSEDCAPTION_VERSION_MINOR, CLOSEDCAPTION_VERSION_PATCH);
  CHECK (strcmp (closedcaption_version (), want) == 0);
}

static void test_reader_errors (void)
{
  TsReaderError_t error;

  CHECK (ts_reader_open ("/nao/existe.ts", &error) == NULL);
  CHECK (ts_reader_open ("/nao/existe.ts", NULL) == NULL);
}

static void test_pes_garbage (void)
{
  CaptionScreen_t *screen = caption_screen_new (on_line, NULL);
  uint8_t buf[512];
  size_t len;
  unsigned seed = 1234;

  CHECK (screen != NULL);

  /* PES truncado em todos os tamanhos e cheio de lixo */
  for (len = 0; len < sizeof (buf); len++)
    {
      size_t i;

      for (i = 0; i < len; i++)
        {
          seed = seed * 1103515245u + 12345u;
          buf[i] = (uint8_t) (seed >> 16);
        }
      if (len > 3)
        {
          buf[0] = 0x00;
          buf[1] = 0x00;
          buf[2] = 0x01;
        }
      caption_screen_push_pes (screen, buf, len);
    }

  caption_screen_flush (screen);
  caption_screen_free (screen);
}

static void test_ts_garbage (void)
{
  CaptionScreen_t *screen = caption_screen_new (on_line, NULL);
  TsDemux_t *demux = ts_demux_new ();
  uint8_t packet[TS_PACKET_SIZE];
  unsigned seed = 42;
  int n;

  CHECK (screen != NULL && demux != NULL);
  ts_demux_set_pes_callback (demux, 0x130, on_pes, screen);

  for (n = 0; n < 20000; n++)
    {
      size_t i;

      for (i = 0; i < sizeof (packet); i++)
        {
          seed = seed * 1103515245u + 12345u;
          packet[i] = (uint8_t) (seed >> 16);
        }
      packet[0] = TS_SYNC_BYTE;
      /* metade dos pacotes cai no PID da legenda */
      if (n % 2)
        {
          packet[1] = (uint8_t) ((packet[1] & 0xe0) | 0x01);
          packet[2] = 0x30;
        }
      ts_demux_push (demux, packet);
    }

  ts_demux_flush (demux);
  caption_screen_flush (screen);
  ts_demux_free (demux);
  caption_screen_free (screen);
}

int main (int argc, char *argv[])
{
  const char *only = (argc > 1) ? argv[1] : NULL;

  /* sem argumento roda tudo; com argumento roda só um (version|reader|pes|ts) */
  if (!only || !strcmp (only, "version")) test_version ();
  if (!only || !strcmp (only, "reader")) test_reader_errors ();
  if (!only || !strcmp (only, "pes")) test_pes_garbage ();
  if (!only || !strcmp (only, "ts")) test_ts_garbage ();

  if (failures)
    {
      fprintf (stderr, "%d falha(s)\n", failures);
      return EXIT_FAILURE;
    }
  puts ("OK: test-api");
  return EXIT_SUCCESS;
}
