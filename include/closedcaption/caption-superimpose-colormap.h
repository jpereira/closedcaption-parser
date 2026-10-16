/**
 * @file caption-superimpose-colormap.h
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @date Thu Aug 12 13:49:21 BRT 2010
 * @brief Color map data unit. <b>Not implemented</b>.
 *
 * Skeleton to handle the data units of type
 * @ref CAPTION_HANDLER_TYPE_COLOR_MAP. For now it just allocates the struct, without
 * parsing the data (with @c -DDEBUG_CLOSEDCAPTION and
 * @c -DDEBUG_CAPTION_SUPERIMPOSE_COLORMAP, it prints the data unit dump).
 */
#ifndef CAPTION_SUPERIMPOSE_COLORMAP_H_
#define CAPTION_SUPERIMPOSE_COLORMAP_H_

#include "caption-common.h"
#include "caption-data.h"
#include "caption-management.h"

/**
 * Color map data unit. The @c var1 and @c var2 fields are temporary.
 */
typedef struct CaptionSuperImposeColormap_t {
  char *var1;   /**< Temporary: freed by caption_superimpose_colormap_free(). */
  uint8_t var2; /**< Temporary. */

  const PES_pts_t *pts; /**< Points to PacketPES_t::pts; valid while the PacketPES_t exists. */
  struct CaptionSuperImposeColormap_t *next; /**< Next item of the list, or NULL. */
} CaptionSuperImposeColormap_t;

/**
 * Creates the struct from a color map data unit.
 *
 * @param m  Content read by caption_management_load(), of type
 *           @ref CAPTION_HANDLER_TYPE_COLOR_MAP, with a non-empty data unit.
 * @return The struct, or NULL if out of memory. Free with
 *         caption_superimpose_colormap_free().
 */
CaptionSuperImposeColormap_t *caption_superimpose_colormap_new (const CaptionManagement_t *m);

/**
 * Frees the struct created by caption_superimpose_colormap_new(), including
 * all the items chained in @c next.
 *
 * @param cc  The struct. NULL is fine.
 */
void caption_superimpose_colormap_free (CaptionSuperImposeColormap_t *cc);

#endif /* CAPTION_SUPERIMPOSE_COLORMAP_H_ */
