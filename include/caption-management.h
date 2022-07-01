/**
 * @file: caption-management.h
 * @autor: Jorge Pereira <jorge.pereira@tqtvd.com>
 * @date: Thu Aug 12 13:49:21 BRT 2010
 * @desc: Mais informações em 'arquitetura-closedcaption.txt'
 */
#ifndef CAPTION_MANAGEMENT_H_
#define CAPTION_MANAGEMENT_H_

#include "packet-pes.h"
#include "caption-data.h"
#include "caption-common.h"

#define MANAGEMENT_TMD_FREE                  0x00
#define MANAGEMENT_TMD_REALTIME              0x01
#define MANAGEMENT_TMD_OFFSET                0x10
#define MANAGEMENT_TMD_RESERVED              0x11

// Arib STD-B24, Tabela 9-8
#define MANAGEMENT_TCS_8BIT                  0x00
#define MANAGEMENT_TCS_UCS                   0x01

// Arib STD-B24, Tabela 9-9
#define MANAGEMENT_ROLL_NONE                 0x00 
#define MANAGEMENT_ROLL_UP                   0x01

#define _CAPTION_HANDLER_STATEMENT_BODY    0x20
#define _CAPTION_HANDLER_GEOMETRIC         0x28
#define _CAPTION_HANDLER_SYNTHESIZED_SOUND 0x2c
#define _CAPTION_HANDLER_1BYTE_DRCS        0x30
#define _CAPTION_HANDLER_2BYTE_DRCS        0x31
#define _CAPTION_HANDLER_COLOR_MAP         0x34
#define _CAPTION_HANDLER_BITMAP            0x35

typedef enum {
  CAPTION_HANDLER_TYPE_UNKNOWN           = 0,
  CAPTION_HANDLER_TYPE_STATEMENT_BODY    = 1,
  CAPTION_HANDLER_TYPE_GEOMETRIC         = 2,
  CAPTION_HANDLER_TYPE_SYNTHESIZED_SOUND = 3,
  CAPTION_HANDLER_TYPE_1BYTE_DRCS        = 4,
  CAPTION_HANDLER_TYPE_2BYTE_DRCS        = 5,
  CAPTION_HANDLER_TYPE_COLOR_MAP         = 6,
  CAPTION_HANDLER_TYPE_BITMAP            = 7,

  CAPTION_HANDLER_TYPE_MANAGEMENT        = 8,

  CAPTION_HANDLER_TYPE_MAX               = 9
} CaptionManagementType_t;

// Conforme norma ARIB STD-B24, Pag 153, 9.3.1
typedef struct CaptionManagement_t {
  uint8_t TMD;
  uint64_t TMD_otm; /** 36-bits (offset time, 9 4-bit BCD) + 4-bits reserved */
  uint8_t reserved; /** reserved field */

  uint8_t num_lang; /** Number of Languages */
  uint8_t lang_tag; /** Identification of Language */
  uint8_t DMF; /** 4-bit, Display Mode */
  uint8_t DC; /** 8-bit, Display Condition Designation */ 
  char ISO_639_lang_code[3+1]; /** 24-bit, Language Code */
  uint8_t format; /** Display format */
  uint8_t TCS; /** Character coding, (0x00=8bit, 0x01=ucs, 0x10|0x11=reserved) */
  uint8_t rollup_mode; /** 2-bit, Roll-up mode */

  /** Devido ao topico 9.3.2 ser pequeno, adicionei ele abaixo. */
  struct statement { /* Table B7-7: Encoding Example of Digital Closed Caption
                      * Text Data Converted from Analog */
    uint8_t unit_sep; /** */
    uint8_t unit_param; /** */
    uint32_t unit_size; /** 24-bit: Tamanho de *data */
    const uint8_t *unit_data;
  } statement;

  const PES_pts_t *pts; // apenas referencia.
} CaptionManagement_t;

bool
caption_management_load (const CaptionData_t *caption,
                         CaptionManagement_t *management);

CaptionManagementType_t
caption_management_get_type (const CaptionManagement_t *management);

#endif /* CAPTION_MANAGEMENT_H_*/

