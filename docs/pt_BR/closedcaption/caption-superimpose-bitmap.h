/**
 * @file caption-superimpose-bitmap.h
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @date Thu Aug 12 13:49:21 BRT 2010
 * @brief Data unit de bitmap (uma imagem PNG com posição e cores que piscam).
 *
 * Lê as data units do tipo @ref CAPTION_HANDLER_TYPE_BITMAP, seguindo o "bitmap
 * graphics coding" (ARIB STD-B24, Volume 1, Part 3, 6.2, Table 6-1, p. 156): cabeçalho de posição,
 * cabeçalho de cores que piscam e o arquivo PNG. A lib não decodifica o PNG: valida a
 * assinatura, lê o tamanho do chunk IHDR e entrega os bytes para usar com qualquer decodificador.
 */
#ifndef CAPTION_SUPERIMPOSE_BITMAP_H_
#define CAPTION_SUPERIMPOSE_BITMAP_H_

#include "caption-common.h"
#include "caption-data.h"
#include "caption-management.h"

/**
 * Data unit de bitmap.
 */
typedef struct CaptionSuperImposeBitmap_t {
  int16_t x;                 /**< X do canto superior esquerdo, na área de exibição (pode ser negativo). */
  int16_t y;                 /**< Y do canto superior esquerdo, na área de exibição (pode ser negativo). */

  uint8_t num_flc_colors;    /**< Quantidade de cores que piscam. */
  uint8_t *flc_colors;       /**< Índice na paleta de cada cor que pisca; NULL se @c num_flc_colors é 0. */

  uint8_t *png;              /**< O arquivo PNG, copiado da data unit. */
  size_t png_len;            /**< Tamanho de @c png. */
  uint32_t width;            /**< Largura da imagem, em pixels (do IHDR do PNG). */
  uint32_t height;           /**< Altura da imagem, em pixels (do IHDR do PNG). */

  const PES_pts_t *pts; /**< Aponta para PacketPES_t::pts; válido enquanto o PacketPES_t existir. */
  struct CaptionSuperImposeBitmap_t *next; /**< Sempre NULL: uma data unit de bitmap tem uma imagem só. */
} CaptionSuperImposeBitmap_t;

/**
 * Cria a struct a partir de uma data unit de bitmap.
 *
 * @param m  Conteúdo lido por caption_management_load() ou caption_management_load_unit(), do tipo
 *           @ref CAPTION_HANDLER_TYPE_BITMAP, com data unit não vazia.
 * @return A struct, ou NULL se faltou memória, se a data unit está truncada ou se o PNG não é
 *         válido (assinatura e IHDR). Libere com caption_superimpose_bitmap_free().
 */
CaptionSuperImposeBitmap_t *caption_superimpose_bitmap_new (const CaptionManagement_t *m);

/**
 * Libera a struct criada por caption_superimpose_bitmap_new(), incluindo
 * todos os itens encadeados em @c next.
 *
 * @param cc  A struct. NULL é aceito.
 */
void caption_superimpose_bitmap_free (CaptionSuperImposeBitmap_t *cc);

#endif /* CAPTION_SUPERIMPOSE_BITMAP_H_ */
