/**
 * @file: caption-closed.h
 * @autor: Jorge Pereira <jorge.pereira@tqtvd.com>
 * @date: Thu Aug 12 13:49:21 BRT 2010
 * @desc: Mais informações em 'arquitetura-closedcaption.txt'
 */
#ifndef CAPTION_CLOSED_H_
#define CAPTION_CLOSED_H_

#include "caption-common.h"
#include "caption-data.h"
#include "caption-management.h"

typedef enum {
  CC_CharacterSize_Small = 0x01 ,
  CC_CharacterSize_Middle = 0x02,
  CC_CharacterSize_Standard = 0x03,
  CC_CharacterSize_t_eMax = 0x04,
  
  CC_CharacterSize_Custom = 0x05, // Hacking: Caso receba um SZX, será um "Custom" e utilizar valor de "char_size_custom"
  CC_CharacterSize_UNKNOWN = 0xff
} CC_CharacterSize_t;

typedef enum {
// Arib STD-B24, Tabela 9-7
  CC_Format_HOR_STAN_DENSITY  = 0x00,
  CC_Format_VER_STAN_DENSITY  = 0x01,
  CC_Format_HOR_HIGH_DENSITY  = 0x02,
  CC_Format_VER_HIGH_DENSITY  = 0x03,
  CC_Format_HOR_WEST_LANG     = 0x04,
  CC_Format_HOR_1920x1080     = 0x05,
  CC_Format_VER_1920x1080     = 0x06,
  CC_Format_HOR_960x540       = 0x07,
  CC_Format_VER_960x540       = 0x08,
  CC_Format_HOR_720x480       = 0x09,
  CC_Format_VER_720x480       = 0x0a,
  CC_Format_HOR_1280x720      = 0x0b,
  CC_Format_VER_1280x720      = 0x0c,
  CC_Format_t_eMax            = 0x0d
} CC_Format_t;

typedef struct {
  uint16_t width;
  uint16_t height;
} CC_Dimension_t;
#define CC_Dimension_is_equal(dimen1, dimen2) (*((uint32_t*)&dimen1) == *((uint32_t*)&dimen2))

typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;
} CC_Color_t;
#define CC_Color_is_equal(color1, color2) (*((uint32_t*)&color1) == *((uint32_t*)&color2))

/**
 * Raster Colour Command
 */
typedef enum {
  CC_Rcs_BLACK = 0,
  CC_Rcs_FULL_INTENSITY_RED = 1,
  CC_Rcs_FULL_INTENSITY_GREEN = 2,
  CC_Rcs_FULL_INTENSITY_YELLOW = 3,
  CC_Rcs_FULL_INTENSITY_BLUE = 4,
  CC_Rcs_FULL_INTENSITY_MAGNETA = 5,
  CC_Rcs_FULL_INTENSITY_CYAN = 6,
  CC_Rcs_FULL_INTENSITY_WHITE = 7,
  CC_Rcs_TRANSPARENT = 8,
  Cc_Rcs_HALF_INTENSITY_RED = 9,
  Cc_Rcs_HALF_INTENSITY_GREEN = 10,
  Cc_Rcs_HALF_INTENSITY_YELLOW = 11,
  Cc_Rcs_HALF_INTENSITY_BLUE = 12,
  Cc_Rcs_HALF_INTENSITY_MAGNETA = 13,
  Cc_Rcs_HALF_INTENSITY_CYAN = 14,
  Cc_Rcs_HALF_INTENSITY_WHITE = 15,
  CC_Rcs_t_eMax = 16
} CC_Rcs_t;

typedef struct { 
  CC_Format_t format;
  CC_CharacterSize_t char_size;
  CC_Dimension_t SDF; /* Set Display Format */
  CC_Dimension_t SDP; /* Set Display Position */
  CC_Dimension_t SSM; /* Character composition dot designation */
  CC_Rcs_t RCS; /* Raster Colour Command */
  CC_Color_t foreground;
  CC_Color_t background;
  CC_Color_t half_foreground;
  CC_Color_t half_background;

  struct APS {
    uint8_t first;
    uint8_t second;
  } APS;
  
  bool need_clear; /* Utilizada para sinalizar o recebimento do comando limpar tela. */
  uint8_t char_size_custom;
  uint8_t char_number_in_oneline;
  uint8_t char_line_number;
  uint16_t SHS; /* Set Horizontal Spacing */
  uint16_t SVS; /* Set Vertical Spacing */

  const PES_pts_t *pts; /* apenas referencia */
} CaptionClosed_t;

/**
 * Utilizado na criacao do objeto CaptionClosed_t utilizando como
 * fonte um objeto do tipo CaptionManagement_t previamente alocado.
 *
 * Lembrando que caso tenha mais de uma legenda "CC" dentro do pacote
 * CaptionManagement_t, seja 
 *
 * @param m Um pacote de dados valido do tipo CaptionManagement_t
 * @return Se 'm' for valido, retorna um CaptionClosed_t, liberar atraves de caption_closed_free()
 */
bool caption_closed_run (const CaptionManagement_t *m, CaptionClosed_t *cc);

#endif /*CAPTION_CLOSED_H_*/
