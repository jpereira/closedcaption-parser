/**
 * @file caption-hooking.h
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @date Thu Aug 12 13:49:21 BRT 2010
 * @brief Registering the callbacks (hooks) that get the text and the caption
 *        commands while caption_closed_run() runs.
 *
 * During caption_closed_run(), each text character is converted to
 * UTF-8 and handed to the text hook, and each recognized C0/C1 control
 * command is handed to the controls hook.
 *
 * Example:
 * @code
 * static void
 * on_string (CaptionClosed_t *cc, const char *utf8, const uint8_t len)
 * {
 *   fwrite (utf8, 1, len, stdout);
 * }
 *
 * static void
 * on_controls (CaptionClosed_t *cc, const uint8_t cmd)
 * {
 *   if (cmd == CTRL_C0_APR) // Active Position Return: new line
 *     putchar ('\n');
 * }
 *
 * caption_hooking_set_string (on_string);
 * caption_hooking_set_controls (on_controls);
 * @endcode
 *
 * @warning The hooks are global: there is only one of each kind for the whole
 *          process, and they must not be changed while another thread
 *          is running caption_closed_run(). Use CaptionClosed_t::userdata to
 *          find the context inside the hook.
 */
#ifndef CAPTION_HOOKING_H_
#define CAPTION_HOOKING_H_

#include "caption-common.h"
#include "caption-closed.h"

/**
 * Hook called for each recognized control command.
 *
 * @param cc   Caption state, already updated by the command.
 * @param cmd  Command code: C0 (@ref CaptionClosed_CtrlCodes_C0), C1
 *             (@ref CaptionClosed_CtrlCodes_C1) or, for the CSI, the code of the
 *             subcommand (@ref CaptionClosed_CtrlCodes_C1_CSI).
 */
typedef void (*CaptionHookingControls_t) (CaptionClosed_t *cc,
                                          const uint8_t cmd);

/**
 * Hook called for each text character.
 *
 * @param cc      Caption state (colors, size, position) at the time of the character.
 * @param string  UTF-8 character. Not '\\0' terminated.
 * @param len     Number of bytes in @p string (1 to 3).
 */
typedef void (*CaptionHookingString_t) (CaptionClosed_t *cc,
                                        const char *string,
                                        const uint8_t len);

/**
 * Registers the control commands hook (C0/C1).
 *
 * @param callback  The hook, or NULL to turn it off.
 */
void caption_hooking_set_controls (CaptionHookingControls_t callback);

/**
 * Registers the text hook.
 *
 * @param callback  The hook, or NULL to turn it off.
 */
void caption_hooking_set_string (CaptionHookingString_t callback);

#endif /* CAPTION_HOOKING_H_ */
