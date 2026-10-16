/**
 * @file caption-data.h
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @date Thu Aug 12 13:49:21 BRT 2010
 * @brief Reading the caption @c data_group (ARIB STD-B24, Volume 3, 9.2).
 *
 * Each caption PES carries a @c data_group:
 *
 * @verbatim
 * data_group_id(6) data_group_version(2) data_group_link_number(8)
 * last_data_group_link_number(8) data_group_size(16)
 * data_group_data_byte[data_group_size] CRC_16(16)
 * @endverbatim
 *
 * The @c data_group_id tells the content: 0x00/0x20 is the "caption management
 * data" (groups A and B) and the others are the "caption statement data" of each
 * language (see caption-management.h).
 */
#ifndef CAPTION_DATA_H_
#define CAPTION_DATA_H_

#include "caption-common.h"
#include "packet-pes.h"

#define CAPTION_MANAGEMENT_DGI_A 0x00 /**< @c data_group_id of the "caption management data", group A. */
#define CAPTION_MANAGEMENT_DGI_B 0x20 /**< @c data_group_id of the "caption management data", group B. */

/**
 * @c data_group loaded by caption_data_load().
 */
typedef struct CaptionData_t {
  uint8_t group_id;         /**< @c data_group_id (6 bits). */
  uint8_t version;          /**< @c data_group_version (2 bits). */
  uint8_t link_number;      /**< @c data_group_link_number. */
  uint8_t last_link_number; /**< @c last_data_group_link_number. */
  uint16_t datalen;         /**< @c data_group_size: size of @ref data. */
  const uint8_t *data;      /**< @c data_group_data_byte, inside the PES buffer. */
  uint16_t crc16;           /**< @c CRC_16 read from the packet. */

  const PES_pts_t *pts;     /**< Points to PacketPES_t::pts; valid while the PacketPES_t exists. */
} CaptionData_t;

/**
 * Checks the CRC-16 of a @c data_group.
 *
 * The CRC is the CRC-16/CCITT (polynomial x^16 + x^12 + x^5 + 1, initial value 0),
 * calculated over the whole @c data_group, from the @c data_group_id to the last
 * data byte (ARIB STD-B24, Volume 3, 9.2).
 *
 * @param data     Start of the @c data_group (the @c data_group_id byte).
 * @param datalen  Number of bytes of @p data covered by the CRC
 *                 (5 of header + @c data_group_size).
 * @param crc16    CRC read from the packet.
 * @return TRUE if the CRC matches.
 */
bool caption_data_check_crc16 (const uint8_t *data,
                               const uint16_t *datalen,
                               const uint16_t *crc16);

/**
 * Loads the @c data_group of a PES and checks the CRC.
 *
 * Stuffing bytes after the @c data_group are ignored.
 *
 * @param pes      PES loaded by packet_pes_load().
 * @param caption  Receives the @c data_group. The pointers point inside the
 *                 PES buffer.
 * @return TRUE if the @c data_group is valid and the CRC matches.
 */
bool caption_data_load (const PacketPES_t *pes,
                        CaptionData_t *caption);

#endif /* CAPTION_DATA_H_ */
