/**
 * @file caption-superimpose-1bytedrcs.h
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @date Thu Aug 12 13:49:21 BRT 2010
 * @brief Data unit de DRCS de 1 byte (caracteres desenhados pela emissora).
 *
 * Lê as data units do tipo @ref CAPTION_HANDLER_TYPE_1BYTE_DRCS, seguindo a
 * "DRCS structure syntax" (ARIB STD-B24, Volume 1, Part 2, Annex D). Cada
 * caractere/fonte enviado na data unit vira um item da lista.
 *
 * Exemplo (imprime os glifos em ASCII art):
 * @code
 * CaptionSuperImpose1ByteDrcs_t *d = caption_superimpose_1bytedrcs_new (&mgmt), *i;
 *
 * for (i = d; i != NULL; i = i->next)
 *   if (i->pixels != NULL)
 *     for (y = 0; y < i->height; y++, putchar ('\n'))
 *       for (x = 0; x < i->width; x++)
 *         putchar (i->pixels[y * i->width + x] ? '#' : '.');
 *
 * caption_superimpose_1bytedrcs_free (d);
 * @endcode
 */
#ifndef CAPTION_SUPERIMPOSE_1BYTEDRCS_H_
#define CAPTION_SUPERIMPOSE_1BYTEDRCS_H_

#include "caption-common.h"
#include "caption-data.h"
#include "caption-management.h"

#define CAPTION_DRCS_MODE_2_GRADATION  0x00 /**< 2 gradações, sem compressão: preenche @c pixels. */
#define CAPTION_DRCS_MODE_MULTI_GRAD   0x01 /**< Várias gradações, sem compressão: preenche @c pixels. */
#define CAPTION_DRCS_MODE_2_COLOR_GEO  0x02 /**< Dados geométricos (2 cores): preenche @c geometric. */
#define CAPTION_DRCS_MODE_MULTI_COLOR  0x03 /**< Dados geométricos (multicolor): preenche @c geometric. */

/**
 * Uma fonte (tipo de letra) de um caractere DRCS. Um caractere enviado com mais de uma fonte
 * vira mais de um item, todos com o mesmo @c character_code.
 */
typedef struct CaptionSuperImpose1ByteDrcs_t {
  uint16_t character_code; /**< Código do caractere. DRCS de 1 byte: 0x41xx (DRCS-1) a 0x4Fxx (DRCS-15),
                                o 2º byte (0x21 a 0x7e) é a posição no conjunto. */
  uint8_t font_id;         /**< Tipo de letra (0 a 15); 0 quer dizer "qualquer". */
  uint8_t mode;            /**< Modo de transmissão, veja @ref CAPTION_DRCS_MODE_2_GRADATION e os próximos. */

  uint8_t depth;           /**< Número de gradações menos 2 (0: 2 gradações). Só nos modos 0 e 1. */
  uint8_t width;           /**< Largura do padrão, em pixels. Só nos modos 0 e 1. */
  uint8_t height;          /**< Altura do padrão, em pixels. Só nos modos 0 e 1. */
  uint8_t *pixels;         /**< @c width x @c height pixels, um byte cada, da esquerda para a direita e de
                                cima para baixo. 0 é o fundo, @c depth + 1 é a frente. NULL nos modos
                                2 e 3, ou quando o padrão é vazio. */

  uint8_t region_x;        /**< Área lógica (unidade de 1/256) usada pelos dados geométricos. Só nos modos 2 e 3. */
  uint8_t region_y;        /**< Idem. */
  uint8_t *geometric;      /**< Dados geométricos, como vieram (veja caption-superimpose-geometric.h). NULL
                                quando não tem. Só nos modos 2 e 3. */
  uint16_t geometric_len;  /**< Tamanho de @c geometric. */

  const PES_pts_t *pts; /**< Aponta para PacketPES_t::pts; válido enquanto o PacketPES_t existir. */
  struct CaptionSuperImpose1ByteDrcs_t *next; /**< Próximo item da lista, ou NULL. */
} CaptionSuperImpose1ByteDrcs_t;

/**
 * Cria a lista a partir de uma data unit de DRCS de 1 byte (caracteres desenhados pela emissora).
 *
 * @param m  Conteúdo lido por caption_management_load() ou caption_management_load_unit(), do tipo
 *           @ref CAPTION_HANDLER_TYPE_1BYTE_DRCS, com data unit não vazia.
 * @return A lista, ou NULL se faltou memória, se a data unit não tem caractere ou se está
 *         truncada/inválida. Libere com caption_superimpose_1bytedrcs_free().
 */
CaptionSuperImpose1ByteDrcs_t *caption_superimpose_1bytedrcs_new (const CaptionManagement_t *m);

/**
 * Libera a lista criada por caption_superimpose_1bytedrcs_new(), incluindo
 * todos os itens encadeados em @c next.
 *
 * @param cc  A lista. NULL é aceito.
 */
void caption_superimpose_1bytedrcs_free (CaptionSuperImpose1ByteDrcs_t *cc);

#endif /* CAPTION_SUPERIMPOSE_1BYTEDRCS_H_ */
