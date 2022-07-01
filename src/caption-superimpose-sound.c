/**
 * @file: caption-superimpose-sound.c
 * @autor: Jorge Pereira <jorge.pereira@tqtvd.com>
 * @date: Thu Aug 12 13:49:21 BRT 2010
 * @desc: Mais informações em 'arquitetura-closedcaption.txt'
 */
#include "caption-superimpose-sound.h"

#define DEBUG_CAPTION_SUPERIMPOSE_SOUND

CaptionSuperImposeSound_t*
caption_superimpose_sound_new (const CaptionManagement_t *m)
{
  CaptionSuperImposeSound_t *obj;
  //const uint8_t *data = m->statement.unit_data;
  //uint8_t len = m->statement.unit_size;

  assert (m != NULL);
  assert (m->statement.unit_data != NULL);
  assert (m->statement.unit_size > 0);
  assert (m->pts != NULL);

  obj = (CaptionSuperImposeSound_t*)calloc (1, sizeof (CaptionSuperImposeSound_t));

#ifdef DEBUG_CAPTION_SUPERIMPOSE_SOUND
  ATV_HEXDUMP (m->statement.unit_data, m->statement.unit_size);
#endif

  // Possiveis dados a parsear...
  obj->var1 = NULL;
  obj->var2 = 1;
  obj->pts  = m->pts; // apenas referencia

#if 0
  // TODO: Caso seja precise adicionar mais de um CC em um unico objeto
  // basta encadear em 'next' conforme exemplo abaixo.
  // Por Jorge Pereira.
  obj->next = (CaptionSuperImposeSound_t*)calloc(1, sizeof(CaptionSuperImposeSound_t));
  obj->next->var1 = strdup("teste");
  obj->next->var2 = 2;
#else
  obj->next = NULL;
#endif

  return obj;
}

void
caption_superimpose_sound_free (CaptionSuperImposeSound_t *cc)
{
  CaptionSuperImposeSound_t *obj = cc;

  for (; obj != NULL; obj = obj->next)
  {
    if (obj->var1 != NULL)
    {
      free (obj->var1);
    }
    obj->var1 = NULL;

    if (obj != NULL)
    {
      free (obj);
    }
  }
}

