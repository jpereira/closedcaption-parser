/**
 * @file caption-screen.c
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @brief Converte os pacotes PES de legenda em linhas de texto.
 */
#include "caption-screen.h"
#include "caption.h"

typedef struct {
  char text[CAPTION_SCREEN_LINE_LEN];
  uint32_t start_ms; /* quando a linha apareceu pela primeira vez */
} ScreenLine_t;

typedef struct {
  ScreenLine_t line[CAPTION_SCREEN_MAX_LINES];
  unsigned count;
} ScreenLines_t;

struct CaptionScreen_s {
  CaptionScreenLineCallback_t callback;
  void *userdata;

  CaptionClosed_t cc;   /* estado da legenda (cores, tamanho, etc.) */
  char text[4096];      /* texto recebido pelos hooks no PES corrente */
  size_t text_len;
  ScreenLines_t screen; /* o que está na "tela" no momento */

  uint32_t now_ms;      /* tempo do PES corrente */
  uint32_t first_pts;
  bool has_first_pts;
};

/**
 * Lê um caractere UTF-8 de 's' e devolve a sua letra base, sem acento
 * (ex: 'Ã' -> 'A'). Apenas o Latin-1 (U+00A0 a U+00FF) é convertido.
 */
static int
char_fold (const char **s)
{
  static const char base[] =
    "AAAAAAACEEEEIIIIDNOOOOOxOUUUUYPs"  // U+00C0 - U+00DF
    "aaaaaaaceeeeiiiidnooooo/ouuuuypy"; // U+00E0 - U+00FF
  const unsigned char *p = (const unsigned char *)*s;

  // U+00C0 - U+00FF: 0xc3 0x80-0xbf
  if (p[0] == 0xc3 && (p[1] & 0xc0) == 0x80)
  {
    *s += 2;
    return base[p[1] & 0x3f];
  }

  // U+00A0 - U+00BF: 0xc2 0xa0-0xbf (sem letras, mantém o próprio byte)
  if (p[0] == 0xc2 && (p[1] & 0xc0) == 0x80)
  {
    *s += 2;
    return p[1];
  }

  *s += 1;
  return p[0];
}

bool
caption_screen_line_continues (const char *line,
                               const char *prefix)
{
  assert (line != NULL);
  assert (prefix != NULL);

  while (*prefix != '\0')
  {
    if (*line == '\0' || char_fold (&line) != char_fold (&prefix))
      return FALSE;
  }

  return TRUE;
}

static void
line_deliver (CaptionScreen_t *s,
              const ScreenLine_t *l)
{
  CaptionLine_t line = { l->text, l->start_ms };

  if (s->callback != NULL)
    s->callback (&line, s->userdata);
}

/**
 * Monta a tela nova a partir de s->text e compara com a anterior: as linhas
 * que deixaram a tela (sem continuação na tela nova) são entregues.
 */
static void
screen_commit (CaptionScreen_t *s)
{
  ScreenLines_t screen;
  const char *p = s->text, *end = s->text + s->text_len;
  unsigned i, j;

  memset (&screen, 0, sizeof (screen));

  // Separa as linhas, removendo os espaços de preenchimento.
  while (p < end && screen.count < CAPTION_SCREEN_MAX_LINES)
  {
    const char *nl = memchr (p, '\n', end - p);
    size_t len = (nl ? nl : end) - p;

    while (len > 0 && p[len - 1] == ' ')
      len--;

    if (len > 0)
    {
      ScreenLine_t *l = &screen.line[screen.count++];

      if (len >= sizeof (l->text))
        len = sizeof (l->text) - 1;

      memcpy (l->text, p, len);
      l->start_ms = s->now_ms;
    }

    p = nl ? nl + 1 : end;
  }

  for (i = 0; i < s->screen.count; i++)
  {
    const ScreenLine_t *old = &s->screen.line[i];
    bool continued = FALSE;

    for (j = 0; j < screen.count; j++)
    {
      if (caption_screen_line_continues (screen.line[j].text, old->text))
      {
        screen.line[j].start_ms = old->start_ms;
        continued = TRUE;
      }
    }

    if (!continued)
      line_deliver (s, old);
  }

  s->screen = screen;
  s->text_len = 0;
}

/* <hooking> */

static void
on_string (CaptionClosed_t *cc,
           const char *string,
           const uint8_t len)
{
  CaptionScreen_t *s = cc->userdata;

  if (s->text_len + len < sizeof (s->text))
  {
    memcpy (&s->text[s->text_len], string, len);
    s->text_len += len;
  }
}

static void
on_controls (CaptionClosed_t *cc,
             const uint8_t cmd)
{
  CaptionScreen_t *s = cc->userdata;

  switch (cmd)
  {
    // APR (Active Position Return) e APS (Active Position Set) mudam de linha.
    case CTRL_C0_APR:
    case CTRL_C0_APS:
      if (s->text_len > 0 && s->text[s->text_len - 1] != '\n')
        on_string (cc, "\n", 1);
      break;

    // CS (Clear Screen) no meio do PES: fecha o que já estava na tela.
    case CTRL_C0_CS:
      if (s->text_len > 0)
        screen_commit (s);
      break;
  }
}

/* </hooking> */

CaptionScreen_t *
caption_screen_new (CaptionScreenLineCallback_t callback,
                    void *userdata)
{
  CaptionScreen_t *s = calloc (1, sizeof (CaptionScreen_t));

  if (s == NULL)
    return NULL;

  s->callback = callback;
  s->userdata = userdata;
  s->cc.userdata = s;

  return s;
}

void
caption_screen_free (CaptionScreen_t *screen)
{
  free (screen);
}

bool
caption_screen_push_pes (CaptionScreen_t *s,
                         const uint8_t *raw,
                         size_t len)
{
  PacketPES_t pes;
  CaptionData_t data;
  CaptionManagement_t mgmt;

  assert (s != NULL);
  assert (raw != NULL);

  memset (&pes, 0, sizeof (pes));

  if (!packet_pes_load (raw, len, &pes))
    return FALSE;

  if (!caption_data_load (&pes, &data))
    return FALSE;

  if (!caption_management_load (&data, &mgmt))
    return FALSE;

  // O "caption management data" e as demais data units não têm texto.
  if (caption_management_get_type (&mgmt) != CAPTION_HANDLER_TYPE_STATEMENT_BODY)
    return FALSE;

  // Tempo relativo ao primeiro PTS de legenda.
  if (pes.pts.ts != 0)
  {
    if (!s->has_first_pts)
    {
      s->first_pts = pes.pts.ts;
      s->has_first_pts = TRUE;
    }

    s->now_ms = (uint32_t)(pes.pts.ts - s->first_pts) / 90;
  }

  // Os hooks são globais, então são registrados a cada PES.
  caption_hooking_set_string (on_string);
  caption_hooking_set_controls (on_controls);

  s->text_len = 0;
  s->cc.need_clear = FALSE;

  caption_closed_run (&mgmt, &s->cc);
  s->cc.pts = NULL; // apontava para o 'pes' local

  screen_commit (s);

  return TRUE;
}

void
caption_screen_flush (CaptionScreen_t *s)
{
  unsigned i;

  assert (s != NULL);

  for (i = 0; i < s->screen.count; i++)
    line_deliver (s, &s->screen.line[i]);

  s->screen.count = 0;
}
