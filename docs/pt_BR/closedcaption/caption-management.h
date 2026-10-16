/**
 * @file caption-management.h
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @date Thu Aug 12 13:49:21 BRT 2010
 * @brief Leitura do "caption management data" e do "caption statement data"
 *        (ARIB STD-B24, Volume 3, 9.3).
 *
 * O conteúdo do @c data_group (veja caption-data.h) depende do
 * @c data_group_id:
 *
 * - <b>caption management data</b> (@c data_group_id 0x00 ou 0x20): modo de
 *   tempo (TMD), idiomas disponíveis, formato de exibição, codificação dos
 *   caracteres (TCS) e modo roll-up;
 * - <b>caption statement data</b> (demais valores): modo de tempo e as
 *   <i>data units</i> com o texto da legenda (@c statement body), DRCS,
 *   bitmaps, etc.
 *
 * caption_management_load() lê os dois formatos para a mesma estrutura,
 * CaptionManagement_t, e caption_management_get_type() informa o que foi lido.
 */
#ifndef CAPTION_MANAGEMENT_H_
#define CAPTION_MANAGEMENT_H_

#include "packet-pes.h"
#include "caption-data.h"
#include "caption-common.h"

/**
 * @name TMD: modo de tempo (ARIB STD-B24, Tabela 9-4, valores binários 00 a 11)
 * @{
 */
#define MANAGEMENT_TMD_FREE                0x00 /**< Exibição livre, sem sincronismo de tempo. */
#define MANAGEMENT_TMD_REALTIME            0x01 /**< Tempo real: o statement data traz o STM. */
#define MANAGEMENT_TMD_OFFSET              0x02 /**< Tempo com offset: management traz o OTM, statement traz o STM. */
#define MANAGEMENT_TMD_RESERVED            0x03 /**< Reservado. */
/** @} */

/**
 * @name TCS: codificação dos caracteres (ARIB STD-B24, Tabela 9-8)
 * @{
 */
#define MANAGEMENT_TCS_8BIT                0x00 /**< Código de 8 bits (ISO-8859-15 no Brasil). */
#define MANAGEMENT_TCS_UCS                 0x01 /**< UCS (Unicode). Não suportado. */
/** @} */

/**
 * @name Modo roll-up (ARIB STD-B24, Tabela 9-9)
 * @{
 */
#define MANAGEMENT_ROLL_NONE               0x00 /**< Sem roll-up. */
#define MANAGEMENT_ROLL_UP                 0x01 /**< Com roll-up. */
/** @} */

/**
 * @name data_unit_parameter: tipo de cada data unit (ARIB STD-B24, Tabela 9-12)
 * @{
 */
#define _CAPTION_HANDLER_STATEMENT_BODY    0x20 /**< Texto da legenda. */
#define _CAPTION_HANDLER_GEOMETRIC         0x28 /**< Gráficos geométricos. */
#define _CAPTION_HANDLER_SYNTHESIZED_SOUND 0x2c /**< Som sintetizado. */
#define _CAPTION_HANDLER_1BYTE_DRCS        0x30 /**< DRCS de 1 byte (caracteres desenhados pela emissora). */
#define _CAPTION_HANDLER_2BYTE_DRCS        0x31 /**< DRCS de 2 bytes. */
#define _CAPTION_HANDLER_COLOR_MAP         0x34 /**< Mapa de cores. */
#define _CAPTION_HANDLER_BITMAP            0x35 /**< Bitmap. */
/** @} */

/**
 * Tipo de conteúdo lido por caption_management_load().
 */
typedef enum {
  CAPTION_HANDLER_TYPE_UNKNOWN           = 0, /**< Data unit de tipo desconhecido. */
  CAPTION_HANDLER_TYPE_STATEMENT_BODY    = 1, /**< Texto da legenda: processe com caption_closed_run(). */
  CAPTION_HANDLER_TYPE_GEOMETRIC         = 2, /**< Gráficos geométricos. */
  CAPTION_HANDLER_TYPE_SYNTHESIZED_SOUND = 3, /**< Som sintetizado. */
  CAPTION_HANDLER_TYPE_1BYTE_DRCS        = 4, /**< DRCS de 1 byte. */
  CAPTION_HANDLER_TYPE_2BYTE_DRCS        = 5, /**< DRCS de 2 bytes. */
  CAPTION_HANDLER_TYPE_COLOR_MAP         = 6, /**< Mapa de cores. */
  CAPTION_HANDLER_TYPE_BITMAP            = 7, /**< Bitmap. */

  CAPTION_HANDLER_TYPE_MANAGEMENT        = 8, /**< Caption management data, ou statement sem data units. */

  CAPTION_HANDLER_TYPE_MAX               = 9  /**< Quantidade de tipos. */
} CaptionManagementType_t;

/**
 * Conteúdo do "caption management data" ou do "caption statement data"
 * (ARIB STD-B24, Volume 3, 9.3.1 e 9.3.2).
 */
typedef struct CaptionManagement_t {
  uint8_t TMD;      /**< Modo de tempo, veja @ref MANAGEMENT_TMD_FREE e seguintes. */
  uint64_t TMD_otm; /**< OTM (management) ou STM (statement): 36 bits, 9 dígitos BCD (hhmmssmmm). */
  uint8_t reserved; /**< Bits reservados do primeiro byte. */

  /** @name Apenas no caption management data (do primeiro idioma)
   *  @{ */
  uint8_t num_lang;            /**< Quantidade de idiomas. */
  uint8_t lang_tag;            /**< Identificação do idioma (0 a 7). */
  uint8_t DMF;                 /**< Modo de exibição (Display Mode, 4 bits). */
  uint8_t DC;                  /**< Condição de exibição (presente quando DMF é 0x0c, 0x0d ou 0x0e). */
  char ISO_639_lang_code[3+1]; /**< Código do idioma, ISO 639-2 (ex: "por"), terminado em '\\0'. */
  uint8_t format;              /**< Formato de exibição, veja @ref CC_Format_t. */
  uint8_t TCS;                 /**< Codificação dos caracteres, veja @ref MANAGEMENT_TCS_8BIT. */
  uint8_t rollup_mode;         /**< Modo roll-up, veja @ref MANAGEMENT_ROLL_NONE. */
  /** @} */

  /**
   * Data unit escolhida no caption statement data: a primeira com o texto da
   * legenda (@ref _CAPTION_HANDLER_STATEMENT_BODY), ou a primeira de qualquer
   * tipo se não houver texto. Zerada no caption management data.
   */
  struct statement {
    uint8_t unit_sep;         /**< @c unit_separator (sempre 0x1f), ou 0 quando não há data unit. */
    uint8_t unit_param;       /**< @c data_unit_parameter: tipo da data unit. */
    uint32_t unit_size;       /**< @c data_unit_size: tamanho de @ref unit_data (24 bits). */
    const uint8_t *unit_data; /**< Dados da data unit, dentro do buffer do PES; NULL quando vazia. */
  } statement; /**< Data unit escolhida; veja a descrição da struct statement. */

  const PES_pts_t *pts; /**< Aponta para o PacketPES_t::pts; válido enquanto o PacketPES_t existir. */
} CaptionManagement_t;

/**
 * Lê o caption management data ou o caption statement data de um
 * @c data_group.
 *
 * O formato é escolhido pelo @c data_group_id: 0x00 e 0x20 são lidos como
 * management, os demais como statement. Todos os tamanhos são verificados
 * contra o @c data_group_size.
 *
 * @param caption     @c data_group carregado por caption_data_load().
 * @param management  Recebe o conteúdo. Os ponteiros apontam para dentro do
 *                    buffer do PES.
 * @return TRUE se o conteúdo foi lido; FALSE se estiver truncado ou vazio.
 */
bool caption_management_load (const CaptionData_t *caption,
                              CaptionManagement_t *management);

/**
 * Informa o tipo de conteúdo lido por caption_management_load().
 *
 * @param management  Conteúdo lido por caption_management_load().
 * @return @ref CAPTION_HANDLER_TYPE_STATEMENT_BODY quando há texto de legenda
 *         para o caption_closed_run(); @ref CAPTION_HANDLER_TYPE_MANAGEMENT
 *         para o caption management data; ou o tipo da data unit.
 */
CaptionManagementType_t caption_management_get_type (const CaptionManagement_t *management);

#endif /* CAPTION_MANAGEMENT_H_ */
