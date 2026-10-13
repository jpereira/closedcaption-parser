/**
 * @file caption-superimpose-2bytedrcs.c
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @date Thu Aug 12 13:49:21 BRT 2010
 * @brief Data unit de DRCS de 2 bytes. Não implementado.
 */
#include "caption-superimpose-2bytedrcs.h"

CaptionSuperImpose2ByteDrcs_t *
caption_superimpose_2bytedrcs_new (const CaptionManagement_t *m)
{
  CaptionSuperImpose2ByteDrcs_t *obj;

  assert (m != NULL);
  assert (m->statement.unit_data != NULL);
  assert (m->statement.unit_size > 0);

  obj = calloc (1, sizeof (CaptionSuperImpose2ByteDrcs_t));
  if (obj == NULL)
    return NULL;

#ifdef DEBUG_CAPTION_SUPERIMPOSE_2BYTEDRCS
  ATV_HEXDUMP (m->statement.unit_data, m->statement.unit_size);
#endif

  // TODO: interpretar a data unit (ARIB STD-B24, Volume 1, Capítulo 8).
  // Caso uma data unit gere mais de um item, encadeie-os em 'next'.
  obj->var1 = NULL;
  obj->var2 = 1;
  obj->pts  = m->pts; // apenas referência
  obj->next = NULL;

  return obj;
}

void
caption_superimpose_2bytedrcs_free (CaptionSuperImpose2ByteDrcs_t *cc)
{
  CaptionSuperImpose2ByteDrcs_t *obj = cc;

  while (obj != NULL)
  {
    CaptionSuperImpose2ByteDrcs_t *next = obj->next;

    free (obj->var1);
    free (obj);

    obj = next;
  }
}
