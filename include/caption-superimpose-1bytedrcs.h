/**
 * @file: caption-superimpose-1bytedrcs.h
 * @autor: Jorge Pereira <jorge.pereira@tqtvd.com>
 * @date: Thu Aug 12 13:49:21 BRT 2010
 * @desc: Mais informações em 'arquitetura-closedcaption.txt'
 */
#ifndef CAPTION_SUPERIMPOSE_1BYTEDRCS_H_
#define CAPTION_SUPERIMPOSE_1BYTEDRCS_H_

#include "caption-common.h"
#include "caption-data.h"
#include "caption-management.h"

typedef struct CaptionSuperImpose1ByteDrcs_t { 
  char *var1;
  uint8_t var2;

  const PES_pts_t *pts; /* apenas referencia */
  struct CaptionSuperImpose1ByteDrcs_t *next; /* Lista encadeada com todos 'CC' encontrados no pacote PES */
} CaptionSuperImpose1ByteDrcs_t;

/**
 * Utilizado na criacao do objeto CaptionSuperImpose1ByteDrcs_t utilizando como
 * fonte um objeto do tipo CaptionManagement_t previamente alocado.
 *
 * ** DOCUMENTAR **
 *
 * @param m Um pacote de dados valido do tipo CaptionManagement_t
 * @return Se 'm' for valido, retorna um CaptionSuperImpose1ByteDrcs_t, sua liberacao atraves de caption_superimpose_1bytedrcs_free()
 */
CaptionSuperImpose1ByteDrcs_t*
caption_superimpose_1bytedrcs_new (const CaptionManagement_t *m);

/**
 * Esta funcao tem por finalidade liberar o recurso alocado na criacao
 * do objeto CaptionSuperImpose1ByteDrcs_t.
 * Lembrando que este ponteiro utilizado deve ter sido previamente alocado
 * por 'caption_superimpose_1bytedrcs_free'
 *
 * @param cc - Ponteiro do tipo CaptionSuperImpose1ByteDrcs_t
 * @return void
 */
void
caption_superimpose_1bytedrcs_free (CaptionSuperImpose1ByteDrcs_t *cc);

#endif /* CAPTION_SUPERIMPOSE_1BYTEDRCS_H_ */

