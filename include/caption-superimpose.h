/**
 * @file: caption-superimpose.h
 * @autor: Jorge Pereira <jorge.pereira@tqtvd.com>
 * @date: Thu Aug 12 13:49:21 BRT 2010
 * @desc: Mais informações em 'arquitetura-closedcaption.txt'
 */
#ifndef CAPTION_SUPERIMPOSE_H_
#define CAPTION_SUPERIMPOSE_H_

#include "caption-common.h"
#include "caption-data.h"

typedef struct CaptionSuperImpose_s CaptionSuperImpose_t;
struct CaptionSuperImpose_s { 
  const uint8_t *data;
  char *text_utf8;
  size_t len;

  CaptionSuperImpose_t *next; /* Lista encadeada com todos os CSI forem encontrados no CaptionData_t */
};

/**
 * Utilizado na criacao do objeto CaptionSuperImpose_t utilizando como
 * fonte um objeto do tipo CaptionData_t previamente alocado.
 *
 * @param caption Um pacote de dados valido do tipo CaptionData_t
 * @return Se 'caption' for valido, retorna um CaptionSuperImpose_t, sua liberacao atraves de caption_superimpose_free()
 */
CaptionSuperImpose_t* caption_superimpose_new (const CaptionData_t *caption);

/**
 * Esta funcao tem por finalidade liberar o recurso alocado na criacao
 * do objeto CaptionSuperImpose_t.
 * Lembrando que este ponteiro utilizado deve ter sido previamente alocado
 * por 'caption_superimpose_free'
 *
 * @param cc - Ponteiro do tipo CaptionSuperImpose_t
 * @return void
 */
void caption_superimpose_free (CaptionSuperImpose_t *csi);

#endif /*CAPTION_SUPERIMPOSE_H_*/

