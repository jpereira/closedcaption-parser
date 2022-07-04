/**
 * @file: caption-closed-controls.h
 * @autor: Jorge Pereira <jorge.pereira@tqtvd.com>
 * @date: Thu Aug 12 13:49:21 BRT 2010
 * @desc: Mais informações em 'arquitetura-closedcaption.txt'
 */
#ifndef CAPTION_CLOSED_CONTROLS_H_
#define CAPTION_CLOSED_CONTROLS_H_

#include "caption-common.h"
#include "caption-data.h"
#include "caption-closed.h"
#include "caption-management.h"

/**
 * Uma vez que voce ativar a flag abaixo, sera gerado a estrutura de dados 'CaptionClosedControls_t'
 * contendo o campo 'code_name', e assim sera exibido mensagens referente aos codigos cadastrados
 * em 'enum ControlCodeCmd_t'. tendo como ideia facilitar a depuracao e identificacao dos 'codes'
 * que estao sendo 'parseados'.
 *
 * Atc, Jorge Pereira
 * Date: Thu Oct 14 20:01:52 BRT 2010
 */
#define DEBUG_CAPTION_CLOSED_CONTROLCODE

typedef uint8_t CaptionClosed_CtrlCode_t;

typedef struct {
  const uint8_t *argv; /* Buffer a ser tratado */
  const uint8_t argc; /* Posicao maxima para argv[] */
  uint8_t *pos; /* Posicao atual em 'argv[]' */
  CaptionClosed_t *caption; /* Objeto de alto nivel. */
  void *data; /* Utilizado para qualquer coisa. */
  CaptionClosed_CtrlCode_t current_code; /* Codigo corrent referente ao callback invocado relacionado a C0/C1 ou C1_CSI */
} CC_ControlCodes_CallbackData_t;

typedef struct {
  /**
   * Mumero do codigo conforme tabela da norma ABNT 15608-3:2008, Pag 4
   */
  const CaptionClosed_CtrlCode_t code; /* Codigo supostamente em enum CaptionClosed_CtrlCodes_C0/1 */

  /**
   * Abaixo, este sera o cara q vai ser chamado para processar o tal 'code'
   */
  bool (*callback)(const CC_ControlCodes_CallbackData_t *d);
   
  /**
   * Este esquema e apenas para incluir o campo "code_name" facilitando depuração,
   * ao menos para identificar facilmente nos logs! \o/
   */
#ifdef DEBUG_CAPTION_CLOSED_CONTROLCODE
  const char *code_name;
#endif /* DEBUG_CAPTION_CLOSED_CONTROLCODE */
} CaptionClosedControls_t;
#ifdef DEBUG_CAPTION_CLOSED_CONTROLCODE
# define CAPTION_CTRL_SET(code, callback) { code, callback, #code }
#else
# define CAPTION_CTRL_SET(code, callback) { code, callback }
#endif /* DEBUG_CAPTION_CLOSED_CONTROLCODE */

/**
 * Este metodo so tera efeito quando compilado utilizando flag de depuracao
 * deste componente.
 */
const char *
caption_closed_controls_get_name (const uint8_t code);

/**
 * Esse sao os caracteres de nivel 0/1, conforme descrito na norma
 * ARIB_TR-B14-1_2of2_v2.8_E2.pdf na sessao 4.5.
 * ARIB STD-B37, 3.2.3.1 Format Conversion
 * ARIB_TR-B14-1_2of2_v2.8_E2.pdf: Tabela 3.11
 *
 * Obs, Perceba que preferi montar duas tabelas fazendo 'padding'
 * com comandos não cadastrados, formando uma dword de 32bits.
 * Por Jorge Pereira
 */
 
/**
 * Comandos e callbacks relacionado ao controle C0
 */
enum CaptionClosed_CtrlCodes_C0 { // 16 bytes, contendo os caracteres C1 conforme norma.
#define CTRL_C0_NULL 0x00
#define CTRL_C0_BELL 0x07
  CTRL_C0_APB = 0x08,
  CTRL_C0_APF = 0x09,
  CTRL_C0_APD = 0x0a,
  CTRL_C0_APU = 0x0b,
  CTRL_C0_CS  = 0x0c,
  CTRL_C0_APR = 0x0d,
  CTRL_C0_LS1 = 0x0e,
  CTRL_C0_LS0 = 0x0f,
 
  // Padding
  CTRL_C0_PAD_0x01 = 0x10, // <--+
  CTRL_C0_PAD_0x02 = 0x11, //    |
  CTRL_C0_PAD_0x03 = 0x12, //    +--> Padding
  CTRL_C0_PAD_0x04 = 0x13, //    |
  CTRL_C0_PAD_0x05 = 0x14, //    |
  CTRL_C0_PAD_0x06 = 0x15, // <--+ 
  CTRL_C0_PAPF     = 0x16,
  CTRL_C0_PAD_0x17 = 0x17, // Padding

  CTRL_C0_CAN      = 0x18,
  CTRL_C0_SS2      = 0x19,
  CTRL_C0_PAD_0x1a = 0x1a, // Padding
  CTRL_C0_ESC      = 0x1b,
  CTRL_C0_APS      = 0x1c,
  CTRL_C0_SS3      = 0x1d,
  CTRL_C0_RS       = 0x1e,
  CTRL_C0_US       = 0x1f
};

bool callback_controls_c0_APB (const CC_ControlCodes_CallbackData_t *d);
bool callback_controls_c0_APF (const CC_ControlCodes_CallbackData_t *d);
bool callback_controls_c0_APD (const CC_ControlCodes_CallbackData_t *d);
bool callback_controls_c0_APU (const CC_ControlCodes_CallbackData_t *d);
bool callback_controls_c0_CS (const CC_ControlCodes_CallbackData_t *d);
bool callback_controls_c0_APR (const CC_ControlCodes_CallbackData_t *d);
bool callback_controls_c0_LS1 (const CC_ControlCodes_CallbackData_t *d);
bool callback_controls_c0_LS0 (const CC_ControlCodes_CallbackData_t *d);
bool callback_controls_c0_PAPF (const CC_ControlCodes_CallbackData_t *d);
bool callback_controls_c0_CAN (const CC_ControlCodes_CallbackData_t *d);
bool callback_controls_c0_SS2 (const CC_ControlCodes_CallbackData_t *d);
bool callback_controls_c0_ESC (const CC_ControlCodes_CallbackData_t *d);
bool callback_controls_c0_APS (const CC_ControlCodes_CallbackData_t *d);
bool callback_controls_c0_SS3 (const CC_ControlCodes_CallbackData_t *d);
bool callback_controls_c0_RS (const CC_ControlCodes_CallbackData_t *d);
bool callback_controls_c0_US (const CC_ControlCodes_CallbackData_t *d);

/**
 * Comandos e callbacks relacionado ao controle C1
 */
enum CaptionClosed_CtrlCodes_C1 { // 32 bytes, contendo os caracteres C1 conforme norma.
  // Foreground Colors Command
  CTRL_C1_BKF = 0x80,
  CTRL_C1_RDF = 0x81,
  CTRL_C1_GRF = 0x82,
  CTRL_C1_YLF = 0x83,
  CTRL_C1_BLF = 0x84,
  CTRL_C1_MGF = 0x85,
  CTRL_C1_CNF = 0x86,
  CTRL_C1_WHF = 0x87,
  
  // Controles dos Caracteres
  CTRL_C1_SSZ = 0x88,
  CTRL_C1_MSZ = 0x89,
  CTRL_C1_NSZ = 0x8a,
  CTRL_C1_SZX = 0x8b,  
  CTRL_C1_PAD_0x8c = 0x8c, // ]
  CTRL_C1_PAD_0x8d = 0x8d, // | Padding: Apenas para alinhamento da estrutura. 
  CTRL_C1_PAD_0x8e = 0x8e, // |
  CTRL_C1_PAD_0x8f = 0x8f, // ]
  CTRL_C1_COL = 0x90,
  
  CTRL_C1_FLC = 0x91,
  CTRL_C1_CDC = 0x92,
  CTRL_C1_POL = 0x93,
  CTRL_C1_WMM = 0x94,
  CTRL_C1_MAC = 0x95,
  
  CTRL_C1_PAD_0x96 = 0x96, // Padding
  CTRL_C1_HLC = 0x97,
  CTRL_C1_RPC = 0x98,
  CTRL_C1_SPL = 0x99,
  CTRL_C1_STL = 0x9a,
  CTRL_C1_CSI = 0x9b, // Control Extension Code (subset)
  CTRL_C1_PAD_0x9c = 0x9c, // Padding
  CTRL_C1_TIM = 0x9d, // Time
  CTRL_C1_PAD_0x9d = 0x9e, // Padding
  CTRL_C1_PAD_0x9f = 0x9f  // Padding
};

// Foreground Colors Command
bool callback_controls_c1_BKF (const CC_ControlCodes_CallbackData_t *d);
bool callback_controls_c1_RDF (const CC_ControlCodes_CallbackData_t *d);
bool callback_controls_c1_GRF (const CC_ControlCodes_CallbackData_t *d);
bool callback_controls_c1_YLF (const CC_ControlCodes_CallbackData_t *d);
bool callback_controls_c1_BLF (const CC_ControlCodes_CallbackData_t *d);
bool callback_controls_c1_MGF (const CC_ControlCodes_CallbackData_t *d);
bool callback_controls_c1_CNF (const CC_ControlCodes_CallbackData_t *d);
bool callback_controls_c1_WHF (const CC_ControlCodes_CallbackData_t *d);

// Controles dos Caracteres
bool callback_controls_c1_SSZ (const CC_ControlCodes_CallbackData_t *d);
bool callback_controls_c1_MSZ (const CC_ControlCodes_CallbackData_t *d);
bool callback_controls_c1_NSZ (const CC_ControlCodes_CallbackData_t *d);
bool callback_controls_c1_SZX (const CC_ControlCodes_CallbackData_t *d);
bool callback_controls_c1_COL (const CC_ControlCodes_CallbackData_t *d);

bool callback_controls_c1_FLC (const CC_ControlCodes_CallbackData_t *d);
bool callback_controls_c1_CDC (const CC_ControlCodes_CallbackData_t *d);
bool callback_controls_c1_POL (const CC_ControlCodes_CallbackData_t *d);
bool callback_controls_c1_WMM (const CC_ControlCodes_CallbackData_t *d);
bool callback_controls_c1_MAC (const CC_ControlCodes_CallbackData_t *d);

bool callback_controls_c1_HLC (const CC_ControlCodes_CallbackData_t *d);
bool callback_controls_c1_RPC (const CC_ControlCodes_CallbackData_t *d);
bool callback_controls_c1_SPL (const CC_ControlCodes_CallbackData_t *d);
bool callback_controls_c1_STL (const CC_ControlCodes_CallbackData_t *d);

bool callback_controls_c1_TIM (const CC_ControlCodes_CallbackData_t *d);

/**
 * Conforme descrito na tabela 3.4.1.2, apenas os comandos CSI.
 *
 * ARIB STD-B24: V5.0E-1, Table 7-17
 */
enum CaptionClosed_CtrlCodes_C1_CSI {
  CTRL_C1_CSI_SWF      = 0x53, // Set Writing Format
  CTRL_C1_CSI_PAD_0x54 = 0x54, // Padding
  CTRL_C1_CSI_PAD_0x55 = 0x55, // Padding
  CTRL_C1_CSI_SDF      = 0x56, // Set Display Format
  CTRL_C1_CSI_SSM      = 0x57, // Character Composition dot Designation
  CTRL_C1_CSI_SHS      = 0x58, // Set Horizontal Spacing
  CTRL_C1_CSI_SVS      = 0x59, // Set Vertical Spacing
  CTRL_C1_CSI_PAD_0x5a = 0x5a, // Padding
  CTRL_C1_CSI_PAD_0x5b = 0x5b, // Padding
  CTRL_C1_CSI_PAD_0x5c = 0x5c, // Padding
  CTRL_C1_CSI_PAD_0x5d = 0x5d, // Padding
  CTRL_C1_CSI_PAD_0x5e = 0x5e, // Padding
  CTRL_C1_CSI_SDP      = 0x5f, // Set Display Position

  CTRL_C1_CSI_PAD_0x60 = 0x60, // Padding
  CTRL_C1_CSI_PAD_0x61 = 0x61, // Padding
  CTRL_C1_CSI_PAD_0x62 = 0x62, // Padding
  CTRL_C1_CSI_ORN      = 0x63, // Omament Control
  CTRL_C1_CSI_PAD_0x64 = 0x64, // Padding
  CTRL_C1_CSI_CFS      = 0x65, // Character Font Setup
  CTRL_C1_CSI_PAD_0x66 = 0x66, // Padding
  CTRL_C1_CSI_PAD_0x67 = 0x67, // Padding
  CTRL_C1_CSI_PAD_0x68 = 0x68, // Padding
  CTRL_C1_CSI_PAD_0x69 = 0x69, // Padding
  CTRL_C1_CSI_PAD_0x6a = 0x6a, // Padding
  CTRL_C1_CSI_PAD_0x6b = 0x6b, // Padding
  CTRL_C1_CSI_PAD_0x6c = 0x6c, // Padding
  CTRL_C1_CSI_PAD_0x6d = 0x6d, // Padding
  CTRL_C1_CSI_RCS      = 0x6e  // Raster Color Command
};

// handler: CSI(0x9b): Control Extension Code
bool callback_controls_c1_CSI_load (const CC_ControlCodes_CallbackData_t *d);
bool callback_controls_c1_CSI_swf (const CC_ControlCodes_CallbackData_t *d);
bool callback_controls_c1_CSI_ssm (const CC_ControlCodes_CallbackData_t *d);
bool callback_controls_c1_CSI_shs (const CC_ControlCodes_CallbackData_t *d);
bool callback_controls_c1_CSI_svs (const CC_ControlCodes_CallbackData_t *d);
bool callback_controls_c1_CSI_orn (const CC_ControlCodes_CallbackData_t *d);
bool callback_controls_c1_CSI_cfs (const CC_ControlCodes_CallbackData_t *d);
bool callback_controls_c1_CSI_rcs (const CC_ControlCodes_CallbackData_t *d);
bool callback_controls_c1_CSI_sdf (const CC_ControlCodes_CallbackData_t *d);
bool callback_controls_c1_CSI_sdp (const CC_ControlCodes_CallbackData_t *d);

/**
 * handler: CSI(0x90): Control Sequence Format
 * Norma: ARIB STD-B24 Version 5.0-E1
 * Pag, 96: Table 7-17 Extension control code (CSI)
 *
 * <brief>
 *
 * Control sequences make up the bulk of the ANSI-BBS spec. And contain four distinct parts which
 * are, in order:
 *
 * Control Sequence Introducer (CSI)
 *
 * The two-character CSI consists of the ESC character (0x1b) followed by the '[' character (0x5b)
 * Parameter Bytes (PBs): Parameter bytes consist of zero of more of the characters
 * from '0' (0x30) to '?' (0x3f)
 * Intermediate Bytes (IBs): Intermediate bytes consist of zero or more of the charaters
 * from ' ' (0x20) to '/' (0x2f)
 * 
 * Final Byte (FB): The final byte consists of one of the characters from '@' (0x40) to '~' (0x7e)
 *
 * </brief fonte: http://www.ansi-bbs.org/ansi-bbs2/control_seqs/>
 *
 * Exemplo de Payload utilizado:
 * <payload>
 * 00000000  0c 9b 37 20 53 9b 31 37  36 3b 39 39 20 5f 9b 36 |..7.S.176;99._.6|
 * 00000010  30 38 3b 33 39 30 20 56  9b 33 36 3b 33 36 20 57 |08;390.V.36;36.W|
 * 00000020  9b 32 20 58 9b 31 36 20  59 9b 38 20 6e 88 1c 4c |.2.X.16.Y.8.n..L|
 * 00000030  40 87 90 50
 *                       5b 4d 41 54  45 55 53 5d 20 4f 20 43 |@..P[MATEUS].O.C|
 * 00000040  41 4c 49 58 54 4f 20 56  41 49 0d 52 45 53 4f 4c |ALIXTO.VAI.RESOL|
 * 00000050  56 45 52 20 4f 20 50 52  4f 42 4c 45 4d 41 20 44 |VER.O.PROBLEMA.D|
 * 00000060  4f 20 43 41 52 52 4f 2c  0d 4f 20 4d 41 49 53 20 |O.CARRO,.O.MAIS.|
 * 00000070  52 c1 50 49 44 4f 20 50  4f 53 53 cd 56 45 4c 2e |R.PIDO.POSS.VEL.|
 * 00000080 
 * </payload>
 */
#define CTRL_CSI_FB            0x20
#define CTRL_CSI_IB            0x3b

#define CTRL_CSI_MAX_Pn_ARGS   5
#define CTRL_CSI_MAX_Pn_FIELDS 5+1 // (2 bytes) { '6', '5', '5', '3', '5', '\0' }

/**
 * Estrutura utilizada para facilitar o acesso aos parametros descritos na especificação. 
 *
 * Exemplos:
 *  P1 ~ Pi
 *  P2 ~ Pi
 *  ...
 *  P5 ~ Pi
 *
 * Lembretes:
 *
 *  1) Entre P1[0] e P1[i] o maximo e de CTRL_CSI_MAX_Pn_FIELDS ([0] ... [65535])
 *  2) Entre P1 e Pi o maximo e de CTRL_CSI_MAX_Pn_ARGS
 */
struct csi_arg_Pn { /* Estrutura utilizada para facilitar o acesso aos */
  uint16_t argv[CTRL_CSI_MAX_Pn_ARGS]; /* [max] = [0] ... [65535] */
  uint8_t argc; 
};

/**
 * Fazendo analogia a norma, lembre-se de acessar o Pn->argv[n]
 * tendo sempre o valor de 'n' - 1, ou seja. Quando você quiser o P2
 * voce ira acessar argv[1].
 */
bool csi_arg_Pn_load (CC_ControlCodes_CallbackData_t *d);

/**
 * Caso tenha percebido eu fiz esta otimizacao baseada na tabela de
 * caracteres fazendo um 'jump' para cada 'char set' entre 16 e 32 bytes.
 */
enum CC_ControlCodes_TableJump_e {
  CC_CTRLCODE_SET_UNKNOWN = 0x00,
  CC_CTRLCODE_SET_C0      = CTRL_C0_APB, // Inicio do C0
  CC_CTRLCODE_SET_C1      = CTRL_C1_BKF  // Inicio do C1
};

/**
 * Verifica se e um 'code' e um C0 ou C1.
 */
bool caption_closed_controls_is_table (const uint8_t *byte,
                                       enum CC_ControlCodes_TableJump_e *table);

/**
 * Faz o handler entre o 'code' e o callback especifico para processar tais
 * informacoes recebidas.
 */
bool caption_closed_controls_load (enum CC_ControlCodes_TableJump_e table,
                                   const uint8_t *argv /* buffer*/,
                                   const uint8_t argc /* total */,
                                   uint8_t *pos /* Posicao atual em argv[]*/,
                                   CaptionClosed_t *caption /* Obj caption */);

/**
 * Converte o indice recebido para RGB conforme tabela descrita.
 */
typedef enum {
  SET_CMLA0_BLACK,  
  SET_CMLA1_FULL_INTENSITY_RED,
  SET_CMLA2_FULL_INTENSITY_GREEN,
  SET_CMLA3_FULL_INTENSITY_YELLOW,
  SET_CMLA4_FULL_INTENSITY_BLUE,
  SET_CMLA5_FULL_INTENSITY_MAGNETA,
  SET_CMLA6_FULL_INTENSITY_CYAN,
  SET_CMLA7_FULL_INTENSITY_WHITE,  
  SET_CMLA8_TRANSPARENT,  
  SET_CMLA9_HALF_INTENSITY_RED,
  SET_CMLA10_HALF_INTENSITY_GREEN,
  SET_CMLA11_HALF_INTENSITY_YELLOW,
  SET_CMLA12_HALF_INTENSITY_BLUE,
  SET_CMLA13_HALF_INTENSITY_MAGNETA,
  SET_CMLA14_HALF_INTENSITY_CYAN,
  SET_CMLA15_HALF_INTENSITY_WHITE,
  SET_CMLA_t_eMax
} CC_Color_CMLA_t;

static inline bool caption_closed_controls_cmlat2rgb (CC_Color_CMLA_t index, CC_Color_t *color)
{
  static CC_Color_t palete_index[] = {
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
   { 0xFE, 0xFE, 0x79, 0xFF }, // SET_CMLA11_HALF_INTENSITY_YELLOW
   { 0x00, 0x00, 0x79, 0xFF }, // SET_CMLA12_HALF_INTENSITY_BLUE
   { 0x79, 0x00, 0x79, 0xFF }, // SET_CMLA13_HALF_INTENSITY_MAGNETA
   { 0x00, 0xFE, 0xFE, 0xFF }, // SET_CMLA14_HALF_INTENSITY_CYAN
   { 0x00, 0x00, 0x00, 0xFF }  // SET_CMLA15_HALF_INTENSITY_WHITE
  };

  assert (color != NULL);

  if (index > ARRAY_SIZE(palete_index))
  {
    ATV_DEBUG("Oops! Indice '%d' invalido em relacao a palete_index[]", index);
    return FALSE;
  }

  *color = palete_index[index];

  return TRUE;
}

#endif /*CAPTION_CLOSED_CONTROLS_H_*/
