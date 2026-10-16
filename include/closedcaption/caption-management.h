/**
 * @file caption-management.h
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @date Thu Aug 12 13:49:21 BRT 2010
 * @brief Reading the "caption management data" and the "caption statement data"
 *        (ARIB STD-B24, Volume 3, 9.3).
 *
 * The @c data_group content (see caption-data.h) depends on the
 * @c data_group_id:
 *
 * - <b>caption management data</b> (@c data_group_id 0x00 or 0x20): time
 *   mode (TMD), available languages, display format, character
 *   encoding (TCS) and roll-up mode;
 * - <b>caption statement data</b> (other values): time mode and the
 *   <i>data units</i> with the caption text (@c statement body), DRCS,
 *   bitmaps, etc.
 *
 * caption_management_load() reads both formats into the same struct,
 * CaptionManagement_t, and caption_management_get_type() tells what was read.
 */
#ifndef CAPTION_MANAGEMENT_H_
#define CAPTION_MANAGEMENT_H_

#include "packet-pes.h"
#include "caption-data.h"
#include "caption-common.h"

/**
 * @name TMD: time mode (ARIB STD-B24, Table 9-4, binary values 00 to 11)
 * @{
 */
#define MANAGEMENT_TMD_FREE                0x00 /**< Free display, no time sync. */
#define MANAGEMENT_TMD_REALTIME            0x01 /**< Real time: the statement data carries the STM. */
#define MANAGEMENT_TMD_OFFSET              0x02 /**< Offset time: management carries the OTM, statement carries the STM. */
#define MANAGEMENT_TMD_RESERVED            0x03 /**< Reserved. */
/** @} */

/**
 * @name TCS: character encoding (ARIB STD-B24, Table 9-8)
 * @{
 */
#define MANAGEMENT_TCS_8BIT                0x00 /**< 8-bit code (ISO-8859-15 in Brazil). */
#define MANAGEMENT_TCS_UCS                 0x01 /**< UCS (Unicode). Not supported. */
/** @} */

/**
 * @name Roll-up mode (ARIB STD-B24, Table 9-9)
 * @{
 */
#define MANAGEMENT_ROLL_NONE               0x00 /**< No roll-up. */
#define MANAGEMENT_ROLL_UP                 0x01 /**< With roll-up. */
/** @} */

/**
 * @name data_unit_parameter: type of each data unit (ARIB STD-B24, Table 9-12)
 * @{
 */
#define _CAPTION_HANDLER_STATEMENT_BODY    0x20 /**< Caption text. */
#define _CAPTION_HANDLER_GEOMETRIC         0x28 /**< Geometric graphics. */
#define _CAPTION_HANDLER_SYNTHESIZED_SOUND 0x2c /**< Synthesized sound. */
#define _CAPTION_HANDLER_1BYTE_DRCS        0x30 /**< 1-byte DRCS (characters drawn by the broadcaster). */
#define _CAPTION_HANDLER_2BYTE_DRCS        0x31 /**< 2-byte DRCS. */
#define _CAPTION_HANDLER_COLOR_MAP         0x34 /**< Color map. */
#define _CAPTION_HANDLER_BITMAP            0x35 /**< Bitmap. */
/** @} */

/**
 * Type of content read by caption_management_load().
 */
typedef enum {
  CAPTION_HANDLER_TYPE_UNKNOWN           = 0, /**< Data unit of unknown type. */
  CAPTION_HANDLER_TYPE_STATEMENT_BODY    = 1, /**< Caption text: process it with caption_closed_run(). */
  CAPTION_HANDLER_TYPE_GEOMETRIC         = 2, /**< Geometric graphics. */
  CAPTION_HANDLER_TYPE_SYNTHESIZED_SOUND = 3, /**< Synthesized sound. */
  CAPTION_HANDLER_TYPE_1BYTE_DRCS        = 4, /**< 1-byte DRCS. */
  CAPTION_HANDLER_TYPE_2BYTE_DRCS        = 5, /**< 2-byte DRCS. */
  CAPTION_HANDLER_TYPE_COLOR_MAP         = 6, /**< Color map. */
  CAPTION_HANDLER_TYPE_BITMAP            = 7, /**< Bitmap. */

  CAPTION_HANDLER_TYPE_MANAGEMENT        = 8, /**< Caption management data, or statement without data units. */

  CAPTION_HANDLER_TYPE_MAX               = 9  /**< Number of types. */
} CaptionManagementType_t;

/**
 * Content of the "caption management data" or the "caption statement data"
 * (ARIB STD-B24, Volume 3, 9.3.1 and 9.3.2).
 */
typedef struct CaptionManagement_t {
  uint8_t TMD;      /**< Time mode, see @ref MANAGEMENT_TMD_FREE and the next ones. */
  uint64_t TMD_otm; /**< OTM (management) or STM (statement): 36 bits, 9 BCD digits (hhmmssmmm). */
  uint8_t reserved; /**< Reserved bits of the first byte. */

  /** @name Only in the caption management data (of the first language)
   *  @{ */
  uint8_t num_lang;            /**< Number of languages. */
  uint8_t lang_tag;            /**< Language tag (0 to 7). */
  uint8_t DMF;                 /**< Display Mode (4 bits). */
  uint8_t DC;                  /**< Display condition (present when DMF is 0x0c, 0x0d or 0x0e). */
  char ISO_639_lang_code[3+1]; /**< Language code, ISO 639-2 (e.g. "por"), '\\0' terminated. */
  uint8_t format;              /**< Display format, see @ref CC_Format_t. */
  uint8_t TCS;                 /**< Character encoding, see @ref MANAGEMENT_TCS_8BIT. */
  uint8_t rollup_mode;         /**< Roll-up mode, see @ref MANAGEMENT_ROLL_NONE. */
  /** @} */

  /**
   * Data unit picked from the caption statement data: the first one with the
   * caption text (@ref _CAPTION_HANDLER_STATEMENT_BODY), or the first of any
   * type if there is no text. Zeroed in the caption management data.
   */
  struct statement {
    uint8_t unit_sep;         /**< @c unit_separator (always 0x1f), or 0 when there is no data unit. */
    uint8_t unit_param;       /**< @c data_unit_parameter: type of the data unit. */
    uint32_t unit_size;       /**< @c data_unit_size: size of @ref unit_data (24 bits). */
    const uint8_t *unit_data; /**< Data unit data, inside the PES buffer; NULL when empty. */
  } statement; /**< Picked data unit; see the description of the statement struct. */

  const PES_pts_t *pts; /**< Points to PacketPES_t::pts; valid while the PacketPES_t exists. */
} CaptionManagement_t;

/**
 * Reads the caption management data or the caption statement data of a
 * @c data_group.
 *
 * The format is chosen by the @c data_group_id: 0x00 and 0x20 are read as
 * management, the others as statement. All the sizes are checked
 * against the @c data_group_size.
 *
 * @param caption     @c data_group loaded by caption_data_load().
 * @param management  Receives the content. The pointers point inside the
 *                    PES buffer.
 * @return TRUE if the content was read; FALSE if it is truncated or empty.
 */
bool caption_management_load (const CaptionData_t *caption,
                              CaptionManagement_t *management);

/**
 * Tells the type of content read by caption_management_load().
 *
 * @param management  Content read by caption_management_load().
 * @return @ref CAPTION_HANDLER_TYPE_STATEMENT_BODY when there is caption text
 *         for caption_closed_run(); @ref CAPTION_HANDLER_TYPE_MANAGEMENT
 *         for the caption management data; or the data unit type.
 */
CaptionManagementType_t caption_management_get_type (const CaptionManagement_t *management);

#endif /* CAPTION_MANAGEMENT_H_ */
