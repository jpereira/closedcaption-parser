/**
 * @file: caption-data.c
 * @autor: Jorge Pereira <jorge.pereira@tqtvd.com>
 * @date: Thu Aug 12 13:49:21 BRT 2010
 * @desc: Mais informações em 'arquitetura-closedcaption.txt'
 */
#include "caption-data.h"
#include "caption-closed.h"
#include "caption-superimpose.h"

//#define DEBUG_CAPTION_DATA

bool
caption_data_check_crc16 (const uint8_t *data,
                          const uint16_t *datalen,
                          const uint16_t *crc16)
{
  // TODO: Implementar conforme ARIB STD-B24, Pag 153
 
  (void)data;
  (void)datalen;
  (void)crc16;

  return TRUE;
}

bool
caption_data_load (const PacketPES_t *pes,
                   CaptionData_t *caption)
{
  assert (pes != NULL);
  assert (caption != NULL);
  const uint8_t *data = pes->payload.data;
  uint16_t len = pes->payload.len;

  if (data == NULL || len == 0) {
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
  caption->crc16 = (data[len-2] << 8 | data[len-1]);

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
  if (!caption_data_check_crc16 (data, &len, &caption->crc16))
  {
    ATV_DEBUG ("Ops!! Falhou verificacao do crc16(%#x), saindo...", caption->crc16);
    return FALSE;
  }

  return TRUE;
}

