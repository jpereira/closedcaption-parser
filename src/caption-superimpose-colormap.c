/**
 * @file: caption-superimpose-colormap.c
 * @autor: Jorge Pereira <jpereiran@gmail.com>
 * @date: Thu Aug 12 13:49:21 BRT 2010
 * @desc: Mais informações em 'arquitetura-closedcaption.txt'
 */
#include "caption-superimpose-colormap.h"


CaptionSuperImposeColormap_t*
caption_superimpose_colormap_new (const CaptionManagement_t *m)
{
  CaptionSuperImposeColormap_t *obj;
  //const uint8_t *data = m->statement.unit_data;
  //uint8_t len = m->statement.unit_size;

  assert (m != NULL);
  assert (m->statement.unit_data != NULL);
  assert (m->statement.unit_size > 0);
  assert (m->pts != NULL);

  obj = (CaptionSuperImposeColormap_t*)calloc (1, sizeof (CaptionSuperImposeColormap_t));
  if (obj == NULL)
    return NULL;

#ifdef DEBUG_CAPTION_SUPERIMPOSE_COLORMAP
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
  obj->next = (CaptionSuperImposeColormap_t*)calloc(1, sizeof(CaptionSuperImposeColormap_t));
  obj->next->var1 = strdup("teste");
  obj->next->var2 = 2;
#else
  obj->next = NULL;
#endif

  return obj;
}

void
caption_superimpose_colormap_free (CaptionSuperImposeColormap_t *cc)
{
  CaptionSuperImposeColormap_t *obj = cc;

  while (obj != NULL)
  {
    CaptionSuperImposeColormap_t *next = obj->next;

    free (obj->var1);
    free (obj);

    obj = next;
  }
}

