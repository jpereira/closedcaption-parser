/**
 * @file: caption-data.c
 * @autor: Jorge Pereira <jpereiran@gmail.com>
 * @date: Thu Aug 12 13:49:21 BRT 2010
 * @desc: Mais informações em 'arquitetura-closedcaption.txt'
 */
#include "caption-data.h"
#include "caption-closed.h"
#include "caption-superimpose.h"

//#define DEBUG_CAPTION_DATA

/**
 * CRC-16 (ITU-T, polinomio x^16 + x^12 + x^5 + 1, valor inicial 0) calculado
 * sobre todo o data_group, conforme ARIB STD-B24, Pag 153. Como o CRC esta no
 * final do grupo, o calculo sobre 'data' + 'crc16' deve resultar em zero.
 */
bool
caption_data_check_crc16 (const uint8_t *data,
                          const uint16_t *datalen,
                          const uint16_t *crc16)
{
  const uint8_t tail[2] = { (uint8_t)(*crc16 >> 8), (uint8_t)(*crc16 & 0xff) };
  uint16_t crc = 0;
  uint32_t i;
  uint8_t bit;

  assert (data != NULL);
  assert (datalen != NULL);
  assert (crc16 != NULL);

  for (i = 0; i < (uint32_t)*datalen + 2; i++)
  {
    uint8_t byte = (i < *datalen) ? data[i] : tail[i - *datalen];

    crc ^= (uint16_t)(byte << 8);

    for (bit = 0; bit < 8; bit++)
      crc = (crc & 0x8000) ? (uint16_t)((crc << 1) ^ 0x1021) : (uint16_t)(crc << 1);
  }

  return (crc == 0);
}

bool
caption_data_load (const PacketPES_t *pes,
                   CaptionData_t *caption)
{
  assert (pes != NULL);
  assert (caption != NULL);
  const uint8_t *data = pes->payload.data;
  uint16_t len = pes->payload.len;
  uint16_t group_len = 0;

  // 5 bytes de header + 2 bytes do crc16, no minimo.
  if (data == NULL || len < 7) {
      return FALSE;
  }
  
  memset (caption, 0, sizeof (CaptionData_t));

  // Carregando o header conforme a norma ARIB STD-B24, Pag 152, Tabela 9.1
  caption->group_id = ((data[0] & 0xFC) >> 2); // 11111100b
  caption->version = (data[0] & 0x03); // 00000011b
  caption->link_number = data[1];
  caption->last_link_number = data[2];
  caption->datalen = (data[3] << 8 | data[4]);
  caption->data = &data[5];

  // O payload pode ter bytes de 'stuffing' depois do data_group, entao o
  // crc16 deve ser lido logo apos os dados, e nao no final do payload.
  if ((uint32_t)caption->datalen + 7 > len)
  {
    ATV_DEBUG ("Ops!! data_group_size (%d) maior que o payload (%d), saindo...", caption->datalen, len);
    return FALSE;
  }

  group_len = caption->datalen + 5;
  caption->crc16 = (data[group_len] << 8 | data[group_len + 1]);

  caption->pts = &pes->pts; // Apenas referencia.

#ifdef DEBUG_CAPTION_DATA
  ATV_HEXDUMP (pes->payload.data, pes->payload.len);
  ATV_DEBUG ("\n"
     "<CaptionData_t>\n"
     " caption {\n"
     "  group_id = %d,\n"
     "  version = %d,\n"
     "  link_number = %d,\n"
     "  last_link_number = %d,\n"
     "  datalen = %d (%#04x),\n"
     "  data = { [0]='%#02x', [1]='%#02x', [2]='%#02x', [3]='%#02x', ... },\n"
     "  crc16 = %d (%#02x)\n"
     " }\n"
     "</CaptionData_t>",
     caption->group_id, caption->version, caption->link_number, caption->last_link_number,
     caption->datalen, caption->datalen, caption->data[0], caption->data[1], caption->data[2],
     caption->data[3], caption->crc16, caption->crc16
  );
  ATV_HEXDUMP (caption->data, caption->datalen);
#endif

  // crc valido?
  if (!caption_data_check_crc16 (data, &group_len, &caption->crc16))
  {
    ATV_DEBUG ("Ops!! Falhou verificacao do crc16(%#x), saindo...", caption->crc16);
    return FALSE;
  }

  return TRUE;
}

