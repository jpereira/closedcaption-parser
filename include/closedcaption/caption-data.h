/**
 * @file caption-data.h
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @date Thu Aug 12 13:49:21 BRT 2010
 * @brief Leitura do @c data_group da legenda (ARIB STD-B24, Volume 3, 9.2).
 *
 * Cada PES de legenda carrega um @c data_group:
 *
 * @verbatim
 * data_group_id(6) data_group_version(2) data_group_link_number(8)
 * last_data_group_link_number(8) data_group_size(16)
 * data_group_data_byte[data_group_size] CRC_16(16)
 * @endverbatim
 *
 * O @c data_group_id indica o conteúdo: 0x00/0x20 é o "caption management
 * data" (grupos A e B) e os demais são o "caption statement data" de cada
 * idioma (veja caption-management.h).
 */
#ifndef CAPTION_DATA_H_
#define CAPTION_DATA_H_

#include "caption-common.h"
#include "packet-pes.h"

#define CAPTION_MANAGEMENT_DGI_A 0x00 /**< @c data_group_id do "caption management data", grupo A. */
#define CAPTION_MANAGEMENT_DGI_B 0x20 /**< @c data_group_id do "caption management data", grupo B. */

/**
 * @c data_group carregado por caption_data_load().
 */
typedef struct CaptionData_t {
  uint8_t group_id;         /**< @c data_group_id (6 bits). */
  uint8_t version;          /**< @c data_group_version (2 bits). */
  uint8_t link_number;      /**< @c data_group_link_number. */
  uint8_t last_link_number; /**< @c last_data_group_link_number. */
  uint16_t datalen;         /**< @c data_group_size: tamanho de @ref data. */
  const uint8_t *data;      /**< @c data_group_data_byte, dentro do buffer do PES. */
  uint16_t crc16;           /**< @c CRC_16 lido do pacote. */

  const PES_pts_t *pts;     /**< Aponta para o PacketPES_t::pts; válido enquanto o PacketPES_t existir. */
} CaptionData_t;

/**
 * Verifica o CRC-16 de um @c data_group.
 *
 * O CRC é o CRC-16/CCITT (polinômio x^16 + x^12 + x^5 + 1, valor inicial 0),
 * calculado sobre todo o @c data_group, do @c data_group_id até o último byte
 * de dados (ARIB STD-B24, Volume 3, 9.2).
 *
 * @param data     Início do @c data_group (o byte do @c data_group_id).
 * @param datalen  Quantidade de bytes de @p data cobertos pelo CRC
 *                 (5 de cabeçalho + @c data_group_size).
 * @param crc16    CRC lido do pacote.
 * @return TRUE se o CRC confere.
 */
bool caption_data_check_crc16 (const uint8_t *data,
                               const uint16_t *datalen,
                               const uint16_t *crc16);

/**
 * Carrega o @c data_group de um PES e verifica o CRC.
 *
 * Bytes de stuffing depois do @c data_group são ignorados.
 *
 * @param pes      PES carregado por packet_pes_load().
 * @param caption  Recebe o @c data_group. Os ponteiros apontam para dentro do
 *                 buffer do PES.
 * @return TRUE se o @c data_group é válido e o CRC confere.
 */
bool caption_data_load (const PacketPES_t *pes,
                        CaptionData_t *caption);

#endif /* CAPTION_DATA_H_ */
