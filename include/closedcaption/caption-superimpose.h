/**
 * @file caption-superimpose.h
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @date Thu Aug 12 13:49:21 BRT 2010
 * @brief Superimpose captions. <b>Not implemented</b>.
 *
 * Superimpose are the messages overlaid on the video that are not part of the
 * program caption (broadcaster notices, alerts). They use the same
 * @c data_group format as the caption (ARIB STD-B24, Volume 3, Chapter 9),
 * but on their own PID (@c component_tag 0x38 to 0x3f).
 *
 * For now it just allocates the struct, without parsing the data.
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
  const uint8_t *data;  /**< data_group data. Not filled. */
  char *text_utf8;      /**< UTF-8 text, freed by caption_superimpose_free(). Not filled. */
  size_t len;           /**< Size of @ref text_utf8. */

  CaptionSuperImpose_t *next; /**< Next message of the same CaptionData_t, or NULL. */
};

/**
 * Creates the struct from a superimpose @c data_group.
 *
 * @param caption  @c data_group loaded by caption_data_load(), not empty.
 * @return The struct, or NULL if out of memory. Free with
 *         caption_superimpose_free().
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
