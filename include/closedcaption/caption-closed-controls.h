/**
 * @file caption-closed-controls.h
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @date Thu Aug 12 13:49:21 BRT 2010
 * @brief C0, C1 and CSI control commands of the caption text.
 *
 * The caption text mixes characters and control commands (ARIB STD-B24,
 * Volume 1, 7.2; ABNT NBR 15606-1; ARIB TR-B14, 6.5):
 *
 * - <b>C0</b> (0x08 to 0x1f): cursor position (APR, APS...), clear screen
 *   (CS), special characters (SS3) etc.;
 * - <b>C1</b> (0x80 to 0x9f): colors (BKF ~ WHF, COL), character size
 *   (SSZ, MSZ, NSZ, SZX) etc.;
 * - <b>CSI</b> (0x9b): extended sequences, in the format
 *   <tt>CSI P1 ; P2 ; ... 0x20 F</tt>, where the parameters are decimal
 *   numbers in ASCII and the final byte @c F tells the command (SWF, SDF,
 *   SDP...).
 *
 * Each command has a <tt>callback_controls_*</tt> callback, called by
 * caption_closed_controls_load(). It reads the parameters, updates the
 * CaptionClosed_t and moves the read position past the parameters. When the
 * callback returns TRUE, the controls hook is called (see
 * caption-hooking.h).
 *
 * Commands that should not be used in closed caption (only in
 * superimpose) return FALSE, but still skip their parameters, so
 * they are not handled as text.
 *
 * Example of caption text (TV Globo):
 * @verbatim
 * 00000000  0c 9b 37 20 53 9b 31 37  36 3b 39 39 20 5f 9b 36 |..7.S.176;99._.6|
 * 00000010  30 38 3b 33 39 30 20 56  9b 33 36 3b 33 36 20 57 |08;390.V.36;36.W|
 * 00000020  9b 32 20 58 9b 31 36 20  59 9b 38 20 6e 88 1c 4c |.2.X.16.Y.8.n..L|
 * 00000030  40 87 90 50 5b 4d 41 54  45 55 53 5d 20 4f 20 43 |@..P[MATEUS].O.C|
 * 00000040  41 4c 49 58 54 4f 20 56  41 49 0d 52 45 53 4f 4c |ALIXTO.VAI.RESOL|
 * 00000050  56 45 52 20 4f 20 50 52  4f 42 4c 45 4d 41 20 44 |VER.O.PROBLEMA.D|
 * 00000060  4f 20 43 41 52 52 4f 2c  0d 4f 20 4d 41 49 53 20 |O.CARRO,.O.MAIS.|
 * 00000070  52 c1 50 49 44 4f 20 50  4f 53 53 cd 56 45 4c 2e |R.PIDO.POSS.VEL.|
 *
 * 0c              CS: clear screen
 * 9b 37 20 53     CSI SWF(7): 960x540 horizontal format
 * 9b ... 20 5f    CSI SDP(176;99): position of the display area
 * 9b ... 20 56    CSI SDF(608;390): size of the display area
 * 9b ... 20 57    CSI SSM(36;36): character size
 * 9b 32 20 58     CSI SHS(2): horizontal spacing
 * 9b 31 36 20 59  CSI SVS(16): vertical spacing
 * 9b 38 20 6e     CSI RCS(8): transparent raster color
 * 88              SSZ: small characters
 * 1c 4c 40        APS(12, 0): cursor at row 12, column 0
 * 87              WHF: white text
 * 90 50           COL: black background
 * 0d              APR: new line
 * c1, cd          'Á' and 'Í' in ISO-8859-15
 * @endverbatim
 */
#ifndef CAPTION_CLOSED_CONTROLS_H_
#define CAPTION_CLOSED_CONTROLS_H_

#include "caption-common.h"
#include "caption-data.h"
#include "caption-closed.h"
#include "caption-management.h"

/**
 * Includes the CaptionClosedControls_t::code_name field in the command tables,
 * used by caption_closed_controls_get_name() in the debug messages.
 */
#define DEBUG_CAPTION_CLOSED_CONTROLCODE

/** Code of a control command (C0, C1 or CSI). */
typedef uint8_t CaptionClosed_CtrlCode_t;

/**
 * Data passed on to the command callbacks.
 */
typedef struct {
  const uint8_t *argv;     /**< Caption text buffer. */
  const uint16_t argc;     /**< Number of bytes available from @ref pos, including the command itself. */
  uint16_t *pos;           /**< Position of the command in @ref argv; the callback moves past the parameters. NULL in the CSI subcommands. */
  CaptionClosed_t *caption;/**< Caption state, updated by the callback. */
  void *data;              /**< Extra data: in the CSI subcommands, it points to the parameters (struct csi_arg_Pn). */
  CaptionClosed_CtrlCode_t current_code; /**< Command code; the CSI swaps it for the subcommand code. */
} CC_ControlCodes_CallbackData_t;

/**
 * Entry of the command tables: code and callback.
 */
typedef struct {
  const CaptionClosed_CtrlCode_t code; /**< Command code (ABNT NBR 15608-3:2008, Table 2). */

  /** Callback that runs the command, or NULL for unused codes. */
  bool (*callback)(const CC_ControlCodes_CallbackData_t *d);

#ifdef DEBUG_CAPTION_CLOSED_CONTROLCODE
  const char *code_name; /**< Command name, for debugging (e.g. "CTRL_C0_APS"). */
#endif /* DEBUG_CAPTION_CLOSED_CONTROLCODE */
} CaptionClosedControls_t;

#ifdef DEBUG_CAPTION_CLOSED_CONTROLCODE
/** Builds an entry of the command tables. */
# define CAPTION_CTRL_SET(code, callback) { code, callback, #code }
#else
# define CAPTION_CTRL_SET(code, callback) { code, callback }
#endif /* DEBUG_CAPTION_CLOSED_CONTROLCODE */

/**
 * Name of a command, for debug messages.
 *
 * @param code  C0, C1 or CSI subcommand code.
 * @return The name (e.g. "CTRL_C0_APS"), or "UNKNOWN".
 */
const char *
caption_closed_controls_get_name (const uint8_t code);

/**
 * C0 commands (ARIB TR-B14, Table 3.11; ARIB STD-B24, Table 7-14).
 *
 * Unused values are filled with @c PAD, so the code minus
 * 0x08 is the direct index into the callbacks table.
 */
enum CaptionClosed_CtrlCodes_C0 {
#define CTRL_C0_NULL 0x00 /**< NUL: ignored. */
#define CTRL_C0_BELL 0x07 /**< BEL: ignored in closed caption. */
  CTRL_C0_APB = 0x08, /**< Active Position Backward: moves the cursor back. */
  CTRL_C0_APF = 0x09, /**< Active Position Forward: moves the cursor forward. */
  CTRL_C0_APD = 0x0a, /**< Active Position Down: moves the cursor down. */
  CTRL_C0_APU = 0x0b, /**< Active Position Up: moves the cursor up. */
  CTRL_C0_CS  = 0x0c, /**< Clear Screen: clears the screen. */
  CTRL_C0_APR = 0x0d, /**< Active Position Return: start of the next line. */
  CTRL_C0_LS1 = 0x0e, /**< Locking Shift 1. */
  CTRL_C0_LS0 = 0x0f, /**< Locking Shift 0. */

  CTRL_C0_PAD_0x01 = 0x10, /**< Unused (0x10). */
  CTRL_C0_PAD_0x02 = 0x11, /**< Unused (0x11). */
  CTRL_C0_PAD_0x03 = 0x12, /**< Unused (0x12). */
  CTRL_C0_PAD_0x04 = 0x13, /**< Unused (0x13). */
  CTRL_C0_PAD_0x05 = 0x14, /**< Unused (0x14). */
  CTRL_C0_PAD_0x06 = 0x15, /**< Unused (0x15). */
  CTRL_C0_PAPF     = 0x16, /**< Parameterized Active Position Forward: moves P1 positions forward. */
  CTRL_C0_PAD_0x17 = 0x17, /**< Unused. */

  CTRL_C0_CAN      = 0x18, /**< Cancel. */
  CTRL_C0_SS2      = 0x19, /**< Single Shift 2: the next character comes from the G2 set. */
  CTRL_C0_PAD_0x1a = 0x1a, /**< Unused. */
  CTRL_C0_ESC      = 0x1b, /**< Escape: designation of character sets (ISO/IEC 2022). */
  CTRL_C0_APS      = 0x1c, /**< Active Position Set: puts the cursor at P1 (row) and P2 (column). */
  CTRL_C0_SS3      = 0x1d, /**< Single Shift 3: the next character comes from the G3 set (e.g. ♪). */
  CTRL_C0_RS       = 0x1e, /**< Record Separator. */
  CTRL_C0_US       = 0x1f  /**< Unit Separator. */
};

/**
 * @name C0 command callbacks
 *
 * All of them get the command data and return TRUE when the command was
 * executed. They return FALSE for commands not supported in closed caption or
 * with invalid parameters.
 * @{
 */
bool callback_controls_c0_APB (const CC_ControlCodes_CallbackData_t *d);  /**< APB: not supported in closed caption. @ctrlcb */
bool callback_controls_c0_APF (const CC_ControlCodes_CallbackData_t *d);  /**< APF: accepted, no effect on the state. @ctrlcb */
bool callback_controls_c0_APD (const CC_ControlCodes_CallbackData_t *d);  /**< APD: not supported in closed caption. @ctrlcb */
bool callback_controls_c0_APU (const CC_ControlCodes_CallbackData_t *d);  /**< APU: not supported in closed caption. @ctrlcb */
bool callback_controls_c0_CS (const CC_ControlCodes_CallbackData_t *d);   /**< CS: sets CaptionClosed_t::need_clear. @ctrlcb */
bool callback_controls_c0_APR (const CC_ControlCodes_CallbackData_t *d);  /**< APR: accepted; the controls hook gets the line break. @ctrlcb */
bool callback_controls_c0_LS1 (const CC_ControlCodes_CallbackData_t *d);  /**< LS1: not supported in closed caption. @ctrlcb */
bool callback_controls_c0_LS0 (const CC_ControlCodes_CallbackData_t *d);  /**< LS0: not supported in closed caption. @ctrlcb */
bool callback_controls_c0_PAPF (const CC_ControlCodes_CallbackData_t *d); /**< PAPF: not supported; skips 1 parameter. @ctrlcb */
bool callback_controls_c0_CAN (const CC_ControlCodes_CallbackData_t *d);  /**< CAN: not supported in closed caption. @ctrlcb */
bool callback_controls_c0_SS2 (const CC_ControlCodes_CallbackData_t *d);  /**< SS2: not supported; skips 1 parameter. @ctrlcb */
bool callback_controls_c0_ESC (const CC_ControlCodes_CallbackData_t *d);  /**< ESC: not supported; skips the whole sequence. @ctrlcb */
bool callback_controls_c0_APS (const CC_ControlCodes_CallbackData_t *d);  /**< APS: updates CaptionClosed_t::APS (2 parameters). @ctrlcb */
bool callback_controls_c0_SS3 (const CC_ControlCodes_CallbackData_t *d);  /**< SS3: hands the G3 character in UTF-8 to the text hook. @ctrlcb */
bool callback_controls_c0_RS (const CC_ControlCodes_CallbackData_t *d);   /**< RS: not supported in closed caption. @ctrlcb */
bool callback_controls_c0_US (const CC_ControlCodes_CallbackData_t *d);   /**< US: accepted, no effect on the state. @ctrlcb */
/** @} */

/**
 * C1 commands (ARIB TR-B14, Table 3.11; ARIB STD-B24, Table 7-15).
 *
 * Unused values are filled with @c PAD, so the code minus
 * 0x80 is the direct index into the callbacks table.
 */
enum CaptionClosed_CtrlCodes_C1 {
  CTRL_C1_BKF = 0x80, /**< Black Foreground: black text. */
  CTRL_C1_RDF = 0x81, /**< Red Foreground: red text. */
  CTRL_C1_GRF = 0x82, /**< Green Foreground: green text. */
  CTRL_C1_YLF = 0x83, /**< Yellow Foreground: yellow text. */
  CTRL_C1_BLF = 0x84, /**< Blue Foreground: blue text. */
  CTRL_C1_MGF = 0x85, /**< Magenta Foreground: magenta text. */
  CTRL_C1_CNF = 0x86, /**< Cyan Foreground: cyan text. */
  CTRL_C1_WHF = 0x87, /**< White Foreground: white text. */

  CTRL_C1_SSZ = 0x88, /**< Small Size: small characters. */
  CTRL_C1_MSZ = 0x89, /**< Middle Size: medium characters. */
  CTRL_C1_NSZ = 0x8a, /**< Normal Size: normal characters. */
  CTRL_C1_SZX = 0x8b, /**< Character Size Controls: size given by P1. */
  CTRL_C1_PAD_0x8c = 0x8c, /**< Unused. */
  CTRL_C1_PAD_0x8d = 0x8d, /**< Unused. */
  CTRL_C1_PAD_0x8e = 0x8e, /**< Unused. */
  CTRL_C1_PAD_0x8f = 0x8f, /**< Unused. */
  CTRL_C1_COL = 0x90, /**< Colour Controls: text/background color (P1, or 0x20 P2 for the palette). */

  CTRL_C1_FLC = 0x91, /**< Flashing Control: blinking. */
  CTRL_C1_CDC = 0x92, /**< Conceal Display Controls. */
  CTRL_C1_POL = 0x93, /**< Pattern Polarity Controls. */
  CTRL_C1_WMM = 0x94, /**< Writing Mode Modification. */
  CTRL_C1_MAC = 0x95, /**< Macro Command. */

  CTRL_C1_PAD_0x96 = 0x96, /**< Unused. */
  CTRL_C1_HLC = 0x97, /**< Highlighting Character Block: outline. */
  CTRL_C1_RPC = 0x98, /**< Repeat Character: repeats the next character. */
  CTRL_C1_SPL = 0x99, /**< Stop Lining: end of underline. */
  CTRL_C1_STL = 0x9a, /**< Start Lining: start of underline. */
  CTRL_C1_CSI = 0x9b, /**< Control Sequence Introducer: extended sequence, see @ref CaptionClosed_CtrlCodes_C1_CSI. */
  CTRL_C1_PAD_0x9c = 0x9c, /**< Unused. */
  CTRL_C1_TIM = 0x9d, /**< Time Controls: wait/timing. */
  CTRL_C1_PAD_0x9e = 0x9e, /**< Unused. */
  CTRL_C1_PAD_0x9f = 0x9f  /**< Unused. */
};

/**
 * @name C1 command callbacks
 * @{
 */
bool callback_controls_c1_BKF (const CC_ControlCodes_CallbackData_t *d); /**< BKF: black text. @ctrlcb */
bool callback_controls_c1_RDF (const CC_ControlCodes_CallbackData_t *d); /**< RDF: red text. @ctrlcb */
bool callback_controls_c1_GRF (const CC_ControlCodes_CallbackData_t *d); /**< GRF: green text. @ctrlcb */
bool callback_controls_c1_YLF (const CC_ControlCodes_CallbackData_t *d); /**< YLF: yellow text. @ctrlcb */
bool callback_controls_c1_BLF (const CC_ControlCodes_CallbackData_t *d); /**< BLF: blue text. @ctrlcb */
bool callback_controls_c1_MGF (const CC_ControlCodes_CallbackData_t *d); /**< MGF: magenta text. @ctrlcb */
bool callback_controls_c1_CNF (const CC_ControlCodes_CallbackData_t *d); /**< CNF: cyan text. @ctrlcb */
bool callback_controls_c1_WHF (const CC_ControlCodes_CallbackData_t *d); /**< WHF: white text. @ctrlcb */

bool callback_controls_c1_SSZ (const CC_ControlCodes_CallbackData_t *d); /**< SSZ: small characters. @ctrlcb */
bool callback_controls_c1_MSZ (const CC_ControlCodes_CallbackData_t *d); /**< MSZ: medium characters. @ctrlcb */
bool callback_controls_c1_NSZ (const CC_ControlCodes_CallbackData_t *d); /**< NSZ: normal characters. @ctrlcb */
bool callback_controls_c1_SZX (const CC_ControlCodes_CallbackData_t *d); /**< SZX: stores P1 in CaptionClosed_t::char_size_custom. @ctrlcb */
bool callback_controls_c1_COL (const CC_ControlCodes_CallbackData_t *d); /**< COL: text, background or half intensity color. @ctrlcb */

bool callback_controls_c1_FLC (const CC_ControlCodes_CallbackData_t *d); /**< FLC: not supported; skips 1 parameter. @ctrlcb */
bool callback_controls_c1_CDC (const CC_ControlCodes_CallbackData_t *d); /**< CDC: not supported; skips 1 or 2 parameters. @ctrlcb */
bool callback_controls_c1_POL (const CC_ControlCodes_CallbackData_t *d); /**< POL: not supported; skips 1 parameter. @ctrlcb */
bool callback_controls_c1_WMM (const CC_ControlCodes_CallbackData_t *d); /**< WMM: not supported; skips 1 parameter. @ctrlcb */
bool callback_controls_c1_MAC (const CC_ControlCodes_CallbackData_t *d); /**< MAC: not supported; skips 1 parameter. @ctrlcb */

bool callback_controls_c1_HLC (const CC_ControlCodes_CallbackData_t *d); /**< HLC: not supported; skips 1 parameter. @ctrlcb */
bool callback_controls_c1_RPC (const CC_ControlCodes_CallbackData_t *d); /**< RPC: not supported; skips 1 parameter. @ctrlcb */
bool callback_controls_c1_SPL (const CC_ControlCodes_CallbackData_t *d); /**< SPL: not supported in closed caption. @ctrlcb */
bool callback_controls_c1_STL (const CC_ControlCodes_CallbackData_t *d); /**< STL: not supported in closed caption. @ctrlcb */

bool callback_controls_c1_TIM (const CC_ControlCodes_CallbackData_t *d); /**< TIME: not supported; skips 2 parameters. @ctrlcb */
/** @} */

/**
 * CSI subcommands: the final byte @c F of the sequence
 * <tt>CSI P1 ; P2 ; ... 0x20 F</tt> (ARIB STD-B24, Table 7-17).
 *
 * Unused values are filled with @c PAD, so the code minus
 * 0x53 is the direct index into the callbacks table.
 */
enum CaptionClosed_CtrlCodes_C1_CSI {
  CTRL_C1_CSI_SWF      = 0x53, /**< Set Writing Format: display format. */
  CTRL_C1_CSI_PAD_0x54 = 0x54, /**< Unused. */
  CTRL_C1_CSI_PAD_0x55 = 0x55, /**< Unused. */
  CTRL_C1_CSI_SDF      = 0x56, /**< Set Display Format: size of the display area. */
  CTRL_C1_CSI_SSM      = 0x57, /**< Character Composition Dot Designation: character size. */
  CTRL_C1_CSI_SHS      = 0x58, /**< Set Horizontal Spacing: horizontal spacing. */
  CTRL_C1_CSI_SVS      = 0x59, /**< Set Vertical Spacing: vertical spacing. */
  CTRL_C1_CSI_PAD_0x5a = 0x5a, /**< Unused. */
  CTRL_C1_CSI_PAD_0x5b = 0x5b, /**< Unused. */
  CTRL_C1_CSI_PAD_0x5c = 0x5c, /**< Unused. */
  CTRL_C1_CSI_PAD_0x5d = 0x5d, /**< Unused. */
  CTRL_C1_CSI_PAD_0x5e = 0x5e, /**< Unused. */
  CTRL_C1_CSI_SDP      = 0x5f, /**< Set Display Position: position of the display area. */

  CTRL_C1_CSI_PAD_0x60 = 0x60, /**< Unused. */
  CTRL_C1_CSI_PAD_0x61 = 0x61, /**< Unused. */
  CTRL_C1_CSI_PAD_0x62 = 0x62, /**< Unused. */
  CTRL_C1_CSI_ORN      = 0x63, /**< Ornament Control: outline/shadow of the characters. */
  CTRL_C1_CSI_PAD_0x64 = 0x64, /**< Unused. */
  CTRL_C1_CSI_CFS      = 0x65, /**< Character Font Set: font. */
  CTRL_C1_CSI_PAD_0x66 = 0x66, /**< Unused. */
  CTRL_C1_CSI_PAD_0x67 = 0x67, /**< Unused. */
  CTRL_C1_CSI_PAD_0x68 = 0x68, /**< Unused. */
  CTRL_C1_CSI_PAD_0x69 = 0x69, /**< Unused. */
  CTRL_C1_CSI_PAD_0x6a = 0x6a, /**< Unused. */
  CTRL_C1_CSI_PAD_0x6b = 0x6b, /**< Unused. */
  CTRL_C1_CSI_PAD_0x6c = 0x6c, /**< Unused. */
  CTRL_C1_CSI_PAD_0x6d = 0x6d, /**< Unused. */
  CTRL_C1_CSI_RCS      = 0x6e  /**< Raster Colour Command: raster color. */
};

/**
 * @name CSI subcommand callbacks
 *
 * callback_controls_c1_CSI_load() handles the CSI (0x9b): finds the final byte,
 * reads the parameters with csi_arg_Pn_load() and calls the subcommand callback.
 * In the subcommands, CC_ControlCodes_CallbackData_t::data points to the
 * parameters (struct csi_arg_Pn).
 * @{
 */
bool callback_controls_c1_CSI_load (const CC_ControlCodes_CallbackData_t *d); /**< CSI: reads the sequence and dispatches the subcommand. @ctrlcb */
bool callback_controls_c1_CSI_swf (const CC_ControlCodes_CallbackData_t *d);  /**< SWF: format, characters per line and lines. @ctrlcb */
bool callback_controls_c1_CSI_ssm (const CC_ControlCodes_CallbackData_t *d);  /**< SSM: character size (P1 x P2). @ctrlcb */
bool callback_controls_c1_CSI_shs (const CC_ControlCodes_CallbackData_t *d);  /**< SHS: horizontal spacing (P1). @ctrlcb */
bool callback_controls_c1_CSI_svs (const CC_ControlCodes_CallbackData_t *d);  /**< SVS: vertical spacing (P1). @ctrlcb */
bool callback_controls_c1_CSI_orn (const CC_ControlCodes_CallbackData_t *d);  /**< ORN: not implemented. @ctrlcb */
bool callback_controls_c1_CSI_cfs (const CC_ControlCodes_CallbackData_t *d);  /**< CFS: not implemented. @ctrlcb */
bool callback_controls_c1_CSI_rcs (const CC_ControlCodes_CallbackData_t *d);  /**< RCS: raster color (P1, from 0 to 15). @ctrlcb */
bool callback_controls_c1_CSI_sdf (const CC_ControlCodes_CallbackData_t *d);  /**< SDF: size of the display area (P1 x P2). @ctrlcb */
bool callback_controls_c1_CSI_sdp (const CC_ControlCodes_CallbackData_t *d);  /**< SDP: position of the display area (P1, P2). @ctrlcb */
/** @} */

#define CTRL_CSI_FB            0x20  /**< Intermediate byte (0x20) that comes before the final byte of the CSI. */
#define CTRL_CSI_IB            0x3b  /**< CSI parameter separator (';'). */

#define CTRL_CSI_MAX_Pn_ARGS   5     /**< Max number of parameters read from a CSI. */
#define CTRL_CSI_MAX_Pn_FIELDS (5+1) /**< Max digits of a parameter, plus the '\\0' (up to "65535"). */

/**
 * Parameters of a CSI sequence, already converted to numbers.
 *
 * The P1 parameter of the spec is in @c argv[0], P2 is in @c argv[1], and so
 * on. Parameters not given stay zeroed.
 */
struct csi_arg_Pn {
  uint16_t argv[CTRL_CSI_MAX_Pn_ARGS]; /**< Parameter values (up to 65535). */
  uint8_t argc;                        /**< Number of parameters read. */
};

/**
 * Reads the decimal parameters of a CSI sequence, separated by ';'.
 *
 * @param d  @c d->argv points to the first parameter, @c d->argc is the
 *           number of parameter bytes and @c d->data points to the
 *           csi_arg_Pn struct that receives the values.
 * @return TRUE if at least one parameter was read.
 */
bool csi_arg_Pn_load (CC_ControlCodes_CallbackData_t *d);

/**
 * Start of each command table: the code minus this value is the index in the
 * callbacks table.
 */
enum CC_ControlCodes_TableJump_e {
  CC_CTRLCODE_SET_UNKNOWN = 0x00,        /**< Not a command. */
  CC_CTRLCODE_SET_C0      = CTRL_C0_APB, /**< C0 table (0x08 to 0x1f). */
  CC_CTRLCODE_SET_C1      = CTRL_C1_BKF  /**< C1 table (0x80 to 0x9f). */
};

/**
 * Checks if a byte is a C0 or C1 command.
 *
 * @param byte   Caption text byte.
 * @param table  Receives the command table, or @ref CC_CTRLCODE_SET_UNKNOWN.
 * @return TRUE if the byte is a command.
 */
bool caption_closed_controls_is_table (const uint8_t *byte,
                                       enum CC_ControlCodes_TableJump_e *table);

/**
 * Runs the C0/C1 command at @c argv[*pos].
 *
 * Calls the command callback, which moves @p pos past the parameters, and, if
 * it returns TRUE, calls the controls hook.
 *
 * @param table    Command table, got from caption_closed_controls_is_table().
 * @param argv     Caption text buffer.
 * @param argc     Number of bytes available from @p pos.
 * @param pos      Position of the command in @p argv; on return, it points to the
 *                 last byte consumed by the command.
 * @param caption  Caption state.
 * @return TRUE if the command was executed.
 */
bool caption_closed_controls_load (enum CC_ControlCodes_TableJump_e table,
                                   const uint8_t *argv,
                                   const uint16_t argc,
                                   uint16_t *pos,
                                   CaptionClosed_t *caption);

/**
 * Colors of the CMLA palette (Colour Map Lower Address), used by the
 * color commands (BKF ~ WHF and COL).
 */
typedef enum {
  SET_CMLA0_BLACK,                  /**< Black. */
  SET_CMLA1_FULL_INTENSITY_RED,     /**< Red. */
  SET_CMLA2_FULL_INTENSITY_GREEN,   /**< Green. */
  SET_CMLA3_FULL_INTENSITY_YELLOW,  /**< Yellow. */
  SET_CMLA4_FULL_INTENSITY_BLUE,    /**< Blue. */
  SET_CMLA5_FULL_INTENSITY_MAGNETA, /**< Magenta. */
  SET_CMLA6_FULL_INTENSITY_CYAN,    /**< Cyan. */
  SET_CMLA7_FULL_INTENSITY_WHITE,   /**< White. */
  SET_CMLA8_TRANSPARENT,            /**< Transparent. */
  SET_CMLA9_HALF_INTENSITY_RED,     /**< Red, half intensity. */
  SET_CMLA10_HALF_INTENSITY_GREEN,  /**< Green, half intensity. */
  SET_CMLA11_HALF_INTENSITY_YELLOW, /**< Yellow, half intensity. */
  SET_CMLA12_HALF_INTENSITY_BLUE,   /**< Blue, half intensity. */
  SET_CMLA13_HALF_INTENSITY_MAGNETA,/**< Magenta, half intensity. */
  SET_CMLA14_HALF_INTENSITY_CYAN,   /**< Cyan, half intensity. */
  SET_CMLA15_HALF_INTENSITY_WHITE,  /**< White, half intensity. */
  SET_CMLA_t_eMax                   /**< Number of colors. */
} CC_Color_CMLA_t;

/**
 * Converts a CMLA palette color to RGBA.
 *
 * @param index  Palette color, from 0 to 15.
 * @param color  Receives the color in RGBA.
 * @return FALSE if @p index is outside the palette.
 */
static inline bool caption_closed_controls_cmlat2rgb (CC_Color_CMLA_t index, CC_Color_t *color)
{
  static const CC_Color_t palete_index[] = {
  /* <R>   <G>   <B>    <A> */
   { 0x00, 0x00, 0x00, 0xFF }, // SET_CMLA0_BLACK
   { 0xFE, 0x00, 0x00, 0xFF }, // SET_CMLA1_FULL_INTENSITY_RED
   { 0x00, 0xFE, 0x00, 0xFF }, // SET_CMLA2_FULL_INTENSITY_GREEN
   { 0xFE, 0xFE, 0x00, 0xFF }, // SET_CMLA3_FULL_INTENSITY_YELLOW
   { 0x00, 0x00, 0xFE, 0xFF }, // SET_CMLA4_FULL_INTENSITY_BLUE
   { 0xFE, 0x00, 0xFE, 0xFF }, // SET_CMLA5_FULL_INTENSITY_MAGNETA
   { 0x00, 0xFE, 0xFE, 0xFF }, // SET_CMLA6_FULL_INTENSITY_CYAN
   { 0xFE, 0xFE, 0xFE, 0xFF }, // SET_CMLA7_FULL_INTENSITY_WHITE
   { 0x00, 0x00, 0x00, 0x00 }, // SET_CMLA8_TRANSPARENT
   { 0x79, 0x00, 0x00, 0xFF }, // SET_CMLA9_HALF_INTENSITY_RED
   { 0x00, 0x7D, 0x00, 0xFF }, // SET_CMLA10_HALF_INTENSITY_GREEN
   { 0x79, 0x79, 0x00, 0xFF }, // SET_CMLA11_HALF_INTENSITY_YELLOW
   { 0x00, 0x00, 0x79, 0xFF }, // SET_CMLA12_HALF_INTENSITY_BLUE
   { 0x79, 0x00, 0x79, 0xFF }, // SET_CMLA13_HALF_INTENSITY_MAGNETA
   { 0x00, 0x79, 0x79, 0xFF }, // SET_CMLA14_HALF_INTENSITY_CYAN
   { 0x79, 0x79, 0x79, 0xFF }  // SET_CMLA15_HALF_INTENSITY_WHITE
  };

  assert (color != NULL);

  if ((unsigned)index >= ARRAY_SIZE(palete_index))
  {
    DTV_DEBUG("Índice de cor (%d) fora da paleta.", index);
    return FALSE;
  }

  *color = palete_index[index];

  return TRUE;
}

#endif /* CAPTION_CLOSED_CONTROLS_H_ */
