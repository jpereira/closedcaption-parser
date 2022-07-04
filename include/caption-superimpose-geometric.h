/**
 * @file: caption-superimpose-geometric.h
 * @autor: Jorge Pereira <jorge.pereira@tqtvd.com>
 * @date: Thu Aug 12 13:49:21 BRT 2010
 * @desc: Mais informações em 'arquitetura-closedcaption.txt'
 */
#ifndef CAPTION_SUPERIMPOSE_GEOMETRIC_H_
#define CAPTION_SUPERIMPOSE_GEOMETRIC_H_

#include "caption-common.h"
#include "caption-data.h"
#include "caption-management.h"

typedef struct CaptionSuperImposeGeometric_t { 
  char *var1;
  uint8_t var2;

  const PES_pts_t *pts; /* apenas referencia */
  struct CaptionSuperImposeGeometric_t *next; /* Lista encadeada com todos 'CC' encontrados no pacote PES */
} CaptionSuperImposeGeometric_t;

/**
 * Utilizado na criacao do objeto CaptionSuperImposeGeometric_t utilizando como
 * fonte um objeto do tipo CaptionManagement_t previamente alocado.
 *
 * ** DOCUMENTAR **
 *
 * @param m Um pacote de dados valido do tipo CaptionManagement_t
 * @return Se 'm' for valido, retorna um CaptionSuperImposeGeometric_t, sua liberacao atraves de caption_superimpose_geometric_free()
 */
CaptionSuperImposeGeometric_t* caption_superimpose_geometric_new (const CaptionManagement_t *m);

/**
 * Esta funcao tem por finalidade liberar o recurso alocado na criacao
 * do objeto CaptionSuperImposeGeometric_t.
 * Lembrando que este ponteiro utilizado deve ter sido previamente alocado
 * por 'caption_superimpose_geometric_free'
 *
 * @param cc - Ponteiro do tipo CaptionSuperImposeGeometric_t
 * @return void
 */
void caption_superimpose_geometric_free (CaptionSuperImposeGeometric_t *cc);

#endif /*CAPTION_SUPERIMPOSE_GEOMETRIC_H_*/
