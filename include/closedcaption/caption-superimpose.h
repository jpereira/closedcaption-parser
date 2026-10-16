/**
 * @file caption-superimpose.h
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @date Thu Aug 12 13:49:21 BRT 2010
 * @brief Superimpose captions.
 *
 * Superimpose are the messages overlaid on the video that are not part of the
 * program caption (broadcaster notices, alerts). They use the same
 * @c data_group format as the caption (ARIB STD-B24, Volume 3, Chapter 9),
 * but on their own PID (@c component_tag 0x38 to 0x3f).
 *
 * Each text data unit (@ref CAPTION_HANDLER_TYPE_STATEMENT_BODY) of the data group becomes one
 * message, with the text converted to UTF-8 like caption_closed_run() does.
 *
 * @note While it runs, caption_superimpose_new() replaces the global hooks of
 *       caption-hooking.h and puts the previous ones back before returning.
 */
#ifndef CAPTION_SUPERIMPOSE_H_
#define CAPTION_SUPERIMPOSE_H_

#include "caption-common.h"
#include "caption-data.h"

/** Superimpose message. See @ref CaptionSuperImpose_s. */
typedef struct CaptionSuperImpose_s CaptionSuperImpose_t;

/**
 * Superimpose message.
 */
struct CaptionSuperImpose_s {
  const uint8_t *data;  /**< data_group data (CaptionData_t::data); valid while the PacketPES_t exists. */
  char *text_utf8;      /**< UTF-8 text, '\\0' terminated, freed by caption_superimpose_free(). */
  size_t len;           /**< Size of @ref text_utf8, without the '\\0'. */

  CaptionSuperImpose_t *next; /**< Next message of the same CaptionData_t, or NULL. */
};

/**
 * Creates the struct from a superimpose @c data_group.
 *
 * @param caption  @c data_group loaded by caption_data_load(), not empty.
 * @return The first message (the others are chained in @c next), or NULL if out of memory, if
 *         the data group is invalid or if it has no text. Free with caption_superimpose_free().
 */
CaptionSuperImpose_t *caption_superimpose_new (const CaptionData_t *caption);

/**
 * Frees the struct created by caption_superimpose_new(), including all the
 * messages chained in @c next.
 *
 * @param csi  The struct. NULL is fine.
 */
void caption_superimpose_free (CaptionSuperImpose_t *csi);

#endif /* CAPTION_SUPERIMPOSE_H_ */
