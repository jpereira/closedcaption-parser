/**
 * @file: packet-pes.h
 * @autor: Jorge Pereira <jorge.pereira@tqtvd.com>
 * @date: Thu Aug 12 13:49:21 BRT 2010
 * @desc: Mais informações em 'arquitetura-closedcaption.txt'
 */
#ifndef PACKET_PES_H_
#define PACKET_PES_H_

#include "caption-common.h"

#define PACKET_PES_PREFIX         0x000001 /* (24 bits) Packet start code prefix. */
#define PACKET_PES_PRIVATE_STREAM 0xbd     /* (8 bits) PES opicional conforme 3.13 do livro Data BroadCast */

#define PTS_is_NONE               0x00 /* Nenhum */
#define PTS_is_FORBIDEN           0x01 /* Proibido */
#define PTS_is_ONLY_PTS           0x02 /* apenas pacotes PTS (Presentation Time Stamp) */
#define PTS_is_BOTH               0x03 /* Tem informacoes PTS/DTS (Presentation/Decode Time Stamp) */

typedef struct {
  uint8_t h; /* hour */
  uint8_t m; /* minute */
  uint8_t s; /* seconds */
  uint16_t u; /* micro-seconds */
  uint32_t ts; /* time-stamp */
} PES_pts_t; /* Presentation Time Stamp */

typedef struct PacketPES_s PacketPES_t;

struct PacketPES_s {
  uint8_t stream_id;
  uint16_t pack_len;

  struct header {
    uint8_t len; /* byte[8] numero de bytes no header */
    union flags {
      uint16_t raw; /* byte [6][7] */
      struct {
        // byte[7]
        uint8_t PES_extension_flag : 1;
        uint8_t PES_CRC_flag : 1;
        uint8_t copy_info_flag : 1;
        uint8_t DSM_trick_mode_flag : 1;
        uint8_t ES_rate_flag : 1;
        uint8_t ESCR_flag : 1;
        uint8_t PTS_DTS_flag : 2;

        // byte [6]
        uint8_t rights : 1;
        uint8_t copyright : 1;
        uint8_t alignment : 1;
        uint8_t priority : 1;
        uint8_t scrambling : 2;
        uint8_t emulation : 2;
      } bit;
    } flags; /* (byte 6/7)  flags  */

  } header; /* flags relacionadas ao header */

  PES_pts_t pts;
  struct payload {
    uint32_t header;
    const uint8_t *data;
    uint16_t len;
  } payload; /* informacoes referente ao payload */

  PacketPES_t *next; /* Caso chegue mais de um pacote PES. */
};

/**
 * Caso o pacote 'pack_raw' seja um 'PES' valido, ira retornar TRUE
 * e sera salvo em 'out' um objeto do tipo 'PacketPES_t'
 */
bool
packet_pes_load (const uint8_t *pack_raw,
                 const size_t raw_len,
                 PacketPES_t *out);

/**
 * Utilizado para transformar um 'pts' em formato mais 'amigavel' conforme PES_pts_t
 */
void
packet_pes_set_pts (uint32_t *ts,
                    PES_pts_t *pts);
                    
#endif /* PACKET_PES_H_ */

