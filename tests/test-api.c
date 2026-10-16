/*
 * Testes da API: versão, entrada inválida e TS/PES quebrado.
 * A ideia é não crashar nunca (rodar com ASan/UBSan: make test-sanitize).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <closedcaption/caption.h>
#include <closedcaption/caption-hooking-priv.h>

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

/* Monta o CaptionManagement_t como se a data unit tivesse vindo de um PES. */
static CaptionManagement_t unit_mgmt (const uint8_t *data, size_t len)
{
  CaptionManagement_t m;

  memset (&m, 0, sizeof (m));
  m.statement.unit_sep = 0x1f;
  m.statement.unit_size = (uint32_t) len;
  m.statement.unit_data = data;
  return m;
}

static void test_drcs (void)
{
  /* 1 caractere 0x4121, 1 fonte, modo 0, 2x2 em 1 bit: 10/01 */
  static const uint8_t two_gray[] = { 1, 0x41, 0x21, 1, 0x00, 0, 2, 2, 0x90 };
  /* depth 14 (16 gradações, 4 bits por pixel), 3x1: 1, 15, 8 */
  static const uint8_t sixteen[] = { 1, 0x7a, 0x50, 1, 0x11, 14, 3, 1, 0x1f, 0x80 };
  /* modo 2: região 128x64 e 3 bytes geométricos */
  static const uint8_t geo[] = { 1, 0x7a, 0x51, 1, 0x32, 0x80, 0x40, 0, 3, 0x24, 0x40, 0x41 };
  /* 2 caracteres, o primeiro com 2 fontes */
  static const uint8_t many[] = { 2, 0x41, 0x21, 2, 0x00, 0, 1, 1, 0x80, 0x10, 0, 1, 1, 0x00,
                                  0x41, 0x22, 1, 0x00, 0, 0, 0 };
  CaptionManagement_t m;
  CaptionSuperImpose1ByteDrcs_t *d;
  size_t n;

  m = unit_mgmt (two_gray, sizeof (two_gray));
  d = caption_superimpose_1bytedrcs_new (&m);
  CHECK (d != NULL && d->next == NULL);
  CHECK (d && d->character_code == 0x4121 && d->font_id == 0 && d->mode == 0);
  CHECK (d && d->width == 2 && d->height == 2 && d->depth == 0 && d->pixels != NULL);
  CHECK (d && d->pixels[0] == 1 && d->pixels[1] == 0 && d->pixels[2] == 0 && d->pixels[3] == 1);
  caption_superimpose_1bytedrcs_free (d);

  m = unit_mgmt (sixteen, sizeof (sixteen));
  d = caption_superimpose_2bytedrcs_new (&m);
  CHECK (d != NULL && d->font_id == 1 && d->mode == 1 && d->depth == 14);
  CHECK (d && d->pixels[0] == 1 && d->pixels[1] == 15 && d->pixels[2] == 8);
  caption_superimpose_2bytedrcs_free (d);

  m = unit_mgmt (geo, sizeof (geo));
  d = caption_superimpose_2bytedrcs_new (&m);
  CHECK (d != NULL && d->mode == 2 && d->pixels == NULL);
  CHECK (d && d->region_x == 0x80 && d->region_y == 0x40 && d->geometric_len == 3);
  CHECK (d && d->geometric[0] == 0x24 && d->geometric[2] == 0x41);
  caption_superimpose_2bytedrcs_free (d);

  m = unit_mgmt (many, sizeof (many));
  d = caption_superimpose_1bytedrcs_new (&m);
  CHECK (d != NULL && d->next != NULL && d->next->next != NULL && d->next->next->next == NULL);
  CHECK (d && d->next && d->next->font_id == 1 && d->next->character_code == 0x4121);
  CHECK (d && d->next && d->next->next && d->next->next->character_code == 0x4122);
  CHECK (d && d->next && d->next->next && d->next->next->pixels == NULL);
  caption_superimpose_1bytedrcs_free (d);

  /* cortada em qualquer ponto não pode ser aceita nem vazar/estourar (ASan) */
  for (n = 1; n < sizeof (many); n++)
    {
      m = unit_mgmt (many, n);
      CHECK (caption_superimpose_1bytedrcs_new (&m) == NULL);
    }

  m = unit_mgmt ((const uint8_t *) "\0", 1); /* nenhum caractere */
  CHECK (caption_superimpose_1bytedrcs_new (&m) == NULL);

  caption_superimpose_1bytedrcs_free (NULL);
  caption_superimpose_2bytedrcs_free (NULL);
}

static void test_bitmap (void)
{
  /* x=100, y=-20, 2 cores piscando (3 e 5) e um PNG que só tem o começo (assinatura + IHDR 7x3) */
  static const uint8_t ok[] = { 0, 100, 0xff, 0xec, 2, 3, 5,
                                0x89, 'P', 'N', 'G', 0x0d, 0x0a, 0x1a, 0x0a,
                                0, 0, 0, 13, 'I', 'H', 'D', 'R', 0, 0, 0, 7, 0, 0, 0, 3, 8, 2, 0, 0, 0 };
  uint8_t bad[sizeof (ok)];
  CaptionManagement_t m;
  CaptionSuperImposeBitmap_t *b;
  size_t n;

  m = unit_mgmt (ok, sizeof (ok));
  b = caption_superimpose_bitmap_new (&m);
  CHECK (b != NULL && b->x == 100 && b->y == -20 && b->width == 7 && b->height == 3);
  CHECK (b && b->num_flc_colors == 2 && b->flc_colors[0] == 3 && b->flc_colors[1] == 5);
  CHECK (b && b->png_len == sizeof (ok) - 7 && b->png[1] == 'P' && b->next == NULL);
  caption_superimpose_bitmap_free (b);

  /* PNG truncado antes do fim do IHDR (largura/altura) */
  for (n = 1; n < sizeof (ok) - 12; n++)
    {
      m = unit_mgmt (ok, n);
      CHECK (caption_superimpose_bitmap_new (&m) == NULL);
    }

  memcpy (bad, ok, sizeof (ok));
  bad[8] = 'X'; /* assinatura errada */
  m = unit_mgmt (bad, sizeof (bad));
  CHECK (caption_superimpose_bitmap_new (&m) == NULL);

  caption_superimpose_bitmap_free (NULL);
}

static void test_geometric_sound (void)
{
  /* DOMAIN com 2 operandos, byte de controle (NUL), RECT sem operando, operando solto no início */
  static const uint8_t geo[] = { 0x41, 0x21, 0x40, 0x00, 0x41, 0x30, 0x23, 0x42, 0x90 };
  static const uint8_t snd[] = { 9, 8, 7 };
  CaptionManagement_t m;
  CaptionSuperImposeGeometric_t *g;
  CaptionSuperImposeSound_t *s;

  m = unit_mgmt (geo, sizeof (geo));
  g = caption_superimpose_geometric_new (&m);
  CHECK (g != NULL && g->opcode == 0x21 && g->operands_len == 2 && g->operands[1] == 0x41);
  CHECK (g && g->next && g->next->opcode == 0x30 && g->next->operands_len == 0 && g->next->operands == NULL);
  CHECK (g && g->next && g->next->next && g->next->next->opcode == 0x23 && g->next->next->operands_len == 1);
  CHECK (g && g->next && g->next->next && g->next->next->next == NULL);
  caption_superimpose_geometric_free (g);

  m = unit_mgmt (geo, 1); /* só operando: nenhum comando */
  CHECK (caption_superimpose_geometric_new (&m) == NULL);

  CHECK (strcmp (caption_superimpose_geometric_get_name (0x21), "DOMAIN") == 0);
  CHECK (strcmp (caption_superimpose_geometric_get_name (0x29), "LINE") == 0);
  CHECK (strcmp (caption_superimpose_geometric_get_name (0x3f), "BLINK") == 0);
  CHECK (strcmp (caption_superimpose_geometric_get_name (0x22), "UNKNOWN") == 0);
  CHECK (strcmp (caption_superimpose_geometric_get_name (0xff), "UNKNOWN") == 0);

  m = unit_mgmt (snd, sizeof (snd));
  s = caption_superimpose_sound_new (&m);
  CHECK (s != NULL && s->len == 3 && s->data != snd && s->data[0] == 9 && s->data[2] == 7);
  caption_superimpose_sound_free (s);

  caption_superimpose_geometric_free (NULL);
  caption_superimpose_sound_free (NULL);
}

static void test_units_garbage (void)
{
  uint8_t buf[96];
  unsigned seed = 7, round;

  for (round = 0; round < 20000; round++)
    {
      size_t len = 1 + (seed >> 8) % sizeof (buf), i;
      CaptionManagement_t m;

      for (i = 0; i < len; i++)
        {
          seed = seed * 1103515245u + 12345u;
          buf[i] = (uint8_t) (seed >> 16);
        }
      /* número de caracteres/fontes pequenos, pra passar do começo */
      if (round % 2)
        {
          buf[0] = (uint8_t) (1 + buf[0] % 3);
          if (len > 3) buf[3] = (uint8_t) (1 + buf[3] % 2);
        }

      m = unit_mgmt (buf, len);
      caption_superimpose_1bytedrcs_free (caption_superimpose_1bytedrcs_new (&m));
      caption_superimpose_bitmap_free (caption_superimpose_bitmap_new (&m));
      caption_superimpose_geometric_free (caption_superimpose_geometric_new (&m));
      caption_superimpose_sound_free (caption_superimpose_sound_new (&m));
    }
}

static void dummy_string (CaptionClosed_t *cc, const char *s, const uint8_t len)
{
  (void) cc; (void) s; (void) len;
}

static void test_superimpose (void)
{
  /* statement data: TMD=00, loop com 3 data units: texto, DRCS (ignorado) e texto */
  static const uint8_t group[] = {
    0x3f, 0, 0, 32,
    0x1f, 0x20, 0, 0, 9, 'O', 'l', 0xe1, 0x0d, 'M', 'u', 'n', 'd', 'o',
    0x1f, 0x30, 0, 0, 3, 1, 0x41, 0x21,
    0x1f, 0x20, 0, 0, 3, 'F', 'i', 'm' };
  CaptionData_t data;
  CaptionSuperImpose_t *msg;

  memset (&data, 0, sizeof (data));
  data.group_id = 1;
  data.data = group;
  data.datalen = sizeof (group);

  caption_hooking_set_string (dummy_string);

  msg = caption_superimpose_new (&data);
  CHECK (msg != NULL && msg->next != NULL && msg->next->next == NULL);
  CHECK (msg && strcmp (msg->text_utf8, "Ol\xc3\xa1\nMundo") == 0 && msg->len == strlen (msg->text_utf8));
  CHECK (msg && msg->next && strcmp (msg->next->text_utf8, "Fim") == 0);
  CHECK (msg && msg->data == group);
  /* os hooks do usuário voltam como estavam */
  CHECK (caption_hooking_priv_get_string () == dummy_string);
  caption_superimpose_free (msg);

  /* caption management data não tem texto */
  data.group_id = 0;
  CHECK (caption_superimpose_new (&data) == NULL);

  caption_hooking_set_string (NULL);
  caption_superimpose_free (NULL);
}

int main (int argc, char *argv[])
{
  const char *only = (argc > 1) ? argv[1] : NULL;

  /* sem argumento roda tudo; com argumento roda só um (version|reader|pes|ts|units) */
  if (!only || !strcmp (only, "version")) test_version ();
  if (!only || !strcmp (only, "reader")) test_reader_errors ();
  if (!only || !strcmp (only, "pes")) test_pes_garbage ();
  if (!only || !strcmp (only, "ts")) test_ts_garbage ();
  if (!only || !strcmp (only, "units"))
    {
      test_drcs ();
      test_bitmap ();
      test_geometric_sound ();
      test_units_garbage ();
      test_superimpose ();
    }

  if (failures)
    {
      fprintf (stderr, "%d falha(s)\n", failures);
      return EXIT_FAILURE;
    }
  puts ("OK: test-api");
  return EXIT_SUCCESS;
}
