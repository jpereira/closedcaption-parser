/**
 * @file: caption-management.c
 * @autor: Jorge Pereira <jorge.pereira@tqtvd.com>
 * @date: Thu Aug 12 13:49:21 BRT 2010
 * @desc: Mais informações em 'arquitetura-closedcaption.txt'
 */
#include "caption-management.h"

#define DEBUG_CAPTION_MANAGEMENT

bool
caption_management_load (const CaptionData_t *caption,
                         CaptionManagement_t *mgmt)
{
  assert (caption != NULL);
  const uint8_t *data = caption->data;
  CaptionManagement_t *m = mgmt;
  uint8_t pos = 0;

  assert (m != NULL);
//  assert (caption->pts != NULL);
//  assert (caption->data != NULL);

  if (!caption->data || caption->datalen == 0) {
    return FALSE;
  }

  memset (m, 0, sizeof (CaptionManagement_t));

  m->pts = caption->pts; // Apenas referencia.
  
  // Seguindo conforme norma ARIB STD-B24, Pag 153, Tabela 9.3.1
  m->TMD = ((data[pos] & 0xc0) >> 6); // 11000000b
  m->reserved = (data[pos] & 0x3f);   // 00111111b

  if (m->TMD == 0x02 && caption->datalen > 6) // Offset Time?
  {
    // TODO: resolver isto
    m->TMD_otm = (data[pos+1] << 8 | data[pos+2] << 16 | data[pos+3] << 24) << 16;
    pos += 5;
  }

  // Assumindo que sempre 'num_lang' sera '0', ou seja! tera apenas um idioma.
  m->num_lang = (data[pos+1]);             // 2-byte: 11111111b
  m->lang_tag = (data[pos+2] & 0xe0) >> 5; // 3-byte: 11100000b
//m->reserved = (data[pos+2] & 0x10) >> 4; // 3-byte: 00010000b (reserved)
  m->DMF      = (data[pos+2] & 0x0f);      // 3-byte: 00001111b

  if (m->DMF == 0x0c || m->DMF == 0x0d || m->DMF == 0x0e) // Padrao e 0x00
  {
    m->DC = data[pos+3];
    pos  += 1;
  }

  if (m->num_lang > 0) // Opa, tem idioma?!
  {
    // Assumindo que "m->" passou por um previo memset() de '0'
    // e o tamanho do buffer 'ISO_639_lang_code' e 3+1, +1 este
    // referente ao '\0'. basta apenas copiar tres posicoes referente
    // ao idioma. [4-6]-Byte (11111111b, 11111111b, 11111111b) = 24-bits

    pos += 1;
    memcpy (&m->ISO_639_lang_code[0], &data[pos+2], 3);

    // 3-Byte, padrao do DMF e '0x00', chamado de 'Message display of attenuation
    // due to rain'. diferente disso e especificado pelo broadcast.
    m->format      = (data[pos+5] & 0xf0) >> 4; // 5-Byte: 11110000b
    m->TCS         = (data[pos+5] & 0x0c) >> 2; // 5-Byte: 00001100b
    m->rollup_mode = (data[pos+5] & 0x03);      // 5-Byte: 00000011b
  }
  else
  {
    // (Caption Statement Data) ARIB STD-B24: Cap 9.3.2
    m->statement.unit_sep   = data[4];
    m->statement.unit_param = data[5];
    m->statement.unit_size  = (data[pos+6] << 16 | data[pos+7] << 8 | data[pos+8]); // 6,7,8-Bytes:
    m->statement.unit_data  = (m->statement.unit_size > 0) ? &data[pos+9] : NULL; // tem dados?
  }

  return TRUE;
}

CaptionManagementType_t
caption_management_get_type (const CaptionManagement_t *management)
{
  const CaptionManagement_t *m = management;

  assert (management != NULL);

  if (m->statement.unit_size > 0)
  {
    switch (m->statement.unit_param)
    {
      case _CAPTION_HANDLER_STATEMENT_BODY:
        return CAPTION_HANDLER_TYPE_STATEMENT_BODY;
      
      case _CAPTION_HANDLER_GEOMETRIC:
        return CAPTION_HANDLER_TYPE_GEOMETRIC;

      case _CAPTION_HANDLER_SYNTHESIZED_SOUND:
        return CAPTION_HANDLER_TYPE_SYNTHESIZED_SOUND;

      case _CAPTION_HANDLER_1BYTE_DRCS:
        return CAPTION_HANDLER_TYPE_1BYTE_DRCS;

      case _CAPTION_HANDLER_2BYTE_DRCS:
        return CAPTION_HANDLER_TYPE_2BYTE_DRCS;

      case _CAPTION_HANDLER_COLOR_MAP:
        return CAPTION_HANDLER_TYPE_COLOR_MAP;

      case _CAPTION_HANDLER_BITMAP:
        return CAPTION_HANDLER_TYPE_BITMAP;
      
      default:
        return CAPTION_HANDLER_TYPE_UNKNOWN;
    }
  }

  return CAPTION_HANDLER_TYPE_MANAGEMENT;
}

