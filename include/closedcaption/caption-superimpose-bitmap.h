/**
 * @file caption-superimpose-bitmap.h
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @date Thu Aug 12 13:49:21 BRT 2010
 * @brief Bitmap data unit. <b>Not implemented</b>.
 *
 * Skeleton to handle the data units of type
 * @ref CAPTION_HANDLER_TYPE_BITMAP. For now it just allocates the struct, without
 * parsing the data (with @c -DDEBUG_CLOSEDCAPTION and
 * @c -DDEBUG_CAPTION_SUPERIMPOSE_BITMAP, it prints the data unit dump).
 */
#ifndef CAPTION_SUPERIMPOSE_BITMAP_H_
#define CAPTION_SUPERIMPOSE_BITMAP_H_

#include "caption-common.h"
#include "caption-data.h"
#include "caption-management.h"

/**
 * Bitmap data unit. The @c var1 and @c var2 fields are temporary.
 */
typedef struct CaptionSuperImposeBitmap_t {
  char *var1;   /**< Temporary: freed by caption_superimpose_bitmap_free(). */
  uint8_t var2; /**< Temporary. */

  const PES_pts_t *pts; /**< Points to PacketPES_t::pts; valid while the PacketPES_t exists. */
  struct CaptionSuperImposeBitmap_t *next; /**< Next item of the list, or NULL. */
} CaptionSuperImposeBitmap_t;

/**
 * Creates the struct from a bitmap data unit.
 *
 * @param m  Content read by caption_management_load(), of type
 *           @ref CAPTION_HANDLER_TYPE_BITMAP, with a non-empty data unit.
 * @return The struct, or NULL if out of memory. Free with
 *         caption_superimpose_bitmap_free().
 */
CaptionSuperImposeBitmap_t *caption_superimpose_bitmap_new (const CaptionManagement_t *m);

/**
 * Frees the struct created by caption_superimpose_bitmap_new(), including
 * all the items chained in @c next.
 *
 * @param cc  The struct. NULL is fine.
 */
void caption_superimpose_bitmap_free (CaptionSuperImposeBitmap_t *cc);

#endif /* CAPTION_SUPERIMPOSE_BITMAP_H_ */
