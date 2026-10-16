/**
 * @file caption-data.c
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @date Thu Aug 12 13:49:21 BRT 2010
 * @brief Leitura do data_group da legenda e verificação do CRC-16.
 */
#include "caption-data.h"
#include "caption-closed.h"
#include "caption-superimpose.h"

/*
 * CRC-16 (ITU-T, polinômio x^16 + x^12 + x^5 + 1, valor inicial 0) calculado
 * sobre todo o data_group (ARIB STD-B24, Volume 3, 9.2). Como o CRC fica no
 * final do grupo, o cálculo sobre 'data' + 'crc16' deve resultar em zero.
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

  // No mínimo: 5 bytes de cabeçalho + 2 bytes do CRC-16.
  if (data == NULL || len < 7) {
      return FALSE;
  }
  
  memset (caption, 0, sizeof (CaptionData_t));

  // Cabeçalho do data_group (ARIB STD-B24, Volume 3, Tabela 9-1)
  caption->group_id = ((data[0] & 0xFC) >> 2); // 11111100b
  caption->version = (data[0] & 0x03); // 00000011b
  caption->link_number = data[1];
  caption->last_link_number = data[2];
  caption->datalen = (data[3] << 8 | data[4]);
  caption->data = &data[5];

  // O payload pode ter bytes de 'stuffing' depois do data_group, então o
  // CRC-16 deve ser lido logo após os dados, e não no final do payload.
  if ((uint32_t)caption->datalen + 7 > len)
  {
    DTV_DEBUG ("data_group_size (%d) maior que o payload (%d), ignorando.", caption->datalen, len);
    return FALSE;
  }

  group_len = caption->datalen + 5;
  caption->crc16 = (data[group_len] << 8 | data[group_len + 1]);

  caption->pts = &pes->pts; // apenas referência

#ifdef DEBUG_CAPTION_DATA
  DTV_HEXDUMP (pes->payload.data, pes->payload.len);
  DTV_DEBUG ("\n"
     "<CaptionData_t>\n"
     " caption {\n"
     "  group_id = %d,\n"
     "  version = %d,\n"
     "  link_number = %d,\n"
     "  last_link_number = %d,\n"
     "  datalen = %d (%#04x),\n"
     "  data = { [0]='%#02x', ... },\n"
     "  crc16 = %d (%#02x)\n"
     " }\n"
     "</CaptionData_t>",
     caption->group_id, caption->version, caption->link_number, caption->last_link_number,
     caption->datalen, caption->datalen, (caption->datalen > 0) ? caption->data[0] : 0,
     caption->crc16, caption->crc16
  );
  DTV_HEXDUMP (caption->data, caption->datalen);
#endif

  // CRC-16 válido?
  if (!caption_data_check_crc16 (data, &group_len, &caption->crc16))
  {
    DTV_DEBUG ("CRC-16 (%#x) não confere, ignorando.", caption->crc16);
    return FALSE;
  }

  return TRUE;
}

