/**
 * @file: caption-superimpose-2bytedrcs.c
 * @autor: Jorge Pereira <jpereiran@gmail.com>
 * @date: Thu Aug 12 13:49:21 BRT 2010
 * @desc: Mais informações em 'arquitetura-closedcaption.txt'
 */
#include "caption-superimpose-2bytedrcs.h"


CaptionSuperImpose2ByteDrcs_t*
caption_superimpose_2bytedrcs_new (const CaptionManagement_t *m)
{
  CaptionSuperImpose2ByteDrcs_t *obj;
  //const uint8_t *data = m->statement.unit_data;
  //uint8_t len = m->statement.unit_size;

  assert (m != NULL);
  assert (m->statement.unit_data != NULL);
  assert (m->statement.unit_size > 0);
  assert (m->pts != NULL);

  obj = (CaptionSuperImpose2ByteDrcs_t*)calloc (1, sizeof (CaptionSuperImpose2ByteDrcs_t));
  if (obj == NULL)
    return NULL;

#ifdef DEBUG_CAPTION_SUPERIMPOSE_2BYTEDRCS
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
  obj->next = (CaptionSuperImpose2ByteDrcs_t*)calloc(1, sizeof(CaptionSuperImpose2ByteDrcs_t));
  obj->next->var1 = strdup("teste");
  obj->next->var2 = 2;
#else
  obj->next = NULL;
#endif

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

