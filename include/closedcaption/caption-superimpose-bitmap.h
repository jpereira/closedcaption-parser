/**
 * @file caption-superimpose-bitmap.h
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @date Thu Aug 12 13:49:21 BRT 2010
 * @brief Bitmap data unit (a PNG image with position and flashing colors).
 *
 * Reads the data units of type @ref CAPTION_HANDLER_TYPE_BITMAP, following the "bitmap
 * graphics coding" (ARIB STD-B24, Volume 1, Part 3, 6.2, Table 6-1, p. 156): position header, flashing
 * header and the PNG file. The library does not decode the PNG: it validates the signature,
 * reads the size from the IHDR chunk and hands the bytes to be used with any PNG decoder.
 */
#ifndef CAPTION_SUPERIMPOSE_BITMAP_H_
#define CAPTION_SUPERIMPOSE_BITMAP_H_

#include "caption-common.h"
#include "caption-data.h"
#include "caption-management.h"

/**
 * Bitmap data unit.
 */
typedef struct CaptionSuperImposeBitmap_t {
  int16_t x;                 /**< X of the upper left corner, on the display area (may be negative). */
  int16_t y;                 /**< Y of the upper left corner, on the display area (may be negative). */

  uint8_t num_flc_colors;    /**< Number of colors that flash. */
  uint8_t *flc_colors;       /**< Palette index of each flashing color; NULL if @c num_flc_colors is 0. */

  uint8_t *png;              /**< The PNG file, copied from the data unit. */
  size_t png_len;            /**< Size of @c png. */
  uint32_t width;            /**< Width of the image, in pixels (from the PNG IHDR). */
  uint32_t height;           /**< Height of the image, in pixels (from the PNG IHDR). */

  const PES_pts_t *pts; /**< Points to PacketPES_t::pts; valid while the PacketPES_t exists. */
  struct CaptionSuperImposeBitmap_t *next; /**< Always NULL: a bitmap data unit has a single image. */
} CaptionSuperImposeBitmap_t;

/**
 * Creates the struct from a bitmap data unit.
 *
 * @param m  Content read by caption_management_load() or caption_management_load_unit(), of type
 *           @ref CAPTION_HANDLER_TYPE_BITMAP, with a non-empty data unit.
 * @return The struct, or NULL if out of memory or if the data unit is truncated or the PNG is not
 *         valid (signature and IHDR). Free with caption_superimpose_bitmap_free().
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
