/**
 * @file packet-pes.h
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @date Thu Aug 12 13:49:21 BRT 2010
 * @brief Leitura do cabeçalho de um pacote PES de legenda.
 *
 * As legendas do ISDB-T são transportadas em pacotes PES com
 * @c stream_id 0xBD (private_stream_1), no formato "PES data packet" da
 * ARIB STD-B24, Volume 3, Capítulo 5:
 *
 * @verbatim
 * [cabeçalho PES][data_identifier][private_stream_id][PES_data_packet_header_length][...][data_group]
 * @endverbatim
 *
 * packet_pes_load() lê o cabeçalho, o PTS e localiza o @c data_group, que é
 * processado em seguida por caption_data_load().
 */
#ifndef PACKET_PES_H_
#define PACKET_PES_H_

#include "caption-common.h"

#define PACKET_PES_PREFIX         0x000001 /**< (24 bits) @c packet_start_code_prefix. */
#define PACKET_PES_PRIVATE_STREAM 0xbd     /**< @c stream_id do private_stream_1, usado pelas legendas. */

/**
 * @name Valores do campo PTS_DTS_flags
 * @{
 */
#define PTS_is_NONE               0x00 /**< Sem PTS nem DTS. */
#define PTS_is_FORBIDEN           0x01 /**< Valor proibido pela norma. */
#define PTS_is_ONLY_PTS           0x02 /**< Apenas PTS (Presentation Time Stamp). */
#define PTS_is_BOTH               0x03 /**< PTS e DTS (Presentation/Decode Time Stamp). */
/** @} */

/**
 * PTS (Presentation Time Stamp) já convertido em horas, minutos e segundos.
 */
typedef struct {
  uint8_t h;   /**< Horas. */
  uint8_t m;   /**< Minutos. */
  uint8_t s;   /**< Segundos. */
  uint16_t u;  /**< Fração de segundo, em décimos de milissegundo (0 a 9999). */
  uint32_t ts; /**< PTS original em unidades de 90 kHz (os 32 bits menos significativos dos 33). */
} PES_pts_t;

/** Pacote PES. Veja @ref PacketPES_s. */
typedef struct PacketPES_s PacketPES_t;

/**
 * Pacote PES carregado por packet_pes_load().
 */
struct PacketPES_s {
  uint8_t stream_id;  /**< @c stream_id (0xBD para legendas). */
  uint16_t pack_len;  /**< @c PES_packet_length: tamanho do pacote após este campo. */

  /** Cabeçalho opcional do PES. */
  struct header {
    uint8_t len; /**< @c PES_header_data_length: bytes de cabeçalho após o byte 8. */

    /** Flags dos bytes 6 e 7 do pacote. */
    union flags {
      uint16_t raw; /**< Bytes 6 e 7, sem decodificar. */

      /**
       * Flags decodificadas.
       *
       * @warning A ordem dos campos de bits depende do compilador; esta
       *          estrutura funciona em GCC/Clang em máquinas little-endian.
       */
      struct {
        // byte[7]
        uint8_t PES_extension_flag : 1;  /**< Tem extensão do cabeçalho. */
        uint8_t PES_CRC_flag : 1;        /**< Tem CRC do PES. */
        uint8_t copy_info_flag : 1;      /**< Tem @c additional_copy_info. */
        uint8_t DSM_trick_mode_flag : 1; /**< Tem @c DSM_trick_mode. */
        uint8_t ES_rate_flag : 1;        /**< Tem @c ES_rate. */
        uint8_t ESCR_flag : 1;           /**< Tem ESCR. */
        uint8_t PTS_DTS_flag : 2;        /**< PTS/DTS presentes, veja @ref PTS_is_NONE e seguintes. */

        // byte [6]
        uint8_t rights : 1;     /**< @c original_or_copy. */
        uint8_t copyright : 1;  /**< @c copyright. */
        uint8_t alignment : 1;  /**< @c data_alignment_indicator. */
        uint8_t priority : 1;   /**< @c PES_priority. */
        uint8_t scrambling : 2; /**< @c PES_scrambling_control (0 = sem embaralhamento). */
        uint8_t emulation : 2;  /**< Marcador fixo '10' do cabeçalho opcional. */
      } bit; /**< Flags decodificadas. */
    } flags; /**< Flags dos bytes 6 e 7. */

  } header; /**< Cabeçalho opcional. */

  PES_pts_t pts; /**< PTS do pacote; zerado quando o pacote não tem PTS. */

  /** Dados do PES (o @c data_group da legenda). */
  struct payload {
    uint32_t header;     /**< @c data_identifier, @c private_stream_id e @c PES_data_packet_header_length (3 bytes). */
    const uint8_t *data; /**< Início do @c data_group, dentro do buffer passado a packet_pes_load(). */
    uint16_t len;        /**< Bytes disponíveis a partir de @ref data, incluindo um eventual stuffing no final. */
  } payload; /**< Dados do PES. */

  PacketPES_t *next; /**< Reservado para encadear mais de um PES; não utilizado. */
};

/**
 * Carrega o cabeçalho de um pacote PES e localiza o @c data_group.
 *
 * @param pack_raw  Pacote PES completo, a partir do @c packet_start_code_prefix (00 00 01).
 * @param raw_len   Tamanho de @p pack_raw em bytes.
 * @param out       Recebe o pacote carregado. Os ponteiros de @c payload
 *                  apontam para dentro de @p pack_raw.
 * @return TRUE se o pacote é um PES válido e com dados.
 */
bool
packet_pes_load (const uint8_t *pack_raw,
                 const size_t raw_len,
                 PacketPES_t *out);

/**
 * Converte um PTS em unidades de 90 kHz para horas, minutos e segundos.
 *
 * @param ts   PTS em unidades de 90 kHz.
 * @param pts  Recebe o PTS convertido; @c pts->ts recebe o próprio @p ts.
 */
void
packet_pes_set_pts (uint32_t *ts,
                    PES_pts_t *pts);

#endif /* PACKET_PES_H_ */
