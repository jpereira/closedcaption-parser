/**
 * @file caption-closed-controls.h
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @date Thu Aug 12 13:49:21 BRT 2010
 * @brief Comandos de controle C0, C1 e CSI do texto da legenda.
 *
 * O texto da legenda mistura caracteres e comandos de controle (ARIB STD-B24,
 * Volume 1, 7.2; ABNT NBR 15606-1; ARIB TR-B14, 6.5):
 *
 * - <b>C0</b> (0x08 a 0x1f): posição do cursor (APR, APS...), limpar a tela
 *   (CS), caracteres especiais (SS3) etc.;
 * - <b>C1</b> (0x80 a 0x9f): cores (BKF ~ WHF, COL), tamanho dos caracteres
 *   (SSZ, MSZ, NSZ, SZX) etc.;
 * - <b>CSI</b> (0x9b): sequências estendidas, no formato
 *   <tt>CSI P1 ; P2 ; ... 0x20 F</tt>, onde os parâmetros são números
 *   decimais em ASCII e o byte final @c F identifica o comando (SWF, SDF,
 *   SDP...).
 *
 * Cada comando tem uma callback <tt>callback_controls_*</tt>, chamada por
 * caption_closed_controls_load(). Ela lê os parâmetros, atualiza o
 * CaptionClosed_t e avança a posição de leitura além dos parâmetros. Quando a
 * callback retorna TRUE, o hook de controles é chamado (veja
 * caption-hooking.h).
 *
 * Comandos que não devem ser usados em closed caption (apenas em
 * superimpose) retornam FALSE, mas ainda assim pulam os seus parâmetros, para
 * que não sejam tratados como texto.
 *
 * Exemplo de texto de legenda (TV Globo):
 * @verbatim
 * 00000000  0c 9b 37 20 53 9b 31 37  36 3b 39 39 20 5f 9b 36 |..7.S.176;99._.6|
 * 00000010  30 38 3b 33 39 30 20 56  9b 33 36 3b 33 36 20 57 |08;390.V.36;36.W|
 * 00000020  9b 32 20 58 9b 31 36 20  59 9b 38 20 6e 88 1c 4c |.2.X.16.Y.8.n..L|
 * 00000030  40 87 90 50 5b 4d 41 54  45 55 53 5d 20 4f 20 43 |@..P[MATEUS].O.C|
 * 00000040  41 4c 49 58 54 4f 20 56  41 49 0d 52 45 53 4f 4c |ALIXTO.VAI.RESOL|
 * 00000050  56 45 52 20 4f 20 50 52  4f 42 4c 45 4d 41 20 44 |VER.O.PROBLEMA.D|
 * 00000060  4f 20 43 41 52 52 4f 2c  0d 4f 20 4d 41 49 53 20 |O.CARRO,.O.MAIS.|
 * 00000070  52 c1 50 49 44 4f 20 50  4f 53 53 cd 56 45 4c 2e |R.PIDO.POSS.VEL.|
 *
 * 0c              CS: limpa a tela
 * 9b 37 20 53     CSI SWF(7): formato 960x540 horizontal
 * 9b ... 20 5f    CSI SDP(176;99): posição da área de exibição
 * 9b ... 20 56    CSI SDF(608;390): tamanho da área de exibição
 * 9b ... 20 57    CSI SSM(36;36): tamanho dos caracteres
 * 9b 32 20 58     CSI SHS(2): espaçamento horizontal
 * 9b 31 36 20 59  CSI SVS(16): espaçamento vertical
 * 9b 38 20 6e     CSI RCS(8): cor de raster transparente
 * 88              SSZ: caracteres pequenos
 * 1c 4c 40        APS(12, 0): cursor na linha 12, coluna 0
 * 87              WHF: texto branco
 * 90 50           COL: fundo preto
 * 0d              APR: nova linha
 * c1, cd          'Á' e 'Í' em ISO-8859-15
 * @endverbatim
 */
#ifndef CAPTION_CLOSED_CONTROLS_H_
#define CAPTION_CLOSED_CONTROLS_H_

#include "caption-common.h"
#include "caption-data.h"
#include "caption-closed.h"
#include "caption-management.h"

/**
 * Inclui o campo CaptionClosedControls_t::code_name nas tabelas de comandos,
 * usado por caption_closed_controls_get_name() nas mensagens de depuração.
 */
#define DEBUG_CAPTION_CLOSED_CONTROLCODE

/** Código de um comando de controle (C0, C1 ou CSI). */
typedef uint8_t CaptionClosed_CtrlCode_t;

/**
 * Dados repassados para as callbacks dos comandos.
 */
typedef struct {
  const uint8_t *argv;     /**< Buffer do texto da legenda. */
  const uint16_t argc;     /**< Quantidade de bytes disponíveis a partir de @ref pos, incluindo o próprio comando. */
  uint16_t *pos;           /**< Posição do comando em @ref argv; a callback avança além dos parâmetros. NULL nos subcomandos do CSI. */
  CaptionClosed_t *caption;/**< Estado da legenda, atualizado pela callback. */
  void *data;              /**< Dados extras: nos subcomandos do CSI, aponta para os parâmetros (struct csi_arg_Pn). */
  CaptionClosed_CtrlCode_t current_code; /**< Código do comando; o CSI troca pelo código do subcomando. */
} CC_ControlCodes_CallbackData_t;

/**
 * Entrada das tabelas de comandos: código e callback.
 */
typedef struct {
  const CaptionClosed_CtrlCode_t code; /**< Código do comando (ABNT NBR 15608-3:2008, Tabela 2). */

  /** Callback que executa o comando, ou NULL para códigos sem uso. */
  bool (*callback)(const CC_ControlCodes_CallbackData_t *d);

#ifdef DEBUG_CAPTION_CLOSED_CONTROLCODE
  const char *code_name; /**< Nome do comando, para depuração (ex: "CTRL_C0_APS"). */
#endif /* DEBUG_CAPTION_CLOSED_CONTROLCODE */
} CaptionClosedControls_t;

#ifdef DEBUG_CAPTION_CLOSED_CONTROLCODE
/** Monta uma entrada das tabelas de comandos. */
# define CAPTION_CTRL_SET(code, callback) { code, callback, #code }
#else
# define CAPTION_CTRL_SET(code, callback) { code, callback }
#endif /* DEBUG_CAPTION_CLOSED_CONTROLCODE */

/**
 * Nome de um comando, para mensagens de depuração.
 *
 * @param code  Código C0, C1 ou do subcomando CSI.
 * @return O nome (ex: "CTRL_C0_APS"), ou "UNKNOWN".
 */
const char *
caption_closed_controls_get_name (const uint8_t code);

/**
 * Comandos C0 (ARIB TR-B14, Tabela 3.11; ARIB STD-B24, Tabela 7-14).
 *
 * Os valores sem uso são preenchidos com @c PAD, para que o código menos
 * 0x08 seja o índice direto na tabela de callbacks.
 */
enum CaptionClosed_CtrlCodes_C0 {
#define CTRL_C0_NULL 0x00 /**< NUL: ignorado. */
#define CTRL_C0_BELL 0x07 /**< BEL: ignorado em closed caption. */
  CTRL_C0_APB = 0x08, /**< Active Position Backward: volta o cursor. */
  CTRL_C0_APF = 0x09, /**< Active Position Forward: avança o cursor. */
  CTRL_C0_APD = 0x0a, /**< Active Position Down: desce o cursor. */
  CTRL_C0_APU = 0x0b, /**< Active Position Up: sobe o cursor. */
  CTRL_C0_CS  = 0x0c, /**< Clear Screen: limpa a tela. */
  CTRL_C0_APR = 0x0d, /**< Active Position Return: início da próxima linha. */
  CTRL_C0_LS1 = 0x0e, /**< Locking Shift 1. */
  CTRL_C0_LS0 = 0x0f, /**< Locking Shift 0. */

  CTRL_C0_PAD_0x01 = 0x10, /**< Sem uso (0x10). */
  CTRL_C0_PAD_0x02 = 0x11, /**< Sem uso (0x11). */
  CTRL_C0_PAD_0x03 = 0x12, /**< Sem uso (0x12). */
  CTRL_C0_PAD_0x04 = 0x13, /**< Sem uso (0x13). */
  CTRL_C0_PAD_0x05 = 0x14, /**< Sem uso (0x14). */
  CTRL_C0_PAD_0x06 = 0x15, /**< Sem uso (0x15). */
  CTRL_C0_PAPF     = 0x16, /**< Parameterized Active Position Forward: avança P1 posições. */
  CTRL_C0_PAD_0x17 = 0x17, /**< Sem uso. */

  CTRL_C0_CAN      = 0x18, /**< Cancel. */
  CTRL_C0_SS2      = 0x19, /**< Single Shift 2: o próximo caractere vem do conjunto G2. */
  CTRL_C0_PAD_0x1a = 0x1a, /**< Sem uso. */
  CTRL_C0_ESC      = 0x1b, /**< Escape: designação de conjuntos de caracteres (ISO/IEC 2022). */
  CTRL_C0_APS      = 0x1c, /**< Active Position Set: posiciona o cursor em P1 (linha) e P2 (coluna). */
  CTRL_C0_SS3      = 0x1d, /**< Single Shift 3: o próximo caractere vem do conjunto G3 (ex: ♪). */
  CTRL_C0_RS       = 0x1e, /**< Record Separator. */
  CTRL_C0_US       = 0x1f  /**< Unit Separator. */
};

/**
 * @name Callbacks dos comandos C0
 *
 * Todas recebem os dados do comando e retornam TRUE quando o comando foi
 * executado. Retornam FALSE para comandos não suportados em closed caption ou
 * com parâmetros inválidos.
 * @{
 */
bool callback_controls_c0_APB (const CC_ControlCodes_CallbackData_t *d);  /**< APB: não suportado em closed caption. @ctrlcb */
bool callback_controls_c0_APF (const CC_ControlCodes_CallbackData_t *d);  /**< APF: aceito, sem efeito no estado. @ctrlcb */
bool callback_controls_c0_APD (const CC_ControlCodes_CallbackData_t *d);  /**< APD: não suportado em closed caption. @ctrlcb */
bool callback_controls_c0_APU (const CC_ControlCodes_CallbackData_t *d);  /**< APU: não suportado em closed caption. @ctrlcb */
bool callback_controls_c0_CS (const CC_ControlCodes_CallbackData_t *d);   /**< CS: liga CaptionClosed_t::need_clear. @ctrlcb */
bool callback_controls_c0_APR (const CC_ControlCodes_CallbackData_t *d);  /**< APR: aceito; o hook de controles recebe a quebra de linha. @ctrlcb */
bool callback_controls_c0_LS1 (const CC_ControlCodes_CallbackData_t *d);  /**< LS1: não suportado em closed caption. @ctrlcb */
bool callback_controls_c0_LS0 (const CC_ControlCodes_CallbackData_t *d);  /**< LS0: não suportado em closed caption. @ctrlcb */
bool callback_controls_c0_PAPF (const CC_ControlCodes_CallbackData_t *d); /**< PAPF: não suportado; pula 1 parâmetro. @ctrlcb */
bool callback_controls_c0_CAN (const CC_ControlCodes_CallbackData_t *d);  /**< CAN: não suportado em closed caption. @ctrlcb */
bool callback_controls_c0_SS2 (const CC_ControlCodes_CallbackData_t *d);  /**< SS2: não suportado; pula 1 parâmetro. @ctrlcb */
bool callback_controls_c0_ESC (const CC_ControlCodes_CallbackData_t *d);  /**< ESC: não suportado; pula a sequência inteira. @ctrlcb */
bool callback_controls_c0_APS (const CC_ControlCodes_CallbackData_t *d);  /**< APS: atualiza CaptionClosed_t::APS (2 parâmetros). @ctrlcb */
bool callback_controls_c0_SS3 (const CC_ControlCodes_CallbackData_t *d);  /**< SS3: entrega o caractere G3 em UTF-8 ao hook de texto. @ctrlcb */
bool callback_controls_c0_RS (const CC_ControlCodes_CallbackData_t *d);   /**< RS: não suportado em closed caption. @ctrlcb */
bool callback_controls_c0_US (const CC_ControlCodes_CallbackData_t *d);   /**< US: aceito, sem efeito no estado. @ctrlcb */
/** @} */

/**
 * Comandos C1 (ARIB TR-B14, Tabela 3.11; ARIB STD-B24, Tabela 7-15).
 *
 * Os valores sem uso são preenchidos com @c PAD, para que o código menos
 * 0x80 seja o índice direto na tabela de callbacks.
 */
enum CaptionClosed_CtrlCodes_C1 {
  CTRL_C1_BKF = 0x80, /**< Black Foreground: texto preto. */
  CTRL_C1_RDF = 0x81, /**< Red Foreground: texto vermelho. */
  CTRL_C1_GRF = 0x82, /**< Green Foreground: texto verde. */
  CTRL_C1_YLF = 0x83, /**< Yellow Foreground: texto amarelo. */
  CTRL_C1_BLF = 0x84, /**< Blue Foreground: texto azul. */
  CTRL_C1_MGF = 0x85, /**< Magenta Foreground: texto magenta. */
  CTRL_C1_CNF = 0x86, /**< Cyan Foreground: texto ciano. */
  CTRL_C1_WHF = 0x87, /**< White Foreground: texto branco. */

  CTRL_C1_SSZ = 0x88, /**< Small Size: caracteres pequenos. */
  CTRL_C1_MSZ = 0x89, /**< Middle Size: caracteres médios. */
  CTRL_C1_NSZ = 0x8a, /**< Normal Size: caracteres normais. */
  CTRL_C1_SZX = 0x8b, /**< Character Size Controls: tamanho especificado por P1. */
  CTRL_C1_PAD_0x8c = 0x8c, /**< Sem uso. */
  CTRL_C1_PAD_0x8d = 0x8d, /**< Sem uso. */
  CTRL_C1_PAD_0x8e = 0x8e, /**< Sem uso. */
  CTRL_C1_PAD_0x8f = 0x8f, /**< Sem uso. */
  CTRL_C1_COL = 0x90, /**< Colour Controls: cor do texto/fundo (P1, ou 0x20 P2 para a paleta). */

  CTRL_C1_FLC = 0x91, /**< Flashing Control: pisca. */
  CTRL_C1_CDC = 0x92, /**< Conceal Display Controls. */
  CTRL_C1_POL = 0x93, /**< Pattern Polarity Controls. */
  CTRL_C1_WMM = 0x94, /**< Writing Mode Modification. */
  CTRL_C1_MAC = 0x95, /**< Macro Command. */

  CTRL_C1_PAD_0x96 = 0x96, /**< Sem uso. */
  CTRL_C1_HLC = 0x97, /**< Highlighting Character Block: contorno. */
  CTRL_C1_RPC = 0x98, /**< Repeat Character: repete o próximo caractere. */
  CTRL_C1_SPL = 0x99, /**< Stop Lining: fim do sublinhado. */
  CTRL_C1_STL = 0x9a, /**< Start Lining: início do sublinhado. */
  CTRL_C1_CSI = 0x9b, /**< Control Sequence Introducer: sequência estendida, veja @ref CaptionClosed_CtrlCodes_C1_CSI. */
  CTRL_C1_PAD_0x9c = 0x9c, /**< Sem uso. */
  CTRL_C1_TIM = 0x9d, /**< Time Controls: espera/temporização. */
  CTRL_C1_PAD_0x9e = 0x9e, /**< Sem uso. */
  CTRL_C1_PAD_0x9f = 0x9f  /**< Sem uso. */
};

/**
 * @name Callbacks dos comandos C1
 * @{
 */
bool callback_controls_c1_BKF (const CC_ControlCodes_CallbackData_t *d); /**< BKF: texto preto. @ctrlcb */
bool callback_controls_c1_RDF (const CC_ControlCodes_CallbackData_t *d); /**< RDF: texto vermelho. @ctrlcb */
bool callback_controls_c1_GRF (const CC_ControlCodes_CallbackData_t *d); /**< GRF: texto verde. @ctrlcb */
bool callback_controls_c1_YLF (const CC_ControlCodes_CallbackData_t *d); /**< YLF: texto amarelo. @ctrlcb */
bool callback_controls_c1_BLF (const CC_ControlCodes_CallbackData_t *d); /**< BLF: texto azul. @ctrlcb */
bool callback_controls_c1_MGF (const CC_ControlCodes_CallbackData_t *d); /**< MGF: texto magenta. @ctrlcb */
bool callback_controls_c1_CNF (const CC_ControlCodes_CallbackData_t *d); /**< CNF: texto ciano. @ctrlcb */
bool callback_controls_c1_WHF (const CC_ControlCodes_CallbackData_t *d); /**< WHF: texto branco. @ctrlcb */

bool callback_controls_c1_SSZ (const CC_ControlCodes_CallbackData_t *d); /**< SSZ: caracteres pequenos. @ctrlcb */
bool callback_controls_c1_MSZ (const CC_ControlCodes_CallbackData_t *d); /**< MSZ: caracteres médios. @ctrlcb */
bool callback_controls_c1_NSZ (const CC_ControlCodes_CallbackData_t *d); /**< NSZ: caracteres normais. @ctrlcb */
bool callback_controls_c1_SZX (const CC_ControlCodes_CallbackData_t *d); /**< SZX: guarda P1 em CaptionClosed_t::char_size_custom. @ctrlcb */
bool callback_controls_c1_COL (const CC_ControlCodes_CallbackData_t *d); /**< COL: cor do texto, do fundo ou de meia intensidade. @ctrlcb */

bool callback_controls_c1_FLC (const CC_ControlCodes_CallbackData_t *d); /**< FLC: não suportado; pula 1 parâmetro. @ctrlcb */
bool callback_controls_c1_CDC (const CC_ControlCodes_CallbackData_t *d); /**< CDC: não suportado; pula 1 ou 2 parâmetros. @ctrlcb */
bool callback_controls_c1_POL (const CC_ControlCodes_CallbackData_t *d); /**< POL: não suportado; pula 1 parâmetro. @ctrlcb */
bool callback_controls_c1_WMM (const CC_ControlCodes_CallbackData_t *d); /**< WMM: não suportado; pula 1 parâmetro. @ctrlcb */
bool callback_controls_c1_MAC (const CC_ControlCodes_CallbackData_t *d); /**< MAC: não suportado; pula 1 parâmetro. @ctrlcb */

bool callback_controls_c1_HLC (const CC_ControlCodes_CallbackData_t *d); /**< HLC: não suportado; pula 1 parâmetro. @ctrlcb */
bool callback_controls_c1_RPC (const CC_ControlCodes_CallbackData_t *d); /**< RPC: não suportado; pula 1 parâmetro. @ctrlcb */
bool callback_controls_c1_SPL (const CC_ControlCodes_CallbackData_t *d); /**< SPL: não suportado em closed caption. @ctrlcb */
bool callback_controls_c1_STL (const CC_ControlCodes_CallbackData_t *d); /**< STL: não suportado em closed caption. @ctrlcb */

bool callback_controls_c1_TIM (const CC_ControlCodes_CallbackData_t *d); /**< TIME: não suportado; pula 2 parâmetros. @ctrlcb */
/** @} */

/**
 * Subcomandos do CSI: o byte final @c F da sequência
 * <tt>CSI P1 ; P2 ; ... 0x20 F</tt> (ARIB STD-B24, Tabela 7-17).
 *
 * Os valores sem uso são preenchidos com @c PAD, para que o código menos
 * 0x53 seja o índice direto na tabela de callbacks.
 */
enum CaptionClosed_CtrlCodes_C1_CSI {
  CTRL_C1_CSI_SWF      = 0x53, /**< Set Writing Format: formato de exibição. */
  CTRL_C1_CSI_PAD_0x54 = 0x54, /**< Sem uso. */
  CTRL_C1_CSI_PAD_0x55 = 0x55, /**< Sem uso. */
  CTRL_C1_CSI_SDF      = 0x56, /**< Set Display Format: tamanho da área de exibição. */
  CTRL_C1_CSI_SSM      = 0x57, /**< Character Composition Dot Designation: tamanho dos caracteres. */
  CTRL_C1_CSI_SHS      = 0x58, /**< Set Horizontal Spacing: espaçamento horizontal. */
  CTRL_C1_CSI_SVS      = 0x59, /**< Set Vertical Spacing: espaçamento vertical. */
  CTRL_C1_CSI_PAD_0x5a = 0x5a, /**< Sem uso. */
  CTRL_C1_CSI_PAD_0x5b = 0x5b, /**< Sem uso. */
  CTRL_C1_CSI_PAD_0x5c = 0x5c, /**< Sem uso. */
  CTRL_C1_CSI_PAD_0x5d = 0x5d, /**< Sem uso. */
  CTRL_C1_CSI_PAD_0x5e = 0x5e, /**< Sem uso. */
  CTRL_C1_CSI_SDP      = 0x5f, /**< Set Display Position: posição da área de exibição. */

  CTRL_C1_CSI_PAD_0x60 = 0x60, /**< Sem uso. */
  CTRL_C1_CSI_PAD_0x61 = 0x61, /**< Sem uso. */
  CTRL_C1_CSI_PAD_0x62 = 0x62, /**< Sem uso. */
  CTRL_C1_CSI_ORN      = 0x63, /**< Ornament Control: contorno/sombra dos caracteres. */
  CTRL_C1_CSI_PAD_0x64 = 0x64, /**< Sem uso. */
  CTRL_C1_CSI_CFS      = 0x65, /**< Character Font Set: fonte. */
  CTRL_C1_CSI_PAD_0x66 = 0x66, /**< Sem uso. */
  CTRL_C1_CSI_PAD_0x67 = 0x67, /**< Sem uso. */
  CTRL_C1_CSI_PAD_0x68 = 0x68, /**< Sem uso. */
  CTRL_C1_CSI_PAD_0x69 = 0x69, /**< Sem uso. */
  CTRL_C1_CSI_PAD_0x6a = 0x6a, /**< Sem uso. */
  CTRL_C1_CSI_PAD_0x6b = 0x6b, /**< Sem uso. */
  CTRL_C1_CSI_PAD_0x6c = 0x6c, /**< Sem uso. */
  CTRL_C1_CSI_PAD_0x6d = 0x6d, /**< Sem uso. */
  CTRL_C1_CSI_RCS      = 0x6e  /**< Raster Colour Command: cor de raster. */
};

/**
 * @name Callbacks dos subcomandos CSI
 *
 * callback_controls_c1_CSI_load() trata o CSI (0x9b): localiza o byte final,
 * lê os parâmetros com csi_arg_Pn_load() e chama a callback do subcomando.
 * Nos subcomandos, CC_ControlCodes_CallbackData_t::data aponta para os
 * parâmetros (struct csi_arg_Pn).
 * @{
 */
bool callback_controls_c1_CSI_load (const CC_ControlCodes_CallbackData_t *d); /**< CSI: lê a sequência e despacha o subcomando. @ctrlcb */
bool callback_controls_c1_CSI_swf (const CC_ControlCodes_CallbackData_t *d);  /**< SWF: formato, caracteres por linha e linhas. @ctrlcb */
bool callback_controls_c1_CSI_ssm (const CC_ControlCodes_CallbackData_t *d);  /**< SSM: tamanho dos caracteres (P1 x P2). @ctrlcb */
bool callback_controls_c1_CSI_shs (const CC_ControlCodes_CallbackData_t *d);  /**< SHS: espaçamento horizontal (P1). @ctrlcb */
bool callback_controls_c1_CSI_svs (const CC_ControlCodes_CallbackData_t *d);  /**< SVS: espaçamento vertical (P1). @ctrlcb */
bool callback_controls_c1_CSI_orn (const CC_ControlCodes_CallbackData_t *d);  /**< ORN: não implementado. @ctrlcb */
bool callback_controls_c1_CSI_cfs (const CC_ControlCodes_CallbackData_t *d);  /**< CFS: não implementado. @ctrlcb */
bool callback_controls_c1_CSI_rcs (const CC_ControlCodes_CallbackData_t *d);  /**< RCS: cor de raster (P1, de 0 a 15). @ctrlcb */
bool callback_controls_c1_CSI_sdf (const CC_ControlCodes_CallbackData_t *d);  /**< SDF: tamanho da área de exibição (P1 x P2). @ctrlcb */
bool callback_controls_c1_CSI_sdp (const CC_ControlCodes_CallbackData_t *d);  /**< SDP: posição da área de exibição (P1, P2). @ctrlcb */
/** @} */

#define CTRL_CSI_FB            0x20  /**< Byte intermediário (0x20) que antecede o byte final do CSI. */
#define CTRL_CSI_IB            0x3b  /**< Separador de parâmetros do CSI (';'). */

#define CTRL_CSI_MAX_Pn_ARGS   5     /**< Máximo de parâmetros lidos de um CSI. */
#define CTRL_CSI_MAX_Pn_FIELDS (5+1) /**< Máximo de dígitos de um parâmetro, mais o '\\0' (até "65535"). */

/**
 * Parâmetros de uma sequência CSI, já convertidos para números.
 *
 * O parâmetro P1 da norma está em @c argv[0], o P2 em @c argv[1], e assim
 * por diante. Parâmetros não informados ficam zerados.
 */
struct csi_arg_Pn {
  uint16_t argv[CTRL_CSI_MAX_Pn_ARGS]; /**< Valores dos parâmetros (até 65535). */
  uint8_t argc;                        /**< Quantidade de parâmetros lidos. */
};

/**
 * Lê os parâmetros decimais de uma sequência CSI, separados por ';'.
 *
 * @param d  @c d->argv aponta para o primeiro parâmetro, @c d->argc é a
 *           quantidade de bytes de parâmetros e @c d->data aponta para a
 *           struct csi_arg_Pn que recebe os valores.
 * @return TRUE se ao menos um parâmetro foi lido.
 */
bool csi_arg_Pn_load (CC_ControlCodes_CallbackData_t *d);

/**
 * Início de cada tabela de comandos: o código menos este valor é o índice na
 * tabela de callbacks.
 */
enum CC_ControlCodes_TableJump_e {
  CC_CTRLCODE_SET_UNKNOWN = 0x00,        /**< Não é um comando. */
  CC_CTRLCODE_SET_C0      = CTRL_C0_APB, /**< Tabela C0 (0x08 a 0x1f). */
  CC_CTRLCODE_SET_C1      = CTRL_C1_BKF  /**< Tabela C1 (0x80 a 0x9f). */
};

/**
 * Verifica se um byte é um comando C0 ou C1.
 *
 * @param byte   Byte do texto da legenda.
 * @param table  Recebe a tabela do comando, ou @ref CC_CTRLCODE_SET_UNKNOWN.
 * @return TRUE se o byte é um comando.
 */
bool caption_closed_controls_is_table (const uint8_t *byte,
                                       enum CC_ControlCodes_TableJump_e *table);

/**
 * Executa o comando C0/C1 em @c argv[*pos].
 *
 * Chama a callback do comando, que avança @p pos além dos parâmetros, e, se
 * ela retornar TRUE, chama o hook de controles.
 *
 * @param table    Tabela do comando, obtida por caption_closed_controls_is_table().
 * @param argv     Buffer do texto da legenda.
 * @param argc     Quantidade de bytes disponíveis a partir de @p pos.
 * @param pos      Posição do comando em @p argv; ao retornar, aponta para o
 *                 último byte consumido pelo comando.
 * @param caption  Estado da legenda.
 * @return TRUE se o comando foi executado.
 */
bool caption_closed_controls_load (enum CC_ControlCodes_TableJump_e table,
                                   const uint8_t *argv,
                                   const uint16_t argc,
                                   uint16_t *pos,
                                   CaptionClosed_t *caption);

/**
 * Cores da paleta CMLA (Colour Map Lower Address), usada pelos comandos de
 * cor (BKF ~ WHF e COL).
 */
typedef enum {
  SET_CMLA0_BLACK,                  /**< Preto. */
  SET_CMLA1_FULL_INTENSITY_RED,     /**< Vermelho. */
  SET_CMLA2_FULL_INTENSITY_GREEN,   /**< Verde. */
  SET_CMLA3_FULL_INTENSITY_YELLOW,  /**< Amarelo. */
  SET_CMLA4_FULL_INTENSITY_BLUE,    /**< Azul. */
  SET_CMLA5_FULL_INTENSITY_MAGNETA, /**< Magenta. */
  SET_CMLA6_FULL_INTENSITY_CYAN,    /**< Ciano. */
  SET_CMLA7_FULL_INTENSITY_WHITE,   /**< Branco. */
  SET_CMLA8_TRANSPARENT,            /**< Transparente. */
  SET_CMLA9_HALF_INTENSITY_RED,     /**< Vermelho, meia intensidade. */
  SET_CMLA10_HALF_INTENSITY_GREEN,  /**< Verde, meia intensidade. */
  SET_CMLA11_HALF_INTENSITY_YELLOW, /**< Amarelo, meia intensidade. */
  SET_CMLA12_HALF_INTENSITY_BLUE,   /**< Azul, meia intensidade. */
  SET_CMLA13_HALF_INTENSITY_MAGNETA,/**< Magenta, meia intensidade. */
  SET_CMLA14_HALF_INTENSITY_CYAN,   /**< Ciano, meia intensidade. */
  SET_CMLA15_HALF_INTENSITY_WHITE,  /**< Branco, meia intensidade. */
  SET_CMLA_t_eMax                   /**< Quantidade de cores. */
} CC_Color_CMLA_t;

/**
 * Converte uma cor da paleta CMLA para RGBA.
 *
 * @param index  Cor da paleta, de 0 a 15.
 * @param color  Recebe a cor em RGBA.
 * @return FALSE se @p index estiver fora da paleta.
 */
static inline bool caption_closed_controls_cmlat2rgb (CC_Color_CMLA_t index, CC_Color_t *color)
{
  static const CC_Color_t palete_index[] = {
  /* <R>   <G>   <B>    <A> */
   { 0x00, 0x00, 0x00, 0xFF }, // SET_CMLA0_BLACK
   { 0xFE, 0x00, 0x00, 0xFF }, // SET_CMLA1_FULL_INTENSITY_RED
   { 0x00, 0xFE, 0x00, 0xFF }, // SET_CMLA2_FULL_INTENSITY_GREEN
   { 0xFE, 0xFE, 0x00, 0xFF }, // SET_CMLA3_FULL_INTENSITY_YELLOW
   { 0x00, 0x00, 0xFE, 0xFF }, // SET_CMLA4_FULL_INTENSITY_BLUE
   { 0xFE, 0x00, 0xFE, 0xFF }, // SET_CMLA5_FULL_INTENSITY_MAGNETA
   { 0x00, 0xFE, 0xFE, 0xFF }, // SET_CMLA6_FULL_INTENSITY_CYAN
   { 0xFE, 0xFE, 0xFE, 0xFF }, // SET_CMLA7_FULL_INTENSITY_WHITE
   { 0x00, 0x00, 0x00, 0x00 }, // SET_CMLA8_TRANSPARENT
   { 0x79, 0x00, 0x00, 0xFF }, // SET_CMLA9_HALF_INTENSITY_RED
   { 0x00, 0x7D, 0x00, 0xFF }, // SET_CMLA10_HALF_INTENSITY_GREEN
   { 0x79, 0x79, 0x00, 0xFF }, // SET_CMLA11_HALF_INTENSITY_YELLOW
   { 0x00, 0x00, 0x79, 0xFF }, // SET_CMLA12_HALF_INTENSITY_BLUE
   { 0x79, 0x00, 0x79, 0xFF }, // SET_CMLA13_HALF_INTENSITY_MAGNETA
   { 0x00, 0x79, 0x79, 0xFF }, // SET_CMLA14_HALF_INTENSITY_CYAN
   { 0x79, 0x79, 0x79, 0xFF }  // SET_CMLA15_HALF_INTENSITY_WHITE
  };

  assert (color != NULL);

  if ((unsigned)index >= ARRAY_SIZE(palete_index))
  {
    ATV_DEBUG("Índice de cor (%d) fora da paleta.", index);
    return FALSE;
  }

  *color = palete_index[index];

  return TRUE;
}

#endif /* CAPTION_CLOSED_CONTROLS_H_ */
