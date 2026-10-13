/**
 * @file: caption-closed-controls.c
 * @autor: Jorge Pereira <jpereiran@gmail.com>
 * @date: Thu Aug 12 13:49:21 BRT 2010
 * @desc: Mais informações em 'arquitetura-closedcaption.txt'
 */
#include "caption-closed-controls.h"
#include "caption-hooking-priv.h"
#include "caption-common.h"

/**
 * Antes de mexer, lembre-se que e preciso que esteja na ordem conforme a norma
 * ABNT 15608-3:2008, pagina 4, tabela 2 (conjunto de caracteres)
 *
 * Arquivo: 8-TR-B14v2_8-1p3-2-E2.pdf
 *
 * <brief>
 * 4.5.7.2 Control codes
 * 
 * The following control codes are not used from the beginning of instruction of scrolling and
 * the end of scrolling.
 * 
 * Format specification:      SWF, SDF, SDP
 * Size specification:        SZX, SSM
 * Change of active position: APB, APR, APF, PAPF, APU, APD, APS, ACPS, SHS, SVS
 * Display effect:            FLC, STL, SPL, HLC
 * Time control:              TIME
 * Raster specification:      RCS APB, APR, APF, PAPF, APU, APD, APS, ACPS, FLC, STL, SPL, HLC and RCS
 *
 *  specified before the scrolling instruction will be released at the start of the
 * scrolling instruction.
 *
 * </brief>
 */

/**
 * Conforme descrito na norma ARIB_TR_B14-1_2of2_v2.8_E2.pdf na pagina 3.94 tabela 3.11.
 * segue todos os callbacks a serem utilizados conforme tais comandos descritos.
 *
 * Ate entao a unica forma de testar foi atraves de alguns 'dumps' feitos do sinal da
 * TV Globo, automaticamente apenas alguns comandos foram realmente testados.
 *
 * Atc, Jorge Pereira <jpereiran@gmail.com>
 * Date: Fri Oct 15 18:06:58 BRT 2010
 */

/**
 * Callbacks referente aos C0, seguindo as restrições referente ao "Closed Caption"
 * em que alguns comandos devem ser ignorados. 
 *
 * <norma ARIB TR-B14 Version 2.8-E2>
 *  6.5: Control code used in closed caption
 *    6.5.1: Control code
 *    <brief>
 *       The control code used in the closed caption is compliant with ARIB STD-B24, Vol. 1,
 *     Section 2.7.1.2. However, the operation limitations in Table 6-9, Table 6-10, Table 6-11
 *     are established.
 *    </brief>
 * </norma ARIB TR-B14 Version 2.8-E2>
 */
static
const CaptionClosedControls_t callbacks_ctrls_c0[] = {
  // BELL: Ignorado quando closedcaption.
  CAPTION_CTRL_SET(CTRL_C0_APB, callback_controls_c0_APB),
  CAPTION_CTRL_SET(CTRL_C0_APF, callback_controls_c0_APF),
  CAPTION_CTRL_SET(CTRL_C0_APD, callback_controls_c0_APD),
  CAPTION_CTRL_SET(CTRL_C0_APU, callback_controls_c0_APU),
  CAPTION_CTRL_SET(CTRL_C0_CS, callback_controls_c0_CS),
  CAPTION_CTRL_SET(CTRL_C0_APR, callback_controls_c0_APR),
  CAPTION_CTRL_SET(CTRL_C0_LS1, callback_controls_c0_LS1),
  CAPTION_CTRL_SET(CTRL_C0_LS0, callback_controls_c0_LS0),

  CAPTION_CTRL_SET(CTRL_C0_PAD_0x01, NULL), // <--+
  CAPTION_CTRL_SET(CTRL_C0_PAD_0x02, NULL), //    |
  CAPTION_CTRL_SET(CTRL_C0_PAD_0x03, NULL), //    +--> // Padding
  CAPTION_CTRL_SET(CTRL_C0_PAD_0x04, NULL), //    |
  CAPTION_CTRL_SET(CTRL_C0_PAD_0x05, NULL), //    |
  CAPTION_CTRL_SET(CTRL_C0_PAD_0x06, NULL), // <--+
  CAPTION_CTRL_SET(CTRL_C0_PAPF, callback_controls_c0_PAPF),
  CAPTION_CTRL_SET(CTRL_C0_PAD_0x17, NULL), // Padding

  CAPTION_CTRL_SET(CTRL_C0_CAN, callback_controls_c0_CAN),
  CAPTION_CTRL_SET(CTRL_C0_SS2, callback_controls_c0_SS2),
  CAPTION_CTRL_SET(CTRL_C0_PAD_0x1a, NULL), // Padding
  CAPTION_CTRL_SET(CTRL_C0_ESC, callback_controls_c0_ESC),
  CAPTION_CTRL_SET(CTRL_C0_APS, callback_controls_c0_APS),
  CAPTION_CTRL_SET(CTRL_C0_SS3, callback_controls_c0_SS3),
  CAPTION_CTRL_SET(CTRL_C0_RS, callback_controls_c0_RS),
  CAPTION_CTRL_SET(CTRL_C0_US, callback_controls_c0_US),
};
 
/**
 * Retorna TRUE se o comando atual tem pelo menos 'n' bytes de parametros no buffer.
 */
#define CTRL_HAS_PARAMS(d, n) ((uint32_t)(n) < (uint32_t)(d)->argc)

/**
 * Pula os 'n' bytes de parametros do comando atual, evitando que sejam tratados
 * como texto quando o comando nao e suportado. Nunca passa do final do buffer.
 *
 * Retorna FALSE, para ser utilizado diretamente como retorno das callbacks.
 */
static bool
ctrl_skip_params (const CC_ControlCodes_CallbackData_t *d,
                  uint16_t n)
{
  *d->pos += CTRL_HAS_PARAMS (d, n) ? n : (d->argc - 1);

  return FALSE;
}

/**
 * Callbacks relacionado aos determinados 'comandos' de controle C0
 */
 
// handler: APB(0x08): Active Position Backwards
bool callback_controls_c0_APB (const CC_ControlCodes_CallbackData_t UNUSED(*d))
{
  return FALSE; // Não deve ser utilizado com "ClosedCaption", apenas "SuperImpose".
}

// handler: APF(0x09): Active Position Forward
// syntax: [apf]
bool
callback_controls_c0_APF (const CC_ControlCodes_CallbackData_t UNUSED(*d))
{
  // TODO:
  ATV_DEBUG("Implementar Isto!");
  return TRUE; //FALSE; // Não deve ser utilizado com "ClosedCaption", apenas "SuperImpose".
}

// handler: APD(0x0a): Active Position Down
bool callback_controls_c0_APD (const CC_ControlCodes_CallbackData_t UNUSED(*d))
{
  return FALSE; // Não deve ser utilizado com "ClosedCaption", apenas "SuperImpose".
}

// handler: APU(0x0b): Active Position Up
bool callback_controls_c0_APU (const CC_ControlCodes_CallbackData_t UNUSED(*d))
{
  return FALSE; // Não deve ser utilizado com "ClosedCaption", apenas "SuperImpose".
}

// handler: CS(0x0c): Clear Screen
// syntax: [cs]
bool
callback_controls_c0_CS (const CC_ControlCodes_CallbackData_t *d)
{
  CaptionClosed_t *caption = d->caption;

  assert (caption != NULL);

  // Setando variavel do caption informando que precisa limpar a tela.
  caption->need_clear = TRUE;

  return TRUE; 
}

// handler: APR(0x0d): Active Position Return
// syntax: [apr]
bool
callback_controls_c0_APR (const CC_ControlCodes_CallbackData_t UNUSED(*d))
{ 
  return TRUE;
}

// handler: LS1(0x0e): Locking Shift 1
bool callback_controls_c0_LS1 (const CC_ControlCodes_CallbackData_t UNUSED(*d))
{
  return FALSE; // Não deve ser utilizado com "ClosedCaption", apenas "SuperImpose".
}

// handler: LS0(0x0f): Locking Shift 0
bool callback_controls_c0_LS0 (const CC_ControlCodes_CallbackData_t UNUSED(*d))
{
  return FALSE; // Não deve ser utilizado com "ClosedCaption", apenas "SuperImpose".
}

// handler: PAPF(0x16): Parameterized Active Position forward
// syntax: [papf] [P1]
bool callback_controls_c0_PAPF (const CC_ControlCodes_CallbackData_t *d)
{
  return ctrl_skip_params (d, 1); // Não deve ser utilizado com "ClosedCaption", apenas "SuperImpose".
}

// handler: CAN(0x18): Cancel
bool callback_controls_c0_CAN (const CC_ControlCodes_CallbackData_t UNUSED(*d))
{
  return FALSE; // Não deve ser utilizado com "ClosedCaption", apenas "SuperImpose".
}

// handler: SS2(0x19): Shift Single 2
// syntax: [ss2] [char]
bool callback_controls_c0_SS2 (const CC_ControlCodes_CallbackData_t *d)
{
  return ctrl_skip_params (d, 1); // Não deve ser utilizado com "ClosedCaption", apenas "SuperImpose".
}

// handler: ESC(0x1b): Escape
// syntax: [esc] [I1 ... In] [F], com I entre 0x20-0x2f (ISO/IEC 2022)
bool callback_controls_c0_ESC (const CC_ControlCodes_CallbackData_t *d)
{
  uint16_t n = 1;

  while (CTRL_HAS_PARAMS (d, n) && d->argv[*d->pos + n] >= 0x20 && d->argv[*d->pos + n] <= 0x2f)
    n++;

  return ctrl_skip_params (d, n); // Não deve ser utilizado com "ClosedCaption", apenas "SuperImpose".
}

/**
 * handler: APS(0x1c): Active Position Set
 * syntax: [CMD] [P1] [P2]
 *
 * <brief>
 * Specified times of active position down is made by P1 (1 byte) of the first
 * parameter in line direction length of character field from the first position
 * of the first line of the display area. Then specified times of active position
 * forward is made by the second parameter P2 (1 byte) in the character path
 * length of character field. Each parameter shall be within the range of 04/0
 * to 07/15 and specify time within the range of 0 to 63 in binary value of 6-
 * bit from b6 to b1. (b8 and b7 are not used).
 * </brief>
 */
bool
callback_controls_c0_APS (const CC_ControlCodes_CallbackData_t *d)
{
  const uint8_t *P = &d->argv[*d->pos];

  if (!CTRL_HAS_PARAMS (d, 2))
      return ctrl_skip_params (d, 2);
  if(!(P[1] >= 0x40 && P[1] <= 0x7f))
      return ctrl_skip_params (d, 2);
  if (!(P[2] >= 0x40 && P[2] <= 0x7f))
      return ctrl_skip_params (d, 2);
  
  d->caption->APS.first  = (P[1] & 0x3f);
  d->caption->APS.second = (P[2] & 0x3f);

  *d->pos += 2;

  return TRUE;
}

/**
 * handler: SS3(0x1d): Single Shift 3 (Characters G3)
 * syntax: [ss3] [code]
 *
 * <brief>
 * Caso queira mais informacoes sobre os caracteres e seus determinados subsets,
 * basta ir em ABNT NBR 15608-3:2008, Pag 5, Tabela 3
 *
 * Lembrando que utilizei como base sobre ISO-8859-15 e UTF-8 os links.
 *
 * http://en.wikipedia.org/wiki/UTF-32/UCS-4
 * http://en.wikipedia.org/wiki/UTF-8
 * http://www.fileformat.info/info/charset/ISO-8859-15/list.htm
 * http://www.fileformat.info/info/charset/UTF-8/list.htm
 * http://www.fileformat.info/info/unicode/char/a.htm
 * </brief>
 *
 * Atc, Jorge Pereira <jpereiran@gmail.com>
 * Data: Thu Oct  7 17:07:38 BRT 2010
 */
struct ss3charmap_g3 {
  uint8_t g3; /* Codigo descrito conforme norma ABNT NBR 15608-3:2008, Pag 5, Tabela 3 */
  const char *utf8; /* Caracter em UTF-8 */
} ss3chars_g3[] = {
  { 0x21, "\xe2\x99\xaa" }, // (♪) EIGHT NOTE

  { 0x30, "\xc2\xa4" }, // (¤) CURRENCY SIGN
  { 0x31, "\xc2\xa6" }, // (¦) BROKEN BAR
  { 0x32, "\xc2\xa8" }, // (¨) DIAERESIS
  { 0x33, "\xc2\xb4" }, // (´) ACUTE ACCENT
  { 0x34, "\xc2\xb8" }, // (¸) CEDILLA
  { 0x35, "\xc2\xbc" }, // (¼) VULGAR FRACTION ONE QUARTER
  { 0x36, "\xc2\xbd" }, // (½) VULGAR FRACTION ONE HALF
  { 0x37, "\xc2\xbe" }, // (¾) VULGAR FRACTION THREE QUARTERS

  { 0x40, "\xe2\x80\xa6" }, // (…) HORIZONTAL ELLIPSIS
  { 0x41, "\xe2\x96\x88" }, // (█) FULL BLOCK
  { 0x42, "\xe2\x80\x98" }, // (‘) LEFT SINGLE QUOTATION MARK
  { 0x43, "\xe2\x80\x99" }, // (’) RIGHT SINGLE QUOTATION MARK
  { 0x44, "\xe2\x80\x9c" }, // (“) LEFT DOUBLE QUOTATION MARK
  { 0x45, "\xe2\x80\x9d" }, // (”) RIGHT DOUBLE QUOTATION MARK
  { 0x46, "\xe2\x80\xa2" }, // (•) BULLET
  { 0x47, "\xe2\x84\xa2" }, // (™) TRADE MARK SIGN
  { 0x48, "\xe2\x85\x9b" }, // (⅛) VULGAR FRACTION ONE EIGHTH

  { 0x49, "\xe2\x85\x9c" }, // (⅜) VULGAR FRACTION THREE EIGHTHS
  { 0x4a, "\xe2\x85\x9d" }, // (⅝) VULGAR FRACTION FIVE EIGHTHS
  { 0x4b, "\xe2\x85\x9e" }, // (⅞) VULGAR FRACTION SEVEN EIGHTHS
};

bool
callback_controls_c0_SS3 (const CC_ControlCodes_CallbackData_t *d)
{
  const uint8_t *argv = &d->argv[*d->pos];
  CaptionClosed_t *caption = d->caption;
  uint8_t map = 0;

  if (!CTRL_HAS_PARAMS (d, 1))
    return ctrl_skip_params (d, 1);

  // Como o vetor e pequeno, e raramente tais caracteres sao enviados. preferi simplificar com um for()
  for (; map < ARRAY_SIZE (ss3chars_g3); map++)
  {
    const struct ss3charmap_g3 *m = &ss3chars_g3[map];

    if (argv[1] == m->g3)
    {
      uint8_t len = strlen (m->utf8);

      *d->pos += 1;
                
      caption_hooking_priv_run_string (caption, m->utf8, &len);
      
      return TRUE;
    }
  }

  ATV_DEBUG ("Ops! O char(0x%x) nao foi reconhecido."
             " verifique ABNT NBR 15608-3:2008, Pag 5, Tabela 3", argv[1]);

  return ctrl_skip_params (d, 1);
}

// handler: RS(0x1e): Data Separator
bool callback_controls_c0_RS (const CC_ControlCodes_CallbackData_t UNUSED(*d))
{
  return FALSE; // Não deve ser utilizado com "ClosedCaption", apenas "SuperImpose".
}

// handler: US(0x1f): Unit Separator
bool callback_controls_c0_US (const CC_ControlCodes_CallbackData_t UNUSED(*d))
{
//const uint8_t *argv = &d->argv[*d->pos];

  return TRUE;
}

/**
 * Callbacks referente aos C1
 */
static
const CaptionClosedControls_t callbacks_ctrls_c1[] = {
  // Foreground Colors Command
  CAPTION_CTRL_SET(CTRL_C1_BKF, callback_controls_c1_BKF), // array[0] (0x80)
  CAPTION_CTRL_SET(CTRL_C1_RDF, callback_controls_c1_RDF),
  CAPTION_CTRL_SET(CTRL_C1_GRF, callback_controls_c1_GRF),
  CAPTION_CTRL_SET(CTRL_C1_YLF, callback_controls_c1_YLF),
  CAPTION_CTRL_SET(CTRL_C1_BLF, callback_controls_c1_BLF),
  CAPTION_CTRL_SET(CTRL_C1_MGF, callback_controls_c1_MGF),
  CAPTION_CTRL_SET(CTRL_C1_CNF, callback_controls_c1_CNF),
  CAPTION_CTRL_SET(CTRL_C1_WHF, callback_controls_c1_WHF),

  // Controles dos Caracteres
  CAPTION_CTRL_SET(CTRL_C1_SSZ, callback_controls_c1_SSZ),
  CAPTION_CTRL_SET(CTRL_C1_MSZ, callback_controls_c1_MSZ),
  CAPTION_CTRL_SET(CTRL_C1_NSZ, callback_controls_c1_NSZ),
  CAPTION_CTRL_SET(CTRL_C1_SZX, callback_controls_c1_SZX),
  CAPTION_CTRL_SET(CTRL_C1_PAD_0x8c, NULL), // ] 
  CAPTION_CTRL_SET(CTRL_C1_PAD_0x8d, NULL), // ] Padding: Apenas para alinhamento da estrutura. 
  CAPTION_CTRL_SET(CTRL_C1_PAD_0x8e, NULL), // ]
  CAPTION_CTRL_SET(CTRL_C1_PAD_0x8f, NULL), // ] 
  CAPTION_CTRL_SET(CTRL_C1_COL, callback_controls_c1_COL),

  //
  CAPTION_CTRL_SET(CTRL_C1_FLC, callback_controls_c1_FLC),
  CAPTION_CTRL_SET(CTRL_C1_CDC, callback_controls_c1_CDC),
  CAPTION_CTRL_SET(CTRL_C1_POL, callback_controls_c1_POL),
  CAPTION_CTRL_SET(CTRL_C1_WMM, callback_controls_c1_WMM),
  CAPTION_CTRL_SET(CTRL_C1_MAC, callback_controls_c1_MAC),

  // (0x96): Padding
  CAPTION_CTRL_SET(CTRL_C1_PAD_0x96, NULL), // Padding: Apenas para alinhamento da estrutura. 
  CAPTION_CTRL_SET(CTRL_C1_HLC, callback_controls_c1_HLC),
  CAPTION_CTRL_SET(CTRL_C1_RPC, callback_controls_c1_RPC),
  CAPTION_CTRL_SET(CTRL_C1_SPL, callback_controls_c1_SPL),
  CAPTION_CTRL_SET(CTRL_C1_STL, callback_controls_c1_STL),

  // (0x9b): Control Sequence Introducer
  CAPTION_CTRL_SET(CTRL_C1_CSI, callback_controls_c1_CSI_load),

  CAPTION_CTRL_SET(CTRL_C1_PAD_0x9c, NULL), // Padding: Apenas para alinhamento da estrutura. 

  CAPTION_CTRL_SET(CTRL_C1_TIM, callback_controls_c1_TIM),

  // (0x9d/0x9f): Padding
  CAPTION_CTRL_SET(CTRL_C1_PAD_0x9e, NULL), // Padding: Apenas para alinhamento da estrutura. 
  CAPTION_CTRL_SET(CTRL_C1_PAD_0x9f, NULL)  // Padding: Apenas para alinhamento da estrutura. 
};

/**
 * Callbacks relacionado aos determinados 'comandos' de controle C1
 */

// handler: BKF(0x80): Black Foreground Color
// syntax: [bkf]
bool
callback_controls_c1_BKF (const CC_ControlCodes_CallbackData_t *d)
{
  return caption_closed_controls_cmlat2rgb (SET_CMLA0_BLACK, &d->caption->foreground);
}

// handler: RDF(0x81): Red Foreground Color
// syntax: [rdf]
bool
callback_controls_c1_RDF (const CC_ControlCodes_CallbackData_t *d)
{
  return caption_closed_controls_cmlat2rgb (SET_CMLA1_FULL_INTENSITY_RED, &d->caption->foreground);
}

// handler: GRF(0x82): Green Foreground Color 
// syntax: [grf]
bool
callback_controls_c1_GRF (const CC_ControlCodes_CallbackData_t *d)
{
  return caption_closed_controls_cmlat2rgb (SET_CMLA2_FULL_INTENSITY_GREEN, &d->caption->foreground);
}

// handler: YLF(0x83): Yellow Foreground Color 
// syntax: [ylf]
bool
callback_controls_c1_YLF (const CC_ControlCodes_CallbackData_t *d)
{
  return caption_closed_controls_cmlat2rgb (SET_CMLA3_FULL_INTENSITY_YELLOW, &d->caption->foreground);
}

// handler: BLF(0x84): Blue Foreground Color 
// syntax: [blf]
bool
callback_controls_c1_BLF (const CC_ControlCodes_CallbackData_t *d)
{
  return caption_closed_controls_cmlat2rgb (SET_CMLA4_FULL_INTENSITY_BLUE, &d->caption->foreground);
}

// handler: MGF(0x85): Magneta Foreground Color 
// syntax: [mgf]
bool
callback_controls_c1_MGF (const CC_ControlCodes_CallbackData_t *d)
{
  return caption_closed_controls_cmlat2rgb (SET_CMLA5_FULL_INTENSITY_MAGNETA, &d->caption->foreground);
}

// handler: CNF(0x86): Cyan Foreground Color 
// syntax: [cnf]
bool
callback_controls_c1_CNF (const CC_ControlCodes_CallbackData_t *d)
{
  return caption_closed_controls_cmlat2rgb (SET_CMLA6_FULL_INTENSITY_CYAN, &d->caption->foreground);
}

// handler: WHF(0x87): White Foreground Format
// syntax: [whf]
bool
callback_controls_c1_WHF (const CC_ControlCodes_CallbackData_t *d)
{
  return caption_closed_controls_cmlat2rgb (SET_CMLA7_FULL_INTENSITY_WHITE, &d->caption->foreground);
}

// handler: SSZ(0x88): Small Size
// syntax: [ssz]
bool
callback_controls_c1_SSZ (const CC_ControlCodes_CallbackData_t *d)
{
  d->caption->char_size = CC_CharacterSize_Small;

  return TRUE;
}

// handler: (0x89): Middle Size
// syntax: [msz]
bool
callback_controls_c1_MSZ (const CC_ControlCodes_CallbackData_t *d)
{
  d->caption->char_size = CC_CharacterSize_Middle;

  return TRUE;
}

// handler: (0x8a): Normal Size
// syntax: [nsz]
bool
callback_controls_c1_NSZ (const CC_ControlCodes_CallbackData_t *d)
{
  d->caption->char_size = CC_CharacterSize_Standard;

  return TRUE;
}

// handler: (0x8b): Specification Size
// syntax: [szx] [tamanho]
bool
callback_controls_c1_SZX (const CC_ControlCodes_CallbackData_t *d)
{
  const uint8_t *argv = &d->argv[*d->pos];
  
  if (!CTRL_HAS_PARAMS (d, 1))
    return ctrl_skip_params (d, 1);
  
  d->caption->char_size_custom = argv[1];
  *d->pos += 1;
  
  return TRUE; // Não deve ser utilizado com "ClosedCaption", apenas "SuperImpose".
}

// handler: COL(0x90): Color Specification
// syntax: [col] [cor]
bool
callback_controls_c1_COL (const CC_ControlCodes_CallbackData_t *d)
{
  const uint8_t *argv = &d->argv[*d->pos];

   if (!CTRL_HAS_PARAMS (d, 1) || (argv[1] == 0x20 && !CTRL_HAS_PARAMS (d, 2)))
     return ctrl_skip_params (d, 2);

   // argv[1] deve ser 0x20 (com argv[2] entre 0x40 e 0x4f) ou estar entre 0x48 e 0x7f.
   if ((argv[1] != 0x20 && (argv[1] < 0x48 || argv[1] > 0x7f)) ||
       (argv[1] == 0x20 && (argv[2] < 0x40 || argv[2] > 0x4f)))
   {
     ATV_DEBUG ("Oops! Problemas com valor da cor (0x%02x), talvez payload invalido.!",
                argv[1]);
     return ctrl_skip_params (d, (argv[1] == 0x20) ? 2 : 1);
   }
   else
   {
     // Tratar a cor recebida.
     if (argv[1] == 0x20) // Especifica a paleta conforme o numero do parametro.
     {
       // TODO: A Norma nao explica muito bem, eu entendi desta forma.
       // caso der algum problema, favor rever a norma em "Table 7-16 C1 control set
       // Comando "COL" e então revise este trecho.

       uint8_t cmla = (argv[2] - 0x40); // Decrementa o primeiro valor para posicionar no indice.

       // Conforme a norma, argv[2] deve ser entre 0x40 e 0x4f
       *d->pos += 2;
        
       return caption_closed_controls_cmlat2rgb (cmla, &d->caption->foreground);
     }
     else
     {
       CC_Color_t *color = NULL;
       uint8_t cmla = argv[1];

       // Seguindo a ordem descrita na norma ARIB_TR_B24, Table 7-16 C1 control set. "COL"
       switch (cmla)
       {
         case 0x48 ... 0x4f: // Setting Foreground.
           color = &d->caption->foreground;
           cmla -= 0x40;
         break;

         case 0x50 ... 0x5f: // Setting Background.
           color = &d->caption->background;
           cmla -= 0x50;
         break;

         case 0x60 ... 0x6f: // Setting Half Foreground.
           color = &d->caption->half_foreground;
           cmla -= 0x60;
         break;

         case 0x70 ... 0x7f: // Setting Half Background.
           color = &d->caption->half_background;
           cmla -= 0x70;
         break;

         default:

           ATV_DEBUG ("Oops! Comando de COR(0x%02x) invalido, talvez desconhecido?", cmla);
           return FALSE;
       }

       *d->pos += 1;

       return caption_closed_controls_cmlat2rgb (cmla, color);
     }

     return TRUE;
   }  
}

bool
callback_controls_c1_FLC (const CC_ControlCodes_CallbackData_t *d)
{
  return ctrl_skip_params (d, 1); // Não deve ser utilizado com "ClosedCaption", apenas "SuperImpose".
}

// syntax: [cdc] [P1] ou [cdc] [0x20] [P1]
bool
callback_controls_c1_CDC (const CC_ControlCodes_CallbackData_t *d)
{
  bool two = CTRL_HAS_PARAMS (d, 1) && d->argv[*d->pos + 1] == 0x20;

  return ctrl_skip_params (d, two ? 2 : 1); // Não deve ser utilizado com "ClosedCaption", apenas "SuperImpose".
}

bool
callback_controls_c1_POL (const CC_ControlCodes_CallbackData_t *d)
{
  return ctrl_skip_params (d, 1); // Não deve ser utilizado com "ClosedCaption", apenas "SuperImpose".
}

bool
callback_controls_c1_WMM (const CC_ControlCodes_CallbackData_t *d)
{
  return ctrl_skip_params (d, 1); // Não deve ser utilizado com "ClosedCaption", apenas "SuperImpose".
}

bool
callback_controls_c1_MAC (const CC_ControlCodes_CallbackData_t *d)
{
  return ctrl_skip_params (d, 1); // Não deve ser utilizado com "ClosedCaption", apenas "SuperImpose".
}

bool
callback_controls_c1_HLC (const CC_ControlCodes_CallbackData_t *d)
{
  return ctrl_skip_params (d, 1); // Não deve ser utilizado com "ClosedCaption", apenas "SuperImpose".
}

bool
callback_controls_c1_RPC (const CC_ControlCodes_CallbackData_t *d)
{
  return ctrl_skip_params (d, 1); // Não deve ser utilizado com "ClosedCaption", apenas "SuperImpose".
}

bool
callback_controls_c1_SPL (const CC_ControlCodes_CallbackData_t UNUSED(*d))
{
  return FALSE; // Não deve ser utilizado com "ClosedCaption", apenas "SuperImpose".
}

bool
callback_controls_c1_STL (const CC_ControlCodes_CallbackData_t UNUSED(*d))
{
  return FALSE; // Não deve ser utilizado com "ClosedCaption", apenas "SuperImpose".
}

// syntax: [time] [0x20] [P1]
bool
callback_controls_c1_TIM (const CC_ControlCodes_CallbackData_t *d)
{
  return ctrl_skip_params (d, 2); // Não deve ser utilizado com "ClosedCaption", apenas "SuperImpose".
}

/**
 * Fazendo analogia a norma, lembre-se de acessar o Pn->argv[n]
 * tendo sempre o valor de 'n' - 1, ou seja. Quando você quiser o P2
 * voce ira acessar argv[1].
 */
bool
csi_arg_Pn_load (CC_ControlCodes_CallbackData_t *d)
{
  struct csi_arg_Pn *Pn = d->data;
  uint16_t i = 0;
  uint8_t buf_n = 0;
  char buf[CTRL_CSI_MAX_Pn_FIELDS] = { 0, };
  
  assert (Pn != NULL);
  assert (d->argv != NULL);
  if (d->argc <= 0)
      return FALSE;

//ATV_DEBUG ("Carregando parametros Pn");
//ATV_HEXDUMP (d->argv, d->argc);

  for (Pn->argc = 0, i = 0; i < d->argc && Pn->argc < CTRL_CSI_MAX_Pn_ARGS; i++)
  {
    if ((d->argv[i] >= '0' && d->argv[i] <= '9') && buf_n < (CTRL_CSI_MAX_Pn_FIELDS - 1))
    {
      buf[buf_n] = (char)d->argv[i];
      buf_n     += 1;
      buf[buf_n] = '\0';
    }
    
    if (buf_n > 0 /* algo em 'buf'? */ &&
        (d->argv[i] == CTRL_CSI_IB /* token? */ || (i+1) == d->argc /* final do buf d->argv[]? */))
    {
      // Base 10: os parametros sao decimais, e podem ter zeros a esquerda (ex: "08").
      unsigned long v = strtoul (&buf[0], NULL, 10);

      Pn->argv[Pn->argc] = (v > UINT16_MAX) ? UINT16_MAX : (uint16_t)v;
      Pn->argc += 1;
      buf_n = 0;
    }
  }

  return (Pn->argc > 0);
}

/**
 * Callbacks utilizadas para processamento dos determinados subsets
 * do CSI conforme a norma.
 */
static CaptionClosedControls_t callbacks_ctrls_c1_csi[] = {
  CAPTION_CTRL_SET(CTRL_C1_CSI_SWF, callback_controls_c1_CSI_swf),
  CAPTION_CTRL_SET(CTRL_C1_CSI_PAD_0x54, NULL),
  CAPTION_CTRL_SET(CTRL_C1_CSI_PAD_0x55, NULL),
  CAPTION_CTRL_SET(CTRL_C1_CSI_SDF, callback_controls_c1_CSI_sdf),
  CAPTION_CTRL_SET(CTRL_C1_CSI_SSM, callback_controls_c1_CSI_ssm),
  CAPTION_CTRL_SET(CTRL_C1_CSI_SHS, callback_controls_c1_CSI_shs),
  CAPTION_CTRL_SET(CTRL_C1_CSI_SVS, callback_controls_c1_CSI_svs),
  CAPTION_CTRL_SET(CTRL_C1_CSI_PAD_0x5a, NULL),
  CAPTION_CTRL_SET(CTRL_C1_CSI_PAD_0x5b, NULL),
  CAPTION_CTRL_SET(CTRL_C1_CSI_PAD_0x5c, NULL),
  CAPTION_CTRL_SET(CTRL_C1_CSI_PAD_0x5d, NULL),
  CAPTION_CTRL_SET(CTRL_C1_CSI_PAD_0x5e, NULL),
  CAPTION_CTRL_SET(CTRL_C1_CSI_SDP, callback_controls_c1_CSI_sdp),
  CAPTION_CTRL_SET(CTRL_C1_CSI_PAD_0x60, NULL),
  CAPTION_CTRL_SET(CTRL_C1_CSI_PAD_0x61, NULL),
  CAPTION_CTRL_SET(CTRL_C1_CSI_PAD_0x62, NULL),
  CAPTION_CTRL_SET(CTRL_C1_CSI_ORN, callback_controls_c1_CSI_orn),
  CAPTION_CTRL_SET(CTRL_C1_CSI_PAD_0x64, NULL),
  CAPTION_CTRL_SET(CTRL_C1_CSI_CFS, callback_controls_c1_CSI_cfs),
  CAPTION_CTRL_SET(CTRL_C1_CSI_PAD_0x66, NULL),
  CAPTION_CTRL_SET(CTRL_C1_CSI_PAD_0x67, NULL),
  CAPTION_CTRL_SET(CTRL_C1_CSI_PAD_0x68, NULL),
  CAPTION_CTRL_SET(CTRL_C1_CSI_PAD_0x69, NULL),
  CAPTION_CTRL_SET(CTRL_C1_CSI_PAD_0x6a, NULL),
  CAPTION_CTRL_SET(CTRL_C1_CSI_PAD_0x6b, NULL),
  CAPTION_CTRL_SET(CTRL_C1_CSI_PAD_0x6c, NULL),
  CAPTION_CTRL_SET(CTRL_C1_CSI_PAD_0x6d, NULL),
  CAPTION_CTRL_SET(CTRL_C1_CSI_RCS, callback_controls_c1_CSI_rcs)
};
#define CALLBACKS_CONTROLS_C1_CSI_LENGTH 28

bool
callback_controls_c1_CSI_load (const CC_ControlCodes_CallbackData_t *d)
{
  const uint8_t *argv = &d->argv[*d->pos];
  uint16_t i = 0;
  CaptionClosed_CtrlCode_t *current_code = (CaptionClosed_CtrlCode_t*)&d->current_code;

//ATV_DEBUG("Processando um CSI");
//ATV_HEXDUMP (argv, d->argc - *d->pos);

  // inicia em argv[1] ignorando o 'cmd=0x1b' vindo em argv[0]
  for (i = 1; i < (d->argc - 1); i++)
  {
  //ATV_DEBUG("Checando argv[%d] 0x%02x", i, argv[i]);
  //  assert ((argv + i + 1) != NULL);

    if (argv[i] == CTRL_CSI_FB)
    {
      const uint8_t cmd = argv[i+1];
      const uint8_t cmd_indice = (cmd - CTRL_C1_CSI_SWF); // Comando subset do CSI
      bool (*callback)(const CC_ControlCodes_CallbackData_t*) = NULL;

      if (cmd_indice >= CALLBACKS_CONTROLS_C1_CSI_LENGTH || 
              (callback = callbacks_ctrls_c1_csi[cmd_indice].callback) == NULL)
      {
        ATV_DEBUG("Callback não encontrado para o code(0x%02x)", cmd);

        // Pula toda a sequencia, para nao ser tratada como texto.
        *d->pos += i + 1;
        return FALSE;
      }
      else
      {
        struct csi_arg_Pn csi_pn = {
          .argv = { 0, },
          .argc = 0
        };
        CC_ControlCodes_CallbackData_t arg_subset = {
          .argv = &argv[1], // Pula o argv[0] referente ao CTRL_C1_CSI
          .argc = (i - 1),  // Remove o argv[total-1] referente ao CTRL_CSI_FB
          .pos = NULL,
          .caption = d->caption,
          .data = &csi_pn,
          .current_code = 0
        };
        bool ret = FALSE;
 
        ATV_DEBUG("  [+] Processando subset -> CSI::%s hex(0x%02x)", 
            callbacks_ctrls_c1_csi[cmd_indice].code_name, cmd);

        if (!csi_arg_Pn_load (&arg_subset))
        {
          ATV_DEBUG("  [!!] Falhou ao processar o subset -> CSI::%s hex(0x%02x)", 
              callbacks_ctrls_c1_csi[cmd_indice].code_name, cmd);

          ATV_DEBUG("Sem parametros para csi_arg_Pn_load()");
        }
      
        // 1) Faz o jump em *d->pos
        *d->pos += i + 1;

        // 2) Chama e recupera o retorno da callback, conforme o subset recebido.
        ret = callback (&arg_subset);
        if (!ret)
        {
          ATV_DEBUG("Oops! Falha durante processamento de CSI::%s hex(0x%02x)", 
              callbacks_ctrls_c1_csi[cmd_indice].code_name, cmd);
          ATV_HEXDUMP (arg_subset.argv, arg_subset.argc);
        }

        *current_code = cmd; // Atualiza o 'code' para o codigo C1_CSI (subset)
        return ret;
      }
    }
  }

  // Sequencia incompleta (sem o 'F'), descarta ate o final do buffer.
  return ctrl_skip_params (d, d->argc);
}

/**
 * handler: CSI::SWF(0x53): Set Window Format
 *
 * Select initialization with parameter P1 (1 or multiple codes) and initializing is done.
 * Code sequence: CSI P11 ~ P1i I1F
 *
 * CSI:       09/11 (control sequence introducer)
 * P11 ~ P1i: 03/0 ~ 03/9 (decimal number specifying format)
 * I1:        02/0 (intermediate character)
 * F:         05/3 (final character)
 *
 * Decimal numbers specifying format are as follows.
 */
bool
callback_controls_c1_CSI_swf (const CC_ControlCodes_CallbackData_t *d)
{
  struct csi_arg_Pn *Pn = d->data;
  uint16_t *P = Pn->argv;
  CaptionClosed_t *c = d->caption;

//  assert (Pn->argc > 0);

  switch (Pn->argc) // Quantos 'P'?
  {
    case 1: // Apenas P1, Então e o 'Format'
    {
      if (P[0] >= CC_Format_t_eMax)
        return FALSE;

      c->format = P[0];
    }
    break;

    case 2 ... 4: // P2 ... P4
    {
      // P1
      if (P[0] != CC_Format_HOR_960x540)
        return FALSE;
      c->format = CC_Format_HOR_960x540;

      // P2
      if (P[1] == 0 || P[1] == 1 || P[1] == 3)
        c->char_size = P[1];

      // P3 e P4 (zerados quando nao informados)
      c->char_number_in_oneline = P[2];
      c->char_line_number = P[3];
    }
    break;

    default:
    {
      // TODO: Como a norma e bem confusa, e nao temos 'captions' de testes
      // para entender direito como resolver tal interpretacao. eu irei deixar
      // este assert() para caso consiga um 'caption' com tal valor, favor
      // parar e estudar tais parametros.
      //
      // Atc. Jorge Pereira <jpereiran@gmail.com>

      ATV_DEBUG("Ops, Qtd de parametros (%d) nao esperado. favor analizar a norma!",
              Pn->argc);
      //assert(FALSE);
      return FALSE;
    }
  }
  
  return TRUE;
}

// handler: CSI::SSM(0x57): Character composition dot designation
// syntax: CSI P11 ~ P1i I1 P21 ~ P2j I2 F
bool
callback_controls_c1_CSI_ssm (const CC_ControlCodes_CallbackData_t *d)
{
  struct csi_arg_Pn *Pn = d->data;
  uint16_t *P = Pn->argv;

  d->caption->SSM.width  = P[0]; // P1
  d->caption->SSM.height = P[1]; // P2

  return TRUE;
}

// handler: CSI::SHS(0x58): Set Horizontal Spacing
// syntax: CSI P11 ~ P1iI1F
bool
callback_controls_c1_CSI_shs (const CC_ControlCodes_CallbackData_t *d)
{
  struct csi_arg_Pn *Pn = d->data;
  uint16_t *P = Pn->argv;

  d->caption->SHS = P[0]; // P1

  return TRUE;
}

// handler: CSI::SVS(0x59): Set Vertical Spacing
// syntax: CSI P11 ~ P1I I1 F
bool
callback_controls_c1_CSI_svs (const CC_ControlCodes_CallbackData_t *d)
{
  struct csi_arg_Pn *Pn = d->data;
  uint16_t *P = Pn->argv;

  d->caption->SVS = P[0]; // P1

  return TRUE;
}

/**
 * handler: CSI::SDF(0x56): Set Display Format 
 *
 * <brief>
 * Display dot number is set using parameter P1 (1 or multiple codes) and P2 (1 or multiple codes).
 *
 * Code sequence: CSI P11 ~ P1i I1 P21 ~ P2j I2 F 
 * CSI:       09/11 (control sequence introducer)
 * P11 ~ P1i: 03/0 ~ 03/9 (dot numbers in horizontal direction)
 * P21 ~ P2j: 03/0 ~ 03/9 (dot numbers in vertical direction)
 * I1:        03/11 (middle character)
 * I2:        02/0 (middle character)
 * F:         05/6 (final character)
 * </brief>
 */
bool
callback_controls_c1_CSI_sdf (const CC_ControlCodes_CallbackData_t *d)
{
  struct csi_arg_Pn *Pn = d->data;
  uint16_t *P = Pn->argv;

  d->caption->SDF.width = P[0]; // P1
  d->caption->SDF.height   = P[1]; // P2

  return TRUE;
}

// handler: CSI::SDP(0x5f): Set Display Position
// syntax: CSI P11 ~ P1i I1 P21 ~ P2j I2F
bool
callback_controls_c1_CSI_sdp (const CC_ControlCodes_CallbackData_t *d)
{
  struct csi_arg_Pn *Pn = d->data;
  uint16_t *P = Pn->argv;

  d->caption->SDP.width = P[0]; // P1
  d->caption->SDP.height   = P[1]; // P2

  return TRUE;
}

/**
 * handler: CSI::ORN(0x63): Ornament Control
 *
 * Character ornament designation is made using parameter P1 (1 code) and
 * character ornament colour is set using parameter P2 (4 codes)
 * Code sequence: CSI P1 I1 P21 P22 P23 P24 I2 F
 *
 * CSI: 9/11 (control sequence introducer)
 * P1: 
 * 03/0: without character decoration
 * 03/1: with hemming
 * 03/2: with shade
 * 03/3: with hollow
 * I1:
 * 3/11 (middle character)
 * P21 P22: 03/0 ~ 03/9 (upper 4 bit of colour map address)
 * P23 P24: 03/0 ~ 03/9 (lower 4 bit of colour map address)
 * I2: 02/0 (middle character)
 * F:  06/3 (final character)
 */
bool
callback_controls_c1_CSI_orn (const CC_ControlCodes_CallbackData_t UNUSED(*d))
{
//struct csi_arg_Pn *Pn = d->data;
//uint16_t *P = Pn->argv;
//CaptionClosed_t *c = d->caption;

  return FALSE; // Não implementado
}

// handler: CSI::CFS(0x65): Character Font Set
// syntax: CSI P11 ~ P1i I1 F
bool
callback_controls_c1_CSI_cfs (const CC_ControlCodes_CallbackData_t UNUSED(*d))
{
//struct csi_arg_Pn *Pn = d->data;
//uint16_t *P = Pn->argv;
//CaptionClosed_t *c = d->caption;

  return FALSE; // Não implementado
}

// handler: CSI::RCS(0x6e): Raster Colour Command
// syntax: CSI P11 ~ P1i IF
bool
callback_controls_c1_CSI_rcs (const CC_ControlCodes_CallbackData_t *d)
{
  struct csi_arg_Pn *Pn = d->data;
  uint16_t *P = Pn->argv;

  if (P[0] >= CC_Rcs_t_eMax)
    return FALSE;

  d->caption->RCS = P[0]; // P1

  return TRUE;
}

bool
caption_closed_controls_is_table (const uint8_t *byte,
                                  enum CC_ControlCodes_TableJump_e *table)
{
  if (*byte >= CTRL_C0_APB && *byte <= CTRL_C0_US)
  {
    *table = CC_CTRLCODE_SET_C0;
  }
  else if (*byte >= CTRL_C1_BKF && *byte <= CTRL_C1_PAD_0x9f)
  {
    *table = CC_CTRLCODE_SET_C1;
  }
  else
  {
    *table = CC_CTRLCODE_SET_UNKNOWN;
    return FALSE;
  }

  return TRUE;
}

/**
 * Aqui e praticamente o main(), onde e feito todo o parser e 'gateway' dos
 * comandos e subsets recebidos no payload de caption.
 **/
bool
caption_closed_controls_load (enum CC_ControlCodes_TableJump_e table,
                              const uint8_t *argv,
                              const uint16_t argc,
                              uint16_t *pos,
                              CaptionClosed_t *caption)
{
  bool (*callback)(const CC_ControlCodes_CallbackData_t *) = NULL;
  CaptionClosed_CtrlCode_t code = 0, code_indice = 0;

  assert (argv != NULL);
  assert (argc > 0);
  assert (pos != NULL);
  assert (caption != NULL);
  assert (table != CC_CTRLCODE_SET_UNKNOWN);

  // Pergunta: por que do '- CC_CTRLCODE_SET_C0/1'?
  // Resposta: Simples, basta voce analisar a tabela que mencionei na definicao de
  // "ControlCodes_t" que ira perceber que fiz um 'padding' na sua definicao facilitando
  // o acesso direto ao 'callback/handler' atraves de seu codigo de controle.
  //
  // Atc, Jorge Pereira
  // Date: Thu Oct 14 20:33:42 BRT 2010
  code        = argv[*pos];
  code_indice = code - table;
  if (table == CC_CTRLCODE_SET_C0)
    callback = callbacks_ctrls_c0[code_indice].callback;
  else if (table == CC_CTRLCODE_SET_C1)
    callback = callbacks_ctrls_c1[code_indice].callback;
  else
    //assert (FALSE);
    return FALSE;

  if (callback != NULL)
  {
    CC_ControlCodes_CallbackData_t d = {
      .argv = argv,
      .argc = argc,
      .pos = pos,
      .caption = caption,
      .current_code = code,
      .data = NULL
    };
    bool ret = callback (&d);

/*  ATV_DEBUG("[-] O Callback '%s'(0x%02x) retornou %s", 
       caption_closed_controls_get_name (code), code PRINT_BOOL(ret));*/

    if (ret)
    {
      caption_hooking_priv_run_controls (d.caption, d.current_code);
    }
    
    if (!ret)
    {
      ATV_DEBUG("[-] O Callback '%s'(0x%02x) retornou %s", 
       caption_closed_controls_get_name (code), code, PRINT_BOOL(ret));
      ATV_DEBUG ("Abaixo segue 'payload' invalido ou desconhecido!");
      ATV_HEXDUMP (&d.argv[*pos], d.argc);
    }

    return ret;
  }

  ATV_DEBUG("Oops! Nao existe callback para (%s) (0x%02x) (shift table=0x%02x)",
      caption_closed_controls_get_name (code), code, table);

  return FALSE;
}

/**
 * Este metodo so tera efeito quando compilado utilizando flag de depuracao
 * deste componente.
 */
const char *
caption_closed_controls_get_name (const uint8_t code)
{
  // Os subsets do CSI (0x53 ~ 0x6e) colidem com os caracteres, entao sao
  // consultados apenas quando o 'code' nao e um C0/C1.
  if (code >= CC_CTRLCODE_SET_C0 && code < CC_CTRLCODE_SET_C0 + ARRAY_SIZE (callbacks_ctrls_c0))
    return callbacks_ctrls_c0[code - CC_CTRLCODE_SET_C0].code_name;

  if (code >= CC_CTRLCODE_SET_C1 && code < CC_CTRLCODE_SET_C1 + ARRAY_SIZE (callbacks_ctrls_c1))
    return callbacks_ctrls_c1[code - CC_CTRLCODE_SET_C1].code_name;

  if (code >= CTRL_C1_CSI_SWF && code < CTRL_C1_CSI_SWF + ARRAY_SIZE (callbacks_ctrls_c1_csi))
    return callbacks_ctrls_c1_csi[code - CTRL_C1_CSI_SWF].code_name;

  ATV_DEBUG("Code (0x%02x) nao encontrado", code);

  return "UNKNOWN";
}
