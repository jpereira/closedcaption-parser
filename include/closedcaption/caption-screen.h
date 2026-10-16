/**
 * @file caption-screen.h
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @brief Turns the caption PES packets into lines of text.
 *
 * This is the easiest way to use the library: you give it the whole PES of the
 * caption PID (see ts-demux.h), it does all the processing
 * (packet_pes_load(), caption_data_load(), caption_management_load() and
 * caption_closed_run()) and hands every ready caption line, in UTF-8,
 * to a callback.
 *
 * <b>How the lines are built</b>
 *
 * - The APR (0x0d) and APS (0x1c) commands break the line; the padding spaces
 *   at the end of each line are removed.
 * - Each PES redraws the whole screen. Live captions (roll-up) resend the
 *   same line every few letters, so a line is only delivered when it
 *   leaves the screen, that is, when no line of the new screen is a
 *   continuation of it.
 * - The comparison ignores accents: the broadcaster often sends the letter without
 *   the accent and fixes it on the next screen (e.g. "ESPECI" → "ESPECÍFICO").
 * - The time of each line is the PTS of the PES where it showed up, relative to the
 *   first caption PTS received.
 *
 * Example:
 * @code
 * static void
 * on_line (const CaptionLine_t *line, void *userdata)
 * {
 *   printf ("[%u ms] %s\n", line->start_ms, line->text);
 * }
 *
 * CaptionScreen_t *screen = caption_screen_new (on_line, NULL);
 *
 * // for each whole PES of the caption PID:
 * caption_screen_push_pes (screen, pes, len);
 *
 * caption_screen_flush (screen); // delivers the lines that are still on the screen
 * caption_screen_free (screen);
 * @endcode
 *
 * @warning The hooks from caption-hooking.h are global: caption_screen_push_pes()
 *          registers its own hooks, replacing whatever is registered.
 *          For the same reason, it is not safe to use the library on
 *          more than one thread at the same time.
 */
#ifndef CAPTION_SCREEN_H_
#define CAPTION_SCREEN_H_

#include "caption-common.h"

/** Max number of lines kept on the screen. */
#define CAPTION_SCREEN_MAX_LINES 16

/** Max size of a line, in UTF-8 bytes (including the '\\0'). */
#define CAPTION_SCREEN_LINE_LEN  256

/**
 * A caption line.
 */
typedef struct {
  const char *text;  /**< UTF-8 text, without the padding spaces at the end. */
  uint32_t start_ms; /**< When the line showed up, in milliseconds, relative to the first caption PTS. */
} CaptionLine_t;

/**
 * Callback called for each finished caption line.
 *
 * @param line      The line. Only valid during the call.
 * @param userdata  Pointer given to caption_screen_new().
 */
typedef void (*CaptionScreenLineCallback_t) (const CaptionLine_t *line,
                                             void *userdata);

/**
 * Caption screen. Opaque struct, created by caption_screen_new().
 */
typedef struct CaptionScreen_s CaptionScreen_t;

/**
 * Creates a caption screen.
 *
 * @param callback  Called for each finished line.
 * @param userdata  Passed on to the @p callback.
 * @return The screen, or NULL if out of memory. Free with caption_screen_free().
 */
CaptionScreen_t *caption_screen_new (CaptionScreenLineCallback_t callback,
                                     void *userdata);

/**
 * Frees the screen. Does not call caption_screen_flush().
 *
 * @param screen  Screen created by caption_screen_new(). NULL is fine.
 */
void caption_screen_free (CaptionScreen_t *screen);

/**
 * Processes a whole PES from the caption PID.
 *
 * Invalid PES, with a bad CRC, or that are not "caption statement data"
 * (for example, the "caption management data") are ignored.
 *
 * @param screen  Screen created by caption_screen_new().
 * @param pes     Whole PES, starting at the @c packet_start_code_prefix (00 00 01).
 * @param len     Size of @p pes in bytes.
 * @return TRUE if the PES had caption text and was processed.
 */
bool caption_screen_push_pes (CaptionScreen_t *screen,
                              const uint8_t *pes,
                              size_t len);

/**
 * Delivers all the lines that are still on the screen and clears the screen.
 * Call it at the end of the file.
 *
 * @param screen  Screen created by caption_screen_new().
 */
void caption_screen_flush (CaptionScreen_t *screen);

/**
 * Checks if @p line starts with @p prefix, ignoring the accents of the Latin-1
 * letters (U+00C0 to U+00FF). For example, "ESPECÍFICO" starts with
 * "ESPECI".
 *
 * @param line    UTF-8 text.
 * @param prefix  UTF-8 prefix.
 * @return TRUE if @p line starts with @p prefix.
 */
bool caption_screen_line_continues (const char *line,
                                    const char *prefix);

#endif /* CAPTION_SCREEN_H_ */
