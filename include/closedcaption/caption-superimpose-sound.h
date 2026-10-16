/**
 * @file caption-superimpose-sound.h
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @date Thu Aug 12 13:49:21 BRT 2010
 * @brief Synthesized sound data unit.
 *
 * Reads the data units of type @ref CAPTION_HANDLER_TYPE_SYNTHESIZED_SOUND. The coding of the
 * synthesized sound is not defined by ARIB STD-B24, it just points to ARIB STD-B5 (ARIB STD-B24,
 * Volume 1, Part 3, 7.3, p. 157). So the library only validates the data unit
 * and hands the bytes as they came, to be decoded by the application.
 */
#ifndef CAPTION_SUPERIMPOSE_SOUND_H_
#define CAPTION_SUPERIMPOSE_SOUND_H_

#include "caption-common.h"
#include "caption-data.h"
#include "caption-management.h"

/**
 * Synthesized sound data unit.
 */
typedef struct CaptionSuperImposeSound_t {
  uint8_t *data;  /**< Synthesized sound data (ARIB STD-B5 coding), copied from the data unit. */
  size_t len;     /**< Size of @c data. */

  const PES_pts_t *pts; /**< Points to PacketPES_t::pts; valid while the PacketPES_t exists. */
  struct CaptionSuperImposeSound_t *next; /**< Always NULL: a sound data unit has a single item. */
} CaptionSuperImposeSound_t;

/**
 * Creates the struct from a synthesized sound data unit.
 *
 * @param m  Content read by caption_management_load() or caption_management_load_unit(), of type
 *           @ref CAPTION_HANDLER_TYPE_SYNTHESIZED_SOUND, with a non-empty data unit.
 * @return The struct, or NULL if out of memory. Free with caption_superimpose_sound_free().
 */
CaptionSuperImposeSound_t *caption_superimpose_sound_new (const CaptionManagement_t *m);

/**
 * Frees the struct created by caption_superimpose_sound_new(), including
 * all the items chained in @c next.
 *
 * @param cc  The struct. NULL is fine.
 */
void caption_superimpose_sound_free (CaptionSuperImposeSound_t *cc);

#endif /* CAPTION_SUPERIMPOSE_SOUND_H_ */
