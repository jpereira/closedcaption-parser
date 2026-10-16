/**
 * @file caption-superimpose-sound.h
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @date Thu Aug 12 13:49:21 BRT 2010
 * @brief Data unit de som sintetizado.
 *
 * Lê as data units do tipo @ref CAPTION_HANDLER_TYPE_SYNTHESIZED_SOUND. A codificação do
 * som sintetizado não é definida pela ARIB STD-B24, ela só aponta para a ARIB STD-B5 (ARIB STD-B24,
 * Volume 1, Part 3, 7.3, p. 157). Então a lib só valida a data unit
 * e entrega os bytes como vieram, para a aplicação decodificar.
 */
#ifndef CAPTION_SUPERIMPOSE_SOUND_H_
#define CAPTION_SUPERIMPOSE_SOUND_H_

#include "caption-common.h"
#include "caption-data.h"
#include "caption-management.h"

/**
 * Data unit de som sintetizado.
 */
typedef struct CaptionSuperImposeSound_t {
  uint8_t *data;  /**< Dados do som sintetizado (codificação ARIB STD-B5), copiados da data unit. */
  size_t len;     /**< Tamanho de @c data. */

  const PES_pts_t *pts; /**< Aponta para PacketPES_t::pts; válido enquanto o PacketPES_t existir. */
  struct CaptionSuperImposeSound_t *next; /**< Sempre NULL: uma data unit de som tem um item só. */
} CaptionSuperImposeSound_t;

/**
 * Cria a struct a partir de uma data unit de som sintetizado.
 *
 * @param m  Conteúdo lido por caption_management_load() ou caption_management_load_unit(), do tipo
 *           @ref CAPTION_HANDLER_TYPE_SYNTHESIZED_SOUND, com data unit não vazia.
 * @return A struct, ou NULL se faltou memória. Libere com caption_superimpose_sound_free().
 */
CaptionSuperImposeSound_t *caption_superimpose_sound_new (const CaptionManagement_t *m);

/**
 * Libera a struct criada por caption_superimpose_sound_new(), incluindo
 * todos os itens encadeados em @c next.
 *
 * @param cc  A struct. NULL é aceito.
 */
void caption_superimpose_sound_free (CaptionSuperImposeSound_t *cc);

#endif /* CAPTION_SUPERIMPOSE_SOUND_H_ */
