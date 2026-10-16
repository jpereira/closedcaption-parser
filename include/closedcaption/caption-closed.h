/**
 * @file caption-closed.h
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @date Thu Aug 12 13:49:21 BRT 2010
 * @brief Interpretação do texto da legenda (closed caption) e do seu estado:
 *        cores, tamanho dos caracteres, posição e formato de exibição.
 *
 * caption_closed_run() percorre o @c statement body (texto + comandos de
 * controle) e:
 *
 * - converte cada caractere ISO-8859-15 para UTF-8 e o entrega ao hook de
 *   texto (veja caption-hooking.h);
 * - executa cada comando C0/C1 (veja caption-closed-controls.h), atualiza o
 *   CaptionClosed_t e chama o hook de controles.
 */
#ifndef CAPTION_CLOSED_H_
#define CAPTION_CLOSED_H_

#include "caption-common.h"
#include "caption-data.h"
#include "caption-management.h"

/**
 * Tamanho dos caracteres (comandos SSZ, MSZ, NSZ e SZX).
 */
typedef enum {
  CC_CharacterSize_Small = 0x01,    /**< Pequeno (SSZ). */
  CC_CharacterSize_Middle = 0x02,   /**< Médio (MSZ). */
  CC_CharacterSize_Standard = 0x03, /**< Normal (NSZ). */
  CC_CharacterSize_t_eMax = 0x04,   /**< Limite dos valores acima. */

  CC_CharacterSize_Custom = 0x05,   /**< Tamanho especificado pelo SZX; o valor está em CaptionClosed_t::char_size_custom. */
  CC_CharacterSize_UNKNOWN = 0xff   /**< Desconhecido. */
} CC_CharacterSize_t;

/**
 * Formato de exibição (ARIB STD-B24, Tabela 9-7; comando CSI SWF).
 */
typedef enum {
  CC_Format_HOR_STAN_DENSITY  = 0x00, /**< Horizontal, densidade padrão. */
  CC_Format_VER_STAN_DENSITY  = 0x01, /**< Vertical, densidade padrão. */
  CC_Format_HOR_HIGH_DENSITY  = 0x02, /**< Horizontal, alta densidade. */
  CC_Format_VER_HIGH_DENSITY  = 0x03, /**< Vertical, alta densidade. */
  CC_Format_HOR_WEST_LANG     = 0x04, /**< Horizontal, idiomas ocidentais. */
  CC_Format_HOR_1920x1080     = 0x05, /**< Horizontal, 1920x1080. */
  CC_Format_VER_1920x1080     = 0x06, /**< Vertical, 1920x1080. */
  CC_Format_HOR_960x540       = 0x07, /**< Horizontal, 960x540. */
  CC_Format_VER_960x540       = 0x08, /**< Vertical, 960x540. */
  CC_Format_HOR_720x480       = 0x09, /**< Horizontal, 720x480. */
  CC_Format_VER_720x480       = 0x0a, /**< Vertical, 720x480. */
  CC_Format_HOR_1280x720      = 0x0b, /**< Horizontal, 1280x720. */
  CC_Format_VER_1280x720      = 0x0c, /**< Vertical, 1280x720. */
  CC_Format_t_eMax            = 0x0d  /**< Quantidade de formatos. */
} CC_Format_t;

/**
 * Largura e altura, em pontos.
 */
typedef struct {
  uint16_t width;  /**< Largura. */
  uint16_t height; /**< Altura. */
} CC_Dimension_t;

/** Compara duas CC_Dimension_t. */
#define CC_Dimension_is_equal(dimen1, dimen2) \
  ((dimen1).width == (dimen2).width && (dimen1).height == (dimen2).height)

/**
 * Cor RGBA.
 */
typedef struct {
  uint8_t r; /**< Vermelho. */
  uint8_t g; /**< Verde. */
  uint8_t b; /**< Azul. */
  uint8_t a; /**< Opacidade: 0x00 é transparente, 0xff é opaco. */
} CC_Color_t;

/** Compara duas CC_Color_t. */
#define CC_Color_is_equal(color1, color2) \
  ((color1).r == (color2).r && (color1).g == (color2).g && \
   (color1).b == (color2).b && (color1).a == (color2).a)

/**
 * Cor de raster (comando CSI RCS, Raster Colour Command).
 */
typedef enum {
  CC_Rcs_BLACK = 0,                  /**< Preto. */
  CC_Rcs_FULL_INTENSITY_RED = 1,     /**< Vermelho. */
  CC_Rcs_FULL_INTENSITY_GREEN = 2,   /**< Verde. */
  CC_Rcs_FULL_INTENSITY_YELLOW = 3,  /**< Amarelo. */
  CC_Rcs_FULL_INTENSITY_BLUE = 4,    /**< Azul. */
  CC_Rcs_FULL_INTENSITY_MAGNETA = 5, /**< Magenta. */
  CC_Rcs_FULL_INTENSITY_CYAN = 6,    /**< Ciano. */
  CC_Rcs_FULL_INTENSITY_WHITE = 7,   /**< Branco. */
  CC_Rcs_TRANSPARENT = 8,            /**< Transparente. */
  Cc_Rcs_HALF_INTENSITY_RED = 9,     /**< Vermelho, meia intensidade. */
  Cc_Rcs_HALF_INTENSITY_GREEN = 10,  /**< Verde, meia intensidade. */
  Cc_Rcs_HALF_INTENSITY_YELLOW = 11, /**< Amarelo, meia intensidade. */
  Cc_Rcs_HALF_INTENSITY_BLUE = 12,   /**< Azul, meia intensidade. */
  Cc_Rcs_HALF_INTENSITY_MAGNETA = 13,/**< Magenta, meia intensidade. */
  Cc_Rcs_HALF_INTENSITY_CYAN = 14,   /**< Ciano, meia intensidade. */
  Cc_Rcs_HALF_INTENSITY_WHITE = 15,  /**< Branco, meia intensidade. */
  CC_Rcs_t_eMax = 16                 /**< Quantidade de cores. */
} CC_Rcs_t;

/**
 * Estado da legenda, atualizado pelos comandos de controle durante o
 * caption_closed_run().
 *
 * Mantenha a mesma estrutura entre os PES de um mesmo PID: alguns comandos
 * (cores, tamanho, formato) valem até serem alterados novamente.
 * Inicialize com zeros.
 */
typedef struct {
  CC_Format_t format;           /**< Formato de exibição (CSI SWF). */
  CC_CharacterSize_t char_size; /**< Tamanho dos caracteres (SSZ, MSZ, NSZ, SZX). */
  CC_Dimension_t SDF;           /**< Tamanho da área de exibição (CSI SDF, Set Display Format). */
  CC_Dimension_t SDP;           /**< Posição da área de exibição (CSI SDP, Set Display Position). */
  CC_Dimension_t SSM;           /**< Tamanho do caractere em pontos (CSI SSM). */
  CC_Rcs_t RCS;                 /**< Cor de raster (CSI RCS). */
  CC_Color_t foreground;        /**< Cor do texto (BKF ~ WHF, COL). */
  CC_Color_t background;        /**< Cor de fundo (COL). */
  CC_Color_t half_foreground;   /**< Cor do texto, meia intensidade (COL). */
  CC_Color_t half_background;   /**< Cor de fundo, meia intensidade (COL). */

  /** Posição do cursor (APS, Active Position Set). */
  struct APS {
    uint8_t first;  /**< Linha (0 a 63). */
    uint8_t second; /**< Coluna (0 a 63). */
  } APS; /**< Posição do cursor (APS). */

  bool need_clear;                /**< TRUE quando o comando CS (limpar a tela) foi recebido. */
  uint8_t char_size_custom;       /**< Parâmetro do SZX, quando @ref char_size é @ref CC_CharacterSize_Custom. */
  uint8_t char_number_in_oneline; /**< Caracteres por linha (CSI SWF). */
  uint8_t char_line_number;       /**< Quantidade de linhas (CSI SWF). */
  uint16_t SHS;                   /**< Espaçamento horizontal (CSI SHS, Set Horizontal Spacing). */
  uint16_t SVS;                   /**< Espaçamento vertical (CSI SVS, Set Vertical Spacing). */

  const PES_pts_t *pts;           /**< PTS do PES em processamento; válido apenas durante o caption_closed_run(). */

  void *userdata;                 /**< Livre para o usuário; útil para recuperar o contexto dentro dos hooks. */
} CaptionClosed_t;

/**
 * Processa o texto da legenda.
 *
 * Percorre o @c statement body de @p m, entregando cada caractere (em UTF-8)
 * ao hook de texto e executando cada comando de controle (que atualiza @p cc
 * e chama o hook de controles). Veja caption-hooking.h.
 *
 * Caracteres fora do ISO-8859-15 e comandos não suportados em closed caption
 * são ignorados, junto com os seus parâmetros.
 *
 * @param m   Conteúdo do tipo @ref CAPTION_HANDLER_TYPE_STATEMENT_BODY, lido
 *            por caption_management_load().
 * @param cc  Estado da legenda, mantido entre as chamadas.
 * @return TRUE.
 */
bool caption_closed_run (const CaptionManagement_t *m,
                         CaptionClosed_t *cc);

/**
 * Converte um caractere ISO-8859-15 (Latin-9) para UTF-8.
 *
 * @param from    Caractere de origem (1 byte).
 * @param to      Recebe o caractere em UTF-8, terminado em '\\0'.
 * @param to_len  Tamanho de @p to; deve ser 4 ou mais.
 * @return A quantidade de bytes escritos em @p to, sem o '\\0' (1 a 3).
 */
uint8_t char_latin9_to_utf8 (const char *const from,
                             char *const to,
                             uint8_t to_len);

#endif /* CAPTION_CLOSED_H_ */
