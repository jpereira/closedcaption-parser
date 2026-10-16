/**
 * @file caption-superimpose-1bytedrcs.h
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @date Thu Aug 12 13:49:21 BRT 2010
 * @brief 1-byte DRCS (characters drawn by the broadcaster) data unit.
 *
 * Reads the data units of type @ref CAPTION_HANDLER_TYPE_1BYTE_DRCS, following the
 * "DRCS structure syntax" (ARIB STD-B24, Volume 1, Part 2, Annex D). Each
 * character/font sent in the data unit becomes one item of the list.
 *
 * Example (prints the glyphs as ASCII art):
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

#define CAPTION_DRCS_MODE_2_GRADATION  0x00 /**< 2 gradations, no compression: @c pixels is filled. */
#define CAPTION_DRCS_MODE_MULTI_GRAD   0x01 /**< Several gradations, no compression: @c pixels is filled. */
#define CAPTION_DRCS_MODE_2_COLOR_GEO  0x02 /**< Geometric data (2 colors): @c geometric is filled. */
#define CAPTION_DRCS_MODE_MULTI_COLOR  0x03 /**< Geometric data (multi-color): @c geometric is filled. */

/**
 * One font (typeface) of one DRCS character. A character sent with more than one font
 * becomes more than one item, all with the same @c character_code.
 */
typedef struct CaptionSuperImpose1ByteDrcs_t {
  uint16_t character_code; /**< Code of the character. 1 byte DRCS: 0x41xx (DRCS-1) to 0x4Fxx (DRCS-15),
                                the 2nd byte (0x21 to 0x7e) is the position in the set. */
  uint8_t font_id;         /**< Typeface (0 to 15); 0 means "any". */
  uint8_t mode;            /**< Transmission mode, see @ref CAPTION_DRCS_MODE_2_GRADATION and the next ones. */

  uint8_t depth;           /**< Number of gradations minus 2 (0: 2 gradations). Only modes 0 and 1. */
  uint8_t width;           /**< Width of the pattern, in pixels. Only modes 0 and 1. */
  uint8_t height;          /**< Height of the pattern, in pixels. Only modes 0 and 1. */
  uint8_t *pixels;         /**< @c width x @c height pixels, one byte each, left to right and top to
                                bottom. 0 is the background, @c depth + 1 is the foreground. NULL in modes
                                2 and 3, or when the pattern is empty. */

  uint8_t region_x;        /**< Logical area (1/256 unit) used by the geometric data. Only modes 2 and 3. */
  uint8_t region_y;        /**< Idem. */
  uint8_t *geometric;      /**< Geometric data, as it came (see caption-superimpose-geometric.h). NULL when
                                there is none. Only modes 2 and 3. */
  uint16_t geometric_len;  /**< Size of @c geometric. */

  const PES_pts_t *pts; /**< Points to PacketPES_t::pts; valid while the PacketPES_t exists. */
  struct CaptionSuperImpose1ByteDrcs_t *next; /**< Next item of the list, or NULL. */
} CaptionSuperImpose1ByteDrcs_t;

/**
 * Creates the list from a 1-byte DRCS (characters drawn by the broadcaster) data unit.
 *
 * @param m  Content read by caption_management_load() or caption_management_load_unit(), of type
 *           @ref CAPTION_HANDLER_TYPE_1BYTE_DRCS, with a non-empty data unit.
 * @return The list, or NULL if out of memory, if the data unit has no character or if it is
 *         truncated/invalid. Free with caption_superimpose_1bytedrcs_free().
 */
CaptionSuperImpose1ByteDrcs_t *caption_superimpose_1bytedrcs_new (const CaptionManagement_t *m);

/**
 * Frees the list created by caption_superimpose_1bytedrcs_new(), including
 * all the items chained in @c next.
 *
 * @param cc  The list. NULL is fine.
 */
void caption_superimpose_1bytedrcs_free (CaptionSuperImpose1ByteDrcs_t *cc);

#endif /* CAPTION_SUPERIMPOSE_1BYTEDRCS_H_ */
