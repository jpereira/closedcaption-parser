/**
 * @file caption-superimpose-geometric.c
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @date Thu Aug 12 13:49:21 BRT 2010
 * @brief Data unit de gráficos geométricos. Não implementado.
 */
#include "caption-superimpose-geometric.h"

CaptionSuperImposeGeometric_t *
caption_superimpose_geometric_new (const CaptionManagement_t *m)
{
  CaptionSuperImposeGeometric_t *obj;

  assert (m != NULL);
  assert (m->statement.unit_data != NULL);
  assert (m->statement.unit_size > 0);

  obj = calloc (1, sizeof (CaptionSuperImposeGeometric_t));
  if (obj == NULL)
    return NULL;

#ifdef DEBUG_CAPTION_SUPERIMPOSE_GEOMETRIC
  DTV_HEXDUMP (m->statement.unit_data, m->statement.unit_size);
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
caption_superimpose_geometric_free (CaptionSuperImposeGeometric_t *cc)
{
  CaptionSuperImposeGeometric_t *obj = cc;

  while (obj != NULL)
  {
    CaptionSuperImposeGeometric_t *next = obj->next;

    free (obj->var1);
    free (obj);

    obj = next;
  }
}
