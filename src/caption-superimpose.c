/**
 * @file: caption-superimpose.c
 * @autor: Jorge Pereira <jpereiran@gmail.com>
 * @date: Thu Aug 12 13:49:21 BRT 2010
 * @desc: Mais informações em 'arquitetura-closedcaption.txt'
 */
#include "caption-superimpose.h"
#include "caption-data.h"

CaptionSuperImpose_t*
caption_superimpose_new (const CaptionData_t *caption)
{
  CaptionSuperImpose_t *obj;

  assert (caption != NULL);
  assert (caption->data != NULL);
  assert (caption->datalen > 0);

  obj = (CaptionSuperImpose_t*)calloc (1, sizeof (CaptionSuperImpose_t));
  if (obj == NULL)
    return NULL;

#if 1
  ATV_HEXDUMP (caption->data, caption->datalen);
#endif

  // TODO: Caso necessario a implementacao do SuperImpose,
  // basta seguir a norma ARIB STD-B24, Cap 9

  obj->text_utf8 = NULL;
  obj->len = 0;

#if 0
  // TODO: Caso precise adicionar mais de um CC em um unico objeto
  // basta encadear em 'next' conforme exemplo abaixo.
  // Por Jorge Pereira.
  obj->next = (CaptionSuperImpose_t*)calloc(1, sizeof(CaptionSuperImpose_t));
  obj->next->text_utf8 = strdup ("Isto e apenas um texto de Exemplo, Por Jorge Pereira");
  obj->next->length = strlen (obj->next->text_utf8);
#else
  obj->next = NULL;
#endif

  return obj;
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

