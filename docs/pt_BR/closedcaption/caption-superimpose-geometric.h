/**
 * @file caption-superimpose-geometric.h
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @date Thu Aug 12 13:49:21 BRT 2010
 * @brief Data unit de gráficos geométricos, separada em comandos.
 *
 * Lê as data units do tipo @ref CAPTION_HANDLER_TYPE_GEOMETRIC. Os gráficos geométricos
 * são uma sequência de "graphics display commands" (ARIB STD-B24, Volume 1, Part 2, 8.1.2):
 * um opcode (0x20 a 0x3f) seguido de zero ou mais bytes de operando (0x40 a 0x7f). Este módulo só
 * separa o fluxo em comandos; desenhar (DOMAIN, POINT, LINE, ARC, RECT, POLY...) fica por conta
 * da aplicação. Controles C0/C1 e bytes fora dessas duas faixas são ignorados.
 */
#ifndef CAPTION_SUPERIMPOSE_GEOMETRIC_H_
#define CAPTION_SUPERIMPOSE_GEOMETRIC_H_

#include "caption-common.h"
#include "caption-data.h"
#include "caption-management.h"

#define CAPTION_GEOMETRIC_OPCODE_RESET        0x20 /**< RESET: 2 bytes fixos. */
#define CAPTION_GEOMETRIC_OPCODE_DOMAIN       0x21 /**< DOMAIN: 1 byte fixo e um operando multivalor. */
#define CAPTION_GEOMETRIC_OPCODE_TEXTURE      0x23 /**< TEXTURE: 1 byte fixo. */
#define CAPTION_GEOMETRIC_OPCODE_SET_COLOR    0x3c /**< SET COLOR. */
#define CAPTION_GEOMETRIC_OPCODE_SET_PATTERN  0x3d /**< SET PATTERN. */
#define CAPTION_GEOMETRIC_OPCODE_SELECT_COLOR 0x3e /**< SELECT COLOR. */
#define CAPTION_GEOMETRIC_OPCODE_BLINK        0x3f /**< BLINK. */

/**
 * Um graphics display command.
 */
typedef struct CaptionSuperImposeGeometric_t {
  uint8_t opcode;     /**< Opcode (0x20 a 0x3f). Os bits baixos dizem a variante (absoluto/relativo,
                           contorno/preenchido, set-and-draw). */
  uint8_t *operands;  /**< Bytes de operando (0x40 a 0x7f); NULL se não tem. */
  size_t operands_len; /**< Tamanho de @c operands. */

  const PES_pts_t *pts; /**< Aponta para PacketPES_t::pts; válido enquanto o PacketPES_t existir. */
  struct CaptionSuperImposeGeometric_t *next; /**< Próximo comando da lista, ou NULL. */
} CaptionSuperImposeGeometric_t;

/**
 * Cria a lista de comandos a partir de uma data unit de gráficos geométricos.
 *
 * @param m  Conteúdo lido por caption_management_load() ou caption_management_load_unit(), do tipo
 *           @ref CAPTION_HANDLER_TYPE_GEOMETRIC, com data unit não vazia.
 * @return A lista, ou NULL se faltou memória ou se não tem nenhum comando. Libere com
 *         caption_superimpose_geometric_free().
 */
CaptionSuperImposeGeometric_t *caption_superimpose_geometric_new (const CaptionManagement_t *m);

/**
 * Nome da família do comando de um opcode ("RESET", "DOMAIN", "TEXTURE", "POINT", "LINE", "ARC",
 * "RECT", "POLY", "SET COLOR", "SET PATTERN", "SELECT COLOR" ou "BLINK").
 *
 * @param opcode  Opcode do comando.
 * @return O nome, ou "UNKNOWN". Nunca NULL.
 */
const char *caption_superimpose_geometric_get_name (uint8_t opcode);

/**
 * Libera a lista criada por caption_superimpose_geometric_new(), incluindo
 * todos os itens encadeados em @c next.
 *
 * @param cc  A lista. NULL é aceito.
 */
void caption_superimpose_geometric_free (CaptionSuperImposeGeometric_t *cc);

#endif /* CAPTION_SUPERIMPOSE_GEOMETRIC_H_ */
