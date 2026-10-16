/**
 * @file caption-superimpose-2bytedrcs.h
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @date Thu Aug 12 13:49:21 BRT 2010
 * @brief Data unit de DRCS de 2 bytes (caracteres desenhados pela emissora).
 *
 * Lê as data units do tipo @ref CAPTION_HANDLER_TYPE_2BYTE_DRCS. A sintaxe é a mesma
 * do DRCS de 1 byte (ARIB STD-B24, Volume 1, Part 2, Annex D); só muda o significado do
 * @c character_code: aqui os dois bytes são o próprio código do caractere suplementar
 * (Gaiji). Por isso a struct é a mesma do caption-superimpose-1bytedrcs.h.
 */
#ifndef CAPTION_SUPERIMPOSE_2BYTEDRCS_H_
#define CAPTION_SUPERIMPOSE_2BYTEDRCS_H_

#include "caption-common.h"
#include "caption-data.h"
#include "caption-management.h"
#include "caption-superimpose-1bytedrcs.h"

/**
 * Uma fonte de um caractere DRCS de 2 bytes: os mesmos campos de CaptionSuperImpose1ByteDrcs_t,
 * mas o @c character_code é o código de 2 bytes do caractere.
 */
typedef CaptionSuperImpose1ByteDrcs_t CaptionSuperImpose2ByteDrcs_t;

/**
 * Cria a lista a partir de uma data unit de DRCS de 2 bytes (caracteres desenhados pela emissora).
 *
 * @param m  Conteúdo lido por caption_management_load() ou caption_management_load_unit(), do tipo
 *           @ref CAPTION_HANDLER_TYPE_2BYTE_DRCS, com data unit não vazia.
 * @return A lista, ou NULL se faltou memória, se a data unit não tem caractere ou se está
 *         truncada/inválida. Libere com caption_superimpose_2bytedrcs_free().
 */
CaptionSuperImpose2ByteDrcs_t *caption_superimpose_2bytedrcs_new (const CaptionManagement_t *m);

/**
 * Libera a lista criada por caption_superimpose_2bytedrcs_new(), incluindo
 * todos os itens encadeados em @c next.
 *
 * @param cc  A lista. NULL é aceito.
 */
void caption_superimpose_2bytedrcs_free (CaptionSuperImpose2ByteDrcs_t *cc);

#endif /* CAPTION_SUPERIMPOSE_2BYTEDRCS_H_ */
