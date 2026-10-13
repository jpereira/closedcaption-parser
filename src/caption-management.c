/**
 * @file: caption-management.c
 * @autor: Jorge Pereira <jpereiran@gmail.com>
 * @date: Thu Aug 12 13:49:21 BRT 2010
 * @desc: Mais informações em 'arquitetura-closedcaption.txt'
 */
#include "caption-management.h"

bool
caption_management_load (const CaptionData_t *caption,
                         CaptionManagement_t *mgmt)
{
  assert (caption != NULL);
  const uint8_t *data = caption->data;
  const uint32_t len = caption->datalen;
  CaptionManagement_t *m = mgmt;
  // group_id 0x00 (grupo A) ou 0x20 (grupo B) e "caption management data",
  // os demais sao "caption statement data". (ARIB STD-B24, Pag 152, 9.2)
  const bool is_management = ((caption->group_id & 0x0f) == 0);
  uint32_t pos = 0;
  uint32_t end = 0;
  uint8_t i;

  assert (m != NULL);

  if (!caption->data || caption->datalen == 0) {
    return FALSE;
  }

  memset (m, 0, sizeof (CaptionManagement_t));

  m->pts = caption->pts; // Apenas referencia.
  
  // Seguindo conforme norma ARIB STD-B24, Pag 153, Tabela 9.3.1
  m->TMD = ((data[pos] & 0xc0) >> 6); // 11000000b
  m->reserved = (data[pos] & 0x3f);   // 00111111b
  pos += 1;

  // OTM (management, TMD=10) ou STM (statement, TMD=01 ou 10):
  // 36-bits (9 digitos BCD) + 4-bits reservados.
  if ((is_management && m->TMD == MANAGEMENT_TMD_OFFSET) ||
      (!is_management && (m->TMD == MANAGEMENT_TMD_REALTIME || m->TMD == MANAGEMENT_TMD_OFFSET)))
  {
    if (pos + 5 > len)
      return FALSE;

    m->TMD_otm = ((uint64_t)data[pos] << 28 | (uint64_t)data[pos+1] << 20 |
                  (uint64_t)data[pos+2] << 12 | (uint64_t)data[pos+3] << 4 | data[pos+4] >> 4);
    pos += 5;
  }

  if (is_management)
  {
    if (pos + 1 > len)
      return FALSE;

    m->num_lang = data[pos];
    pos += 1;

    // Apenas o primeiro idioma e armazenado em 'm', os demais sao ignorados.
    for (i = 0; i < m->num_lang; i++)
    {
      uint8_t lang_tag, DMF, DC = 0;

      if (pos + 1 > len)
        return FALSE;

      lang_tag = (data[pos] & 0xe0) >> 5; // 11100000b
      DMF      = (data[pos] & 0x0f);      // 00001111b
      pos += 1;

      if (DMF == 0x0c || DMF == 0x0d || DMF == 0x0e) // Padrao e 0x00
      {
        if (pos + 1 > len)
          return FALSE;

        DC   = data[pos];
        pos += 1;
      }

      // ISO_639_language_code (24-bits) + Format/TCS/rollup_mode (8-bits)
      if (pos + 4 > len)
        return FALSE;

      if (i == 0)
      {
        m->lang_tag = lang_tag;
        m->DMF      = DMF;
        m->DC       = DC;
        memcpy (&m->ISO_639_lang_code[0], &data[pos], 3);

        m->format      = (data[pos+3] & 0xf0) >> 4; // 11110000b
        m->TCS         = (data[pos+3] & 0x0c) >> 2; // 00001100b
        m->rollup_mode = (data[pos+3] & 0x03);      // 00000011b
      }

      pos += 4;
    }

    // As 'data units' do management nao sao tratadas.
    return TRUE;
  }

  // (Caption Statement Data) ARIB STD-B24: Cap 9.3.2
  if (pos + 3 > len)
    return FALSE;

  end  = (data[pos] << 16 | data[pos+1] << 8 | data[pos+2]); // data_unit_loop_length
  pos += 3;
  end  = (pos + end > len) ? len : pos + end;

  // Pode haver mais de uma 'data unit' (ex: DRCS antes do texto). Da preferencia
  // ao 'statement body', senao fica com a primeira encontrada.
  while (pos + 5 <= end)
  {
    uint8_t unit_sep   = data[pos];
    uint8_t unit_param = data[pos+1];
    uint32_t unit_size = (data[pos+2] << 16 | data[pos+3] << 8 | data[pos+4]);

    if (unit_sep != 0x1f) // unit_separator
      break;

    pos += 5;

    if (unit_size > end - pos)
      unit_size = end - pos;

    if (m->statement.unit_sep == 0 ||
        (unit_param == _CAPTION_HANDLER_STATEMENT_BODY &&
         m->statement.unit_param != _CAPTION_HANDLER_STATEMENT_BODY))
    {
      m->statement.unit_sep   = unit_sep;
      m->statement.unit_param = unit_param;
      m->statement.unit_size  = unit_size;
      m->statement.unit_data  = (unit_size > 0) ? &data[pos] : NULL; // tem dados?
    }

    pos += unit_size;
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

