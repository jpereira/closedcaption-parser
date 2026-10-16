/**
 * @file caption-superimpose-sound.c
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @date Thu Aug 12 13:49:21 BRT 2010
 * @brief Data unit de som sintetizado (guarda os bytes, que seguem o ARIB STD-B5).
 */
#include "caption-superimpose-sound.h"

CaptionSuperImposeSound_t *
caption_superimpose_sound_new (const CaptionManagement_t *m)
{
  CaptionSuperImposeSound_t *obj;

  assert (m != NULL);
  assert (m->statement.unit_data != NULL);
  assert (m->statement.unit_size > 0);

#ifdef DEBUG_CAPTION_SUPERIMPOSE_SOUND
  DTV_HEXDUMP (m->statement.unit_data, m->statement.unit_size);
#endif

  obj = calloc (1, sizeof (CaptionSuperImposeSound_t));
  if (obj == NULL)
    return NULL;

  obj->data = malloc (m->statement.unit_size);
  if (obj->data == NULL)
  {
    free (obj);
    return NULL;
  }

  memcpy (obj->data, m->statement.unit_data, m->statement.unit_size);
  obj->len = m->statement.unit_size;
  obj->pts = m->pts; // apenas referência

  return obj;
}

void
caption_superimpose_sound_free (CaptionSuperImposeSound_t *cc)
{
  CaptionSuperImposeSound_t *obj = cc;

  while (obj != NULL)
  {
    CaptionSuperImposeSound_t *next = obj->next;

    free (obj->data);
    free (obj);

    obj = next;
  }
}
