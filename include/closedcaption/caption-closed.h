/**
 * @file caption-closed.h
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @date Thu Aug 12 13:49:21 BRT 2010
 * @brief Interpreting the caption (closed caption) text and its state:
 *        colors, character size, position and display format.
 *
 * caption_closed_run() walks the @c statement body (text + control
 * commands) and:
 *
 * - converts each ISO-8859-15 character to UTF-8 and hands it to the text
 *   hook (see caption-hooking.h);
 * - runs each C0/C1 command (see caption-closed-controls.h), updates the
 *   CaptionClosed_t and calls the controls hook.
 */
#ifndef CAPTION_CLOSED_H_
#define CAPTION_CLOSED_H_

#include "caption-common.h"
#include "caption-data.h"
#include "caption-management.h"

/**
 * Character size (SSZ, MSZ, NSZ and SZX commands).
 */
typedef enum {
  CC_CharacterSize_Small = 0x01,    /**< Small (SSZ). */
  CC_CharacterSize_Middle = 0x02,   /**< Medium (MSZ). */
  CC_CharacterSize_Standard = 0x03, /**< Normal (NSZ). */
  CC_CharacterSize_t_eMax = 0x04,   /**< Limit of the values above. */

  CC_CharacterSize_Custom = 0x05,   /**< Size given by SZX; the value is in CaptionClosed_t::char_size_custom. */
  CC_CharacterSize_UNKNOWN = 0xff   /**< Unknown. */
} CC_CharacterSize_t;

/**
 * Display format (ARIB STD-B24, Table 9-7; CSI SWF command).
 */
typedef enum {
  CC_Format_HOR_STAN_DENSITY  = 0x00, /**< Horizontal, standard density. */
  CC_Format_VER_STAN_DENSITY  = 0x01, /**< Vertical, standard density. */
  CC_Format_HOR_HIGH_DENSITY  = 0x02, /**< Horizontal, high density. */
  CC_Format_VER_HIGH_DENSITY  = 0x03, /**< Vertical, high density. */
  CC_Format_HOR_WEST_LANG     = 0x04, /**< Horizontal, western languages. */
  CC_Format_HOR_1920x1080     = 0x05, /**< Horizontal, 1920x1080. */
  CC_Format_VER_1920x1080     = 0x06, /**< Vertical, 1920x1080. */
  CC_Format_HOR_960x540       = 0x07, /**< Horizontal, 960x540. */
  CC_Format_VER_960x540       = 0x08, /**< Vertical, 960x540. */
  CC_Format_HOR_720x480       = 0x09, /**< Horizontal, 720x480. */
  CC_Format_VER_720x480       = 0x0a, /**< Vertical, 720x480. */
  CC_Format_HOR_1280x720      = 0x0b, /**< Horizontal, 1280x720. */
  CC_Format_VER_1280x720      = 0x0c, /**< Vertical, 1280x720. */
  CC_Format_t_eMax            = 0x0d  /**< Number of formats. */
} CC_Format_t;

/**
 * Width and height, in points.
 */
typedef struct {
  uint16_t width;  /**< Width. */
  uint16_t height; /**< Height. */
} CC_Dimension_t;

/** Compares two CC_Dimension_t. */
#define CC_Dimension_is_equal(dimen1, dimen2) \
  ((dimen1).width == (dimen2).width && (dimen1).height == (dimen2).height)

/**
 * RGBA color.
 */
typedef struct {
  uint8_t r; /**< Red. */
  uint8_t g; /**< Green. */
  uint8_t b; /**< Blue. */
  uint8_t a; /**< Opacity: 0x00 is transparent, 0xff is opaque. */
} CC_Color_t;

/** Compares two CC_Color_t. */
#define CC_Color_is_equal(color1, color2) \
  ((color1).r == (color2).r && (color1).g == (color2).g && \
   (color1).b == (color2).b && (color1).a == (color2).a)

/**
 * Raster color (CSI RCS command, Raster Colour Command).
 */
typedef enum {
  CC_Rcs_BLACK = 0,                  /**< Black. */
  CC_Rcs_FULL_INTENSITY_RED = 1,     /**< Red. */
  CC_Rcs_FULL_INTENSITY_GREEN = 2,   /**< Green. */
  CC_Rcs_FULL_INTENSITY_YELLOW = 3,  /**< Yellow. */
  CC_Rcs_FULL_INTENSITY_BLUE = 4,    /**< Blue. */
  CC_Rcs_FULL_INTENSITY_MAGNETA = 5, /**< Magenta. */
  CC_Rcs_FULL_INTENSITY_CYAN = 6,    /**< Cyan. */
  CC_Rcs_FULL_INTENSITY_WHITE = 7,   /**< White. */
  CC_Rcs_TRANSPARENT = 8,            /**< Transparent. */
  Cc_Rcs_HALF_INTENSITY_RED = 9,     /**< Red, half intensity. */
  Cc_Rcs_HALF_INTENSITY_GREEN = 10,  /**< Green, half intensity. */
  Cc_Rcs_HALF_INTENSITY_YELLOW = 11, /**< Yellow, half intensity. */
  Cc_Rcs_HALF_INTENSITY_BLUE = 12,   /**< Blue, half intensity. */
  Cc_Rcs_HALF_INTENSITY_MAGNETA = 13,/**< Magenta, half intensity. */
  Cc_Rcs_HALF_INTENSITY_CYAN = 14,   /**< Cyan, half intensity. */
  Cc_Rcs_HALF_INTENSITY_WHITE = 15,  /**< White, half intensity. */
  CC_Rcs_t_eMax = 16                 /**< Number of colors. */
} CC_Rcs_t;

/**
 * Caption state, updated by the control commands during
 * caption_closed_run().
 *
 * Keep the same struct between the PES of the same PID: some commands
 * (colors, size, format) hold until they are changed again.
 * Initialize it with zeros.
 */
typedef struct {
  CC_Format_t format;           /**< Display format (CSI SWF). */
  CC_CharacterSize_t char_size; /**< Character size (SSZ, MSZ, NSZ, SZX). */
  CC_Dimension_t SDF;           /**< Size of the display area (CSI SDF, Set Display Format). */
  CC_Dimension_t SDP;           /**< Position of the display area (CSI SDP, Set Display Position). */
  CC_Dimension_t SSM;           /**< Character size in points (CSI SSM). */
  CC_Rcs_t RCS;                 /**< Raster color (CSI RCS). */
  CC_Color_t foreground;        /**< Text color (BKF ~ WHF, COL). */
  CC_Color_t background;        /**< Background color (COL). */
  CC_Color_t half_foreground;   /**< Text color, half intensity (COL). */
  CC_Color_t half_background;   /**< Background color, half intensity (COL). */

  /** Cursor position (APS, Active Position Set). */
  struct APS {
    uint8_t first;  /**< Row (0 to 63). */
    uint8_t second; /**< Column (0 to 63). */
  } APS; /**< Cursor position (APS). */

  bool need_clear;                /**< TRUE when the CS command (clear screen) was received. */
  uint8_t char_size_custom;       /**< SZX parameter, when @ref char_size is @ref CC_CharacterSize_Custom. */
  uint8_t char_number_in_oneline; /**< Characters per line (CSI SWF). */
  uint8_t char_line_number;       /**< Number of lines (CSI SWF). */
  uint16_t SHS;                   /**< Horizontal spacing (CSI SHS, Set Horizontal Spacing). */
  uint16_t SVS;                   /**< Vertical spacing (CSI SVS, Set Vertical Spacing). */

  const PES_pts_t *pts;           /**< PTS of the PES being processed; only valid during caption_closed_run(). */

  void *userdata;                 /**< Free for the user; handy to get the context back inside the hooks. */
} CaptionClosed_t;

/**
 * Processes the caption text.
 *
 * Walks the @c statement body of @p m, handing each character (in UTF-8)
 * to the text hook and running each control command (which updates @p cc
 * and calls the controls hook). See caption-hooking.h.
 *
 * Characters outside ISO-8859-15 and commands not supported in closed caption
 * are ignored, along with their parameters.
 *
 * @param m   Content of type @ref CAPTION_HANDLER_TYPE_STATEMENT_BODY, read
 *            by caption_management_load().
 * @param cc  Caption state, kept between calls.
 * @return TRUE.
 */
bool caption_closed_run (const CaptionManagement_t *m,
                         CaptionClosed_t *cc);

/**
 * Converts an ISO-8859-15 (Latin-9) character to UTF-8.
 *
 * @param from    Source character (1 byte).
 * @param to      Receives the UTF-8 character, '\\0' terminated.
 * @param to_len  Size of @p to; must be 4 or more.
 * @return Number of bytes written in @p to, without the '\\0' (1 to 3).
 */
uint8_t char_latin9_to_utf8 (const char *const from,
                             char *const to,
                             uint8_t to_len);

#endif /* CAPTION_CLOSED_H_ */
