/**
 * @file caption-superimpose-geometric.h
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @date Thu Aug 12 13:49:21 BRT 2010
 * @brief Geometric graphics data unit, split into commands.
 *
 * Reads the data units of type @ref CAPTION_HANDLER_TYPE_GEOMETRIC. The geometric graphics
 * are a sequence of "graphics display commands" (ARIB STD-B24, Volume 1, Part 2, 8.1.2):
 * an opcode (0x20 to 0x3f) followed by zero or more operand bytes (0x40 to 0x7f). This module only
 * splits the stream into commands; drawing them (DOMAIN, POINT, LINE, ARC, RECT, POLY...) is up to
 * the application. C0/C1 controls and bytes outside these two ranges are skipped.
 */
#ifndef CAPTION_SUPERIMPOSE_GEOMETRIC_H_
#define CAPTION_SUPERIMPOSE_GEOMETRIC_H_

#include "caption-common.h"
#include "caption-data.h"
#include "caption-management.h"

#define CAPTION_GEOMETRIC_OPCODE_RESET        0x20 /**< RESET: 2 fixed bytes. */
#define CAPTION_GEOMETRIC_OPCODE_DOMAIN       0x21 /**< DOMAIN: 1 fixed byte and a multi-value operand. */
#define CAPTION_GEOMETRIC_OPCODE_TEXTURE      0x23 /**< TEXTURE: 1 fixed byte. */
#define CAPTION_GEOMETRIC_OPCODE_SET_COLOR    0x3c /**< SET COLOR. */
#define CAPTION_GEOMETRIC_OPCODE_SET_PATTERN  0x3d /**< SET PATTERN. */
#define CAPTION_GEOMETRIC_OPCODE_SELECT_COLOR 0x3e /**< SELECT COLOR. */
#define CAPTION_GEOMETRIC_OPCODE_BLINK        0x3f /**< BLINK. */

/**
 * One graphics display command.
 */
typedef struct CaptionSuperImposeGeometric_t {
  uint8_t opcode;     /**< Opcode (0x20 to 0x3f). The low bits tell the variant (absolute/relative,
                           outlined/filled, set-and-draw). */
  uint8_t *operands;  /**< Operand bytes (0x40 to 0x7f); NULL if there is none. */
  size_t operands_len; /**< Size of @c operands. */

  const PES_pts_t *pts; /**< Points to PacketPES_t::pts; valid while the PacketPES_t exists. */
  struct CaptionSuperImposeGeometric_t *next; /**< Next command of the list, or NULL. */
} CaptionSuperImposeGeometric_t;

/**
 * Creates the list of commands from a geometric graphics data unit.
 *
 * @param m  Content read by caption_management_load() or caption_management_load_unit(), of type
 *           @ref CAPTION_HANDLER_TYPE_GEOMETRIC, with a non-empty data unit.
 * @return The list, or NULL if out of memory or if there is no command. Free with
 *         caption_superimpose_geometric_free().
 */
CaptionSuperImposeGeometric_t *caption_superimpose_geometric_new (const CaptionManagement_t *m);

/**
 * Name of the command family of an opcode ("RESET", "DOMAIN", "TEXTURE", "POINT", "LINE", "ARC",
 * "RECT", "POLY", "SET COLOR", "SET PATTERN", "SELECT COLOR" or "BLINK").
 *
 * @param opcode  Opcode of the command.
 * @return The name, or "UNKNOWN". Never NULL.
 */
const char *caption_superimpose_geometric_get_name (uint8_t opcode);

/**
 * Frees the list created by caption_superimpose_geometric_new(), including
 * all the items chained in @c next.
 *
 * @param cc  The list. NULL is fine.
 */
void caption_superimpose_geometric_free (CaptionSuperImposeGeometric_t *cc);

#endif /* CAPTION_SUPERIMPOSE_GEOMETRIC_H_ */
