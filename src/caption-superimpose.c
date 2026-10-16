/**
 * @file caption-superimpose.c
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @date Thu Aug 12 13:49:21 BRT 2010
 * @brief Legendas do tipo superimpose. Não implementado.
 */
#include "caption-superimpose.h"
#include "caption-data.h"

CaptionSuperImpose_t *
caption_superimpose_new (const CaptionData_t *caption)
{
  CaptionSuperImpose_t *obj;

  assert (caption != NULL);
  assert (caption->data != NULL);
  assert (caption->datalen > 0);

  obj = calloc (1, sizeof (CaptionSuperImpose_t));
  if (obj == NULL)
    return NULL;

  DTV_HEXDUMP (caption->data, caption->datalen);

  // TODO: interpretar o superimpose (ARIB STD-B24, Volume 3, Capítulo 9).
  // Caso o data_group tenha mais de uma mensagem, encadeie-as em 'next'.
  obj->text_utf8 = NULL;
  obj->len = 0;
  obj->next = NULL;

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
