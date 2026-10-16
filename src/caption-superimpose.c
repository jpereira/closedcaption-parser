/**
 * @file caption-superimpose.c
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @date Thu Aug 12 13:49:21 BRT 2010
 * @brief Superimpose (texto na mesma estrutura de data_group da legenda).
 */
#include "caption-superimpose.h"
#include "caption-closed.h"
#include "caption-closed-controls.h"
#include "caption-hooking.h"
#include "caption-hooking-priv.h"
#include "caption-management.h"

typedef struct {
  char *buf;
  size_t len;
  size_t cap;
  bool failed;
} Text_t;

static void
text_add (Text_t *t,
          const char *s,
          size_t n)
{
  if (t->failed)
    return;

  if (t->len + n + 1 > t->cap)
  {
    size_t cap = (t->cap == 0) ? 128 : t->cap * 2;
    char *buf;

    while (cap < t->len + n + 1)
      cap *= 2;

    buf = realloc (t->buf, cap);
    if (buf == NULL)
    {
      t->failed = TRUE;
      return;
    }

    t->buf = buf;
    t->cap = cap;
  }

  memcpy (&t->buf[t->len], s, n);
  t->len += n;
  t->buf[t->len] = '\0';
}

/* <hooking> */

static void
on_string (CaptionClosed_t *cc,
           const char *string,
           const uint8_t len)
{
  text_add (cc->userdata, string, len);
}

static void
on_controls (CaptionClosed_t *cc,
             const uint8_t cmd)
{
  Text_t *t = cc->userdata;

  // APR (Active Position Return) e APS (Active Position Set) mudam de linha.
  if ((cmd == CTRL_C0_APR || cmd == CTRL_C0_APS) && t->len > 0 && t->buf[t->len - 1] != '\n')
    text_add (t, "\n", 1);
}

/* </hooking> */

// Lê o texto de uma data unit de texto; NULL se não tem nada além de espaços.
static char *
unit_text (const CaptionManagement_t *m,
           size_t *len)
{
  Text_t t = { NULL, 0, 0, FALSE };
  CaptionClosed_t cc;

  memset (&cc, 0, sizeof (cc));
  cc.userdata = &t;

  caption_closed_run (m, &cc);

  // Tira os espaços e quebras de linha do final.
  while (t.len > 0 && (t.buf[t.len - 1] == ' ' || t.buf[t.len - 1] == '\n'))
    t.buf[--t.len] = '\0';

  if (t.failed || t.len == 0)
  {
    free (t.buf);
    return NULL;
  }

  *len = t.len;
  return t.buf;
}

CaptionSuperImpose_t *
caption_superimpose_new (const CaptionData_t *caption)
{
  CaptionSuperImpose_t *head = NULL, **tail = &head;
  CaptionHookingControls_t old_controls = caption_hooking_priv_get_controls ();
  CaptionHookingString_t old_string = caption_hooking_priv_get_string ();
  CaptionManagement_t m;
  unsigned i;

  assert (caption != NULL);
  assert (caption->data != NULL);
  assert (caption->datalen > 0);

  DTV_HEXDUMP (caption->data, caption->datalen);

  caption_hooking_set_string (on_string);
  caption_hooking_set_controls (on_controls);

  // Uma mensagem para cada data unit de texto do data_group.
  for (i = 0; caption_management_load_unit (caption, i, &m); i++)
  {
    CaptionSuperImpose_t *obj;
    char *text;
    size_t len = 0;

    if (caption_management_get_type (&m) != CAPTION_HANDLER_TYPE_STATEMENT_BODY)
      continue;

    text = unit_text (&m, &len);
    if (text == NULL)
      continue;

    obj = calloc (1, sizeof (CaptionSuperImpose_t));
    if (obj == NULL)
    {
      free (text);
      caption_superimpose_free (head);
      head = NULL;
      break;
    }

    obj->data = caption->data;
    obj->text_utf8 = text;
    obj->len = len;

    *tail = obj;
    tail  = &obj->next;
  }

  caption_hooking_set_string (old_string);
  caption_hooking_set_controls (old_controls);

  return head;
}

void
caption_superimpose_free (CaptionSuperImpose_t *csi)
{
  CaptionSuperImpose_t *obj = csi;

  while (obj != NULL)
  {
    CaptionSuperImpose_t *next = obj->next;

    free (obj->text_utf8);
    free (obj);

    obj = next;
  }
}
