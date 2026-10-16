/**
 * @file caption-hooking-priv.h
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @date Thu Aug 12 13:49:21 BRT 2010
 * @brief Internal use: calling the hooks registered in caption-hooking.h.
 *
 * These functions are used by caption-closed.c and
 * caption-closed-controls.c and are not part of the public API.
 */
#ifndef CAPTION_HOOKING_PRIV_H_
#define CAPTION_HOOKING_PRIV_H_

#include "caption-common.h"
#include "caption-closed.h"

/**
 * Calls the controls hook, if there is one registered.
 *
 * @param caption  Caption state.
 * @param cmd      Command code.
 */
void caption_hooking_priv_run_controls (CaptionClosed_t *caption,
                                        const uint8_t cmd);

/**
 * Calls the text hook, if there is one registered.
 *
 * @param caption  Caption state.
 * @param string   UTF-8 character.
 * @param len      Number of bytes in @p string.
 */
void caption_hooking_priv_run_string (CaptionClosed_t *caption,
                                      const char *string,
                                      uint8_t *len);

#endif /* CAPTION_HOOKING_PRIV_H_ */
