/**
 * @file caption-superimpose-2bytedrcs.h
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @date Thu Aug 12 13:49:21 BRT 2010
 * @brief 2-byte DRCS (characters drawn by the broadcaster) data unit.
 *
 * Reads the data units of type @ref CAPTION_HANDLER_TYPE_2BYTE_DRCS. The syntax is the same
 * as the 1-byte DRCS one (ARIB STD-B24, Volume 1, Part 2, Annex D); only the meaning of the
 * @c character_code changes: here the two bytes are the code of the supplemental character
 * (Gaiji) itself. So the struct is the same of caption-superimpose-1bytedrcs.h.
 */
#ifndef CAPTION_SUPERIMPOSE_2BYTEDRCS_H_
#define CAPTION_SUPERIMPOSE_2BYTEDRCS_H_

#include "caption-common.h"
#include "caption-data.h"
#include "caption-management.h"
#include "caption-superimpose-1bytedrcs.h"

/**
 * One font of one 2-byte DRCS character: same fields as CaptionSuperImpose1ByteDrcs_t, but
 * @c character_code is the 2-byte code of the character.
 */
typedef CaptionSuperImpose1ByteDrcs_t CaptionSuperImpose2ByteDrcs_t;

/**
 * Creates the list from a 2-byte DRCS (characters drawn by the broadcaster) data unit.
 *
 * @param m  Content read by caption_management_load() or caption_management_load_unit(), of type
 *           @ref CAPTION_HANDLER_TYPE_2BYTE_DRCS, with a non-empty data unit.
 * @return The list, or NULL if out of memory, if the data unit has no character or if it is
 *         truncated/invalid. Free with caption_superimpose_2bytedrcs_free().
 */
CaptionSuperImpose2ByteDrcs_t *caption_superimpose_2bytedrcs_new (const CaptionManagement_t *m);

/**
 * Frees the list created by caption_superimpose_2bytedrcs_new(), including
 * all the items chained in @c next.
 *
 * @param cc  The list. NULL is fine.
 */
void caption_superimpose_2bytedrcs_free (CaptionSuperImpose2ByteDrcs_t *cc);

#endif /* CAPTION_SUPERIMPOSE_2BYTEDRCS_H_ */
