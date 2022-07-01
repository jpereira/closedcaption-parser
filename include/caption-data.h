/**
 * @file: caption-data.h
 * @autor: Jorge Pereira <jorge.pereira@tqtvd.com>
 * @date: Thu Aug 12 13:49:21 BRT 2010
 * @desc: Mais informações em 'arquitetura-closedcaption.txt'
 */
#ifndef CAPTION_DATA_H_
#define CAPTION_DATA_H_

#include "caption-common.h"
#include "packet-pes.h"

#define CAPTION_MANAGEMENT_DGI_A 0x00
#define CAPTION_MANAGEMENT_DGI_B 0x20

/**
 * Tudo conforme descrito na norma ARIB STD-B24 , Pag 152, 9.2
 */
typedef struct CaptionData_t {
  uint8_t group_id;
  uint8_t version;
  uint8_t link_number;
  uint8_t last_link_number;
  uint16_t datalen;
  const uint8_t *data;
  uint16_t crc16;

  const PES_pts_t *pts; /* Apenas aponta para o PTS do PacketPES_t::pts */
} CaptionData_t;

bool
caption_data_check_crc16 (const uint8_t *data,
                          const uint16_t *datalen,
                          const uint16_t *crc16);

bool
caption_data_load (const PacketPES_t *pes,
                   CaptionData_t *caption);

#endif /* CAPTION_DATA_H_*/

