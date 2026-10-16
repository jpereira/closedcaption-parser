/**
 * @file caption-superimpose-2bytedrcs.h
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @date Thu Aug 12 13:49:21 BRT 2010
 * @brief 2-byte DRCS (characters drawn by the broadcaster) data unit. <b>Not implemented</b>.
 *
 * Skeleton to handle the data units of type
 * @ref CAPTION_HANDLER_TYPE_2BYTE_DRCS. For now it just allocates the struct, without
 * parsing the data (with @c -DDEBUG_CLOSEDCAPTION and
 * @c -DDEBUG_CAPTION_SUPERIMPOSE_2BYTEDRCS, it prints the data unit dump).
 */
#ifndef CAPTION_SUPERIMPOSE_2BYTEDRCS_H_
#define CAPTION_SUPERIMPOSE_2BYTEDRCS_H_

#include "caption-common.h"
#include "caption-data.h"
#include "caption-management.h"

/**
 * 2-byte DRCS (characters drawn by the broadcaster) data unit. The @c var1 and @c var2 fields are temporary.
 */
typedef struct CaptionSuperImpose2ByteDrcs_t {
  char *var1;   /**< Temporary: freed by caption_superimpose_2bytedrcs_free(). */
  uint8_t var2; /**< Temporary. */

  const PES_pts_t *pts; /**< Points to PacketPES_t::pts; valid while the PacketPES_t exists. */
  struct CaptionSuperImpose2ByteDrcs_t *next; /**< Next item of the list, or NULL. */
} CaptionSuperImpose2ByteDrcs_t;

/**
 * Creates the struct from a 2-byte DRCS (characters drawn by the broadcaster) data unit.
 *
 * @param m  Content read by caption_management_load(), of type
 *           @ref CAPTION_HANDLER_TYPE_2BYTE_DRCS, with a non-empty data unit.
 * @return The struct, or NULL if out of memory. Free with
 *         caption_superimpose_2bytedrcs_free().
 */
CaptionSuperImpose2ByteDrcs_t *caption_superimpose_2bytedrcs_new (const CaptionManagement_t *m);

/**
 * Frees the struct created by caption_superimpose_2bytedrcs_new(), including
 * all the items chained in @c next.
 *
 * @param cc  The struct. NULL is fine.
 */
void caption_superimpose_2bytedrcs_free (CaptionSuperImpose2ByteDrcs_t *cc);

#endif /* CAPTION_SUPERIMPOSE_2BYTEDRCS_H_ */
