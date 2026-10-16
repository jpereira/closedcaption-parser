/**
 * @file caption-superimpose-colormap.h
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @date Thu Aug 12 13:49:21 BRT 2010
 * @brief Data unit de mapa de cores. <b>Não implementado</b>.
 *
 * Esqueleto para o tratamento das data units do tipo
 * @ref CAPTION_HANDLER_TYPE_COLOR_MAP. Por enquanto apenas aloca a estrutura, sem
 * interpretar os dados (com @c -DDEBUG_CLOSEDCAPTION e
 * @c -DDEBUG_CAPTION_SUPERIMPOSE_COLORMAP, imprime o dump da data unit).
 */
#ifndef CAPTION_SUPERIMPOSE_COLORMAP_H_
#define CAPTION_SUPERIMPOSE_COLORMAP_H_

#include "caption-common.h"
#include "caption-data.h"
#include "caption-management.h"

/**
 * Data unit de mapa de cores. Os campos @c var1 e @c var2 são provisórios.
 */
typedef struct CaptionSuperImposeColormap_t {
  char *var1;   /**< Provisório: liberado por caption_superimpose_colormap_free(). */
  uint8_t var2; /**< Provisório. */

  const PES_pts_t *pts; /**< Aponta para o PacketPES_t::pts; válido enquanto o PacketPES_t existir. */
  struct CaptionSuperImposeColormap_t *next; /**< Próximo item da lista, ou NULL. */
} CaptionSuperImposeColormap_t;

/**
 * Cria a estrutura a partir de uma data unit de mapa de cores.
 *
 * @param m  Conteúdo lido por caption_management_load(), do tipo
 *           @ref CAPTION_HANDLER_TYPE_COLOR_MAP, com data unit não vazia.
 * @return A estrutura, ou NULL se faltar memória. Liberar com
 *         caption_superimpose_colormap_free().
 */
CaptionSuperImposeColormap_t *caption_superimpose_colormap_new (const CaptionManagement_t *m);

/**
 * Libera a estrutura criada por caption_superimpose_colormap_new(), incluindo
 * todos os itens encadeados em @c next.
 *
 * @param cc  A estrutura. Aceita NULL.
 */
void caption_superimpose_colormap_free (CaptionSuperImposeColormap_t *cc);

#endif /* CAPTION_SUPERIMPOSE_COLORMAP_H_ */
