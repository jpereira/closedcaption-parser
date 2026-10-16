/**
 * @file packet-pes.h
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @date Thu Aug 12 13:49:21 BRT 2010
 * @brief Reading the header of a caption PES packet.
 *
 * ISDB-T captions are carried in PES packets with
 * @c stream_id 0xBD (private_stream_1), in the "PES data packet" format of
 * ARIB STD-B24, Volume 3, Chapter 5:
 *
 * @verbatim
 * [PES header][data_identifier][private_stream_id][PES_data_packet_header_length][...][data_group]
 * @endverbatim
 *
 * packet_pes_load() reads the header, the PTS and finds the @c data_group, which is
 * then processed by caption_data_load().
 */
#ifndef PACKET_PES_H_
#define PACKET_PES_H_

#include "caption-common.h"

#define PACKET_PES_PREFIX         0x000001 /**< (24 bits) @c packet_start_code_prefix. */
#define PACKET_PES_PRIVATE_STREAM 0xbd     /**< @c stream_id of private_stream_1, used by captions. */

/**
 * @name PTS_DTS_flags field values
 * @{
 */
#define PTS_is_NONE               0x00 /**< No PTS and no DTS. */
#define PTS_is_FORBIDEN           0x01 /**< Value forbidden by the spec. */
#define PTS_is_ONLY_PTS           0x02 /**< PTS only (Presentation Time Stamp). */
#define PTS_is_BOTH               0x03 /**< PTS and DTS (Presentation/Decode Time Stamp). */
/** @} */

/**
 * PTS (Presentation Time Stamp) already converted to hours, minutes and seconds.
 */
typedef struct {
  uint8_t h;   /**< Hours. */
  uint8_t m;   /**< Minutes. */
  uint8_t s;   /**< Seconds. */
  uint16_t u;  /**< Fraction of a second, in tenths of a millisecond (0 to 9999). */
  uint32_t ts; /**< Original PTS in 90 kHz units (the 32 least significant bits of the 33). */
} PES_pts_t;

/** PES packet. See @ref PacketPES_s. */
typedef struct PacketPES_s PacketPES_t;

/**
 * PES packet loaded by packet_pes_load().
 */
struct PacketPES_s {
  uint8_t stream_id;  /**< @c stream_id (0xBD for captions). */
  uint16_t pack_len;  /**< @c PES_packet_length: size of the packet after this field. */

  /** Optional PES header. */
  struct header {
    uint8_t len; /**< @c PES_header_data_length: header bytes after byte 8. */

    /** Flags of bytes 6 and 7 of the packet. */
    union flags {
      uint16_t raw; /**< Bytes 6 and 7, not decoded. */

      /**
       * Decoded flags.
       *
       * @warning The order of the bit fields depends on the compiler; this
       *          struct works on GCC/Clang on little-endian machines.
       */
      struct {
        // byte[7]
        uint8_t PES_extension_flag : 1;  /**< Has header extension. */
        uint8_t PES_CRC_flag : 1;        /**< Has PES CRC. */
        uint8_t copy_info_flag : 1;      /**< Has @c additional_copy_info. */
        uint8_t DSM_trick_mode_flag : 1; /**< Has @c DSM_trick_mode. */
        uint8_t ES_rate_flag : 1;        /**< Has @c ES_rate. */
        uint8_t ESCR_flag : 1;           /**< Has ESCR. */
        uint8_t PTS_DTS_flag : 2;        /**< PTS/DTS present, see @ref PTS_is_NONE and the next ones. */

        // byte [6]
        uint8_t rights : 1;     /**< @c original_or_copy. */
        uint8_t copyright : 1;  /**< @c copyright. */
        uint8_t alignment : 1;  /**< @c data_alignment_indicator. */
        uint8_t priority : 1;   /**< @c PES_priority. */
        uint8_t scrambling : 2; /**< @c PES_scrambling_control (0 = not scrambled). */
        uint8_t emulation : 2;  /**< Fixed marker '10' of the optional header. */
      } bit; /**< Decoded flags. */
    } flags; /**< Flags of bytes 6 and 7. */

  } header; /**< Optional header. */

  PES_pts_t pts; /**< PTS of the packet; zeroed when the packet has no PTS. */

  /** PES data (the caption @c data_group). */
  struct payload {
    uint32_t header;     /**< @c data_identifier, @c private_stream_id e @c PES_data_packet_header_length (3 bytes). */
    const uint8_t *data; /**< Start of the @c data_group, inside the buffer given to packet_pes_load(). */
    uint16_t len;        /**< Bytes available from @ref data, including any stuffing at the end. */
  } payload; /**< PES data. */

  PacketPES_t *next; /**< Reserved to chain more than one PES; not used. */
};

/**
 * Loads the header of a PES packet and finds the @c data_group.
 *
 * @param pack_raw  Whole PES packet, starting at the @c packet_start_code_prefix (00 00 01).
 * @param raw_len   Size of @p pack_raw in bytes.
 * @param out       Receives the loaded packet. The @c payload pointers
 *                  point inside @p pack_raw.
 * @return TRUE if the packet is a valid PES with data.
 */
bool
packet_pes_load (const uint8_t *pack_raw,
                 const size_t raw_len,
                 PacketPES_t *out);

/**
 * Converts a PTS in 90 kHz units to hours, minutes and seconds.
 *
 * @param ts   PTS in 90 kHz units.
 * @param pts  Receives the converted PTS; @c pts->ts gets @p ts itself.
 */
void
packet_pes_set_pts (uint32_t *ts,
                    PES_pts_t *pts);

#endif /* PACKET_PES_H_ */
