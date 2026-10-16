/**
 * @file caption-closed-controls.c
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @date Thu Aug 12 13:49:21 BRT 2010
 * @brief Comandos de controle C0, C1 e CSI do texto da legenda.
 *
 * As tabelas de callbacks precisam seguir a ordem dos códigos da norma
 * (ABNT NBR 15608-3:2008, Tabela 2; ARIB TR-B14, Tabela 3.11): o índice de
 * cada entrada é o código do comando menos o início da tabela.
 *
 * Os comandos usados em closed caption seguem a ARIB STD-B24, Volume 1,
 * 7.2, com as restrições das Tabelas 6-9 a 6-11 da ARIB TR-B14 (6.5.1). Os
 * comandos de posição, tamanho, formato, efeitos e tempo (APB, APF, APD, APU,
 * PAPF, SZX, FLC, STL, SPL, HLC, TIME etc.) não são usados durante a rolagem
 * (roll-up) (ARIB TR-B14, 4.5.7.2).
 *
 * Os comandos foram testados com legendas da TV Globo (2010) e da TV Câmara
 * (veja docs/); os que não aparecem nessas amostras não foram testados.
 */
#include "caption-closed-controls.h"
#include "caption-hooking-priv.h"
#include "caption-common.h"

/*
 * Callbacks dos comandos C0, a partir de 0x08 (APB). Os comandos que não são
 * usados em closed caption retornam FALSE. O BEL (0x07) é ignorado antes de
 * chegar aqui, em caption_closed_run().
 */
static
const CaptionClosedControls_t callbacks_ctrls_c0[] = {
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
  CAPTION_CTRL_SET(CTRL_C0_PAD_0x03, NULL), //    +--> sem uso (0x10 - 0x15)
  CAPTION_CTRL_SET(CTRL_C0_PAD_0x04, NULL), //    |
  CAPTION_CTRL_SET(CTRL_C0_PAD_0x05, NULL), //    |
  CAPTION_CTRL_SET(CTRL_C0_PAD_0x06, NULL), // <--+
  CAPTION_CTRL_SET(CTRL_C0_PAPF, callback_controls_c0_PAPF),
  CAPTION_CTRL_SET(CTRL_C0_PAD_0x17, NULL), // sem uso

  CAPTION_CTRL_SET(CTRL_C0_CAN, callback_controls_c0_CAN),
  CAPTION_CTRL_SET(CTRL_C0_SS2, callback_controls_c0_SS2),
  CAPTION_CTRL_SET(CTRL_C0_PAD_0x1a, NULL), // sem uso
  CAPTION_CTRL_SET(CTRL_C0_ESC, callback_controls_c0_ESC),
  CAPTION_CTRL_SET(CTRL_C0_APS, callback_controls_c0_APS),
  CAPTION_CTRL_SET(CTRL_C0_SS3, callback_controls_c0_SS3),
  CAPTION_CTRL_SET(CTRL_C0_RS, callback_controls_c0_RS),
  CAPTION_CTRL_SET(CTRL_C0_US, callback_controls_c0_US),
};

/* Retorna TRUE se o comando atual tem pelo menos 'n' bytes de parâmetros no buffer. */
#define CTRL_HAS_PARAMS(d, n) ((uint32_t)(n) < (uint32_t)(d)->argc)

/*
 * Pula os 'n' bytes de parâmetros do comando atual, evitando que sejam tratados
 * como texto quando o comando não é suportado. Nunca passa do final do buffer.
 *
 * Retorna FALSE, para ser usado diretamente como retorno das callbacks.
 */
static bool
ctrl_skip_params (const CC_ControlCodes_CallbackData_t *d,
                  uint16_t n)
{
  *d->pos += CTRL_HAS_PARAMS (d, n) ? n : (d->argc - 1);

  return FALSE;
}

/* <C0> */

// APB (0x08): Active Position Backwards
bool callback_controls_c0_APB (const CC_ControlCodes_CallbackData_t UNUSED(*d))
{
  return FALSE; // Não é usado em closed caption, apenas em superimpose.
}

// APF (0x09): Active Position Forward
// sintaxe: [apf]
bool
callback_controls_c0_APF (const CC_ControlCodes_CallbackData_t UNUSED(*d))
{
  // Aceito sem alterar o estado: a posição do cursor não é acompanhada.
  return TRUE;
}

// APD (0x0a): Active Position Down
bool callback_controls_c0_APD (const CC_ControlCodes_CallbackData_t UNUSED(*d))
{
  return FALSE; // Não é usado em closed caption, apenas em superimpose.
}

// APU (0x0b): Active Position Up
bool callback_controls_c0_APU (const CC_ControlCodes_CallbackData_t UNUSED(*d))
{
  return FALSE; // Não é usado em closed caption, apenas em superimpose.
}

// CS (0x0c): Clear Screen
// sintaxe: [cs]
bool
callback_controls_c0_CS (const CC_ControlCodes_CallbackData_t *d)
{
  CaptionClosed_t *caption = d->caption;

  assert (caption != NULL);

  // Sinaliza que a tela deve ser limpa.
  caption->need_clear = TRUE;

  return TRUE;
}

// APR (0x0d): Active Position Return
// sintaxe: [apr]
bool
callback_controls_c0_APR (const CC_ControlCodes_CallbackData_t UNUSED(*d))
{
  return TRUE;
}

// LS1 (0x0e): Locking Shift 1
bool callback_controls_c0_LS1 (const CC_ControlCodes_CallbackData_t UNUSED(*d))
{
  return FALSE; // Não é usado em closed caption, apenas em superimpose.
}

// LS0 (0x0f): Locking Shift 0
bool callback_controls_c0_LS0 (const CC_ControlCodes_CallbackData_t UNUSED(*d))
{
  return FALSE; // Não é usado em closed caption, apenas em superimpose.
}

// PAPF (0x16): Parameterized Active Position forward
// sintaxe: [papf] [P1]
bool callback_controls_c0_PAPF (const CC_ControlCodes_CallbackData_t *d)
{
  return ctrl_skip_params (d, 1); // Não é usado em closed caption, apenas em superimpose.
}

// CAN (0x18): Cancel
bool callback_controls_c0_CAN (const CC_ControlCodes_CallbackData_t UNUSED(*d))
{
  return FALSE; // Não é usado em closed caption, apenas em superimpose.
}

// SS2 (0x19): Shift Single 2
// sintaxe: [ss2] [char]
bool callback_controls_c0_SS2 (const CC_ControlCodes_CallbackData_t *d)
{
  return ctrl_skip_params (d, 1); // Não é usado em closed caption, apenas em superimpose.
}

// ESC (0x1b): Escape
// sintaxe: [esc] [I1 ... In] [F], com I entre 0x20-0x2f (ISO/IEC 2022)
bool callback_controls_c0_ESC (const CC_ControlCodes_CallbackData_t *d)
{
  uint16_t n = 1;

  while (CTRL_HAS_PARAMS (d, n) && d->argv[*d->pos + n] >= 0x20 && d->argv[*d->pos + n] <= 0x2f)
    n++;

  return ctrl_skip_params (d, n); // Não é usado em closed caption, apenas em superimpose.
}

/*
 * APS (0x1c): Active Position Set
 * sintaxe: [aps] [P1] [P2]
 *
 * A partir da primeira posição da área de exibição, desce P1 linhas e avança
 * P2 colunas. Cada parâmetro ocupa 1 byte entre 0x40 e 0x7f, e o valor (0 a
 * 63) está nos 6 bits menos significativos (ARIB STD-B24, Volume 1, 7.2).
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

/*
 * SS3 (0x1d): Single Shift 3
 * sintaxe: [ss3] [código]
 *
 * O próximo byte é um caractere do conjunto G3 (ABNT NBR 15608-3:2008,
 * Tabela 3), convertido para UTF-8 pela tabela abaixo.
 */
struct ss3charmap_g3 {
  uint8_t g3; /* Código do caractere no conjunto G3 (ABNT NBR 15608-3:2008, Tabela 3) */
  const char *utf8; /* Caractere em UTF-8 */
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

  // A tabela é pequena e esses caracteres são raros: uma busca linear basta.
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

  DTV_DEBUG ("Caractere G3 (0x%x) desconhecido; veja a ABNT NBR 15608-3:2008, Tabela 3.", argv[1]);

  return ctrl_skip_params (d, 1);
}

// RS (0x1e): Data Separator
bool callback_controls_c0_RS (const CC_ControlCodes_CallbackData_t UNUSED(*d))
{
  return FALSE; // Não é usado em closed caption, apenas em superimpose.
}

// US (0x1f): Unit Separator
bool callback_controls_c0_US (const CC_ControlCodes_CallbackData_t UNUSED(*d))
{
//const uint8_t *argv = &d->argv[*d->pos];

  return TRUE;
}

/* </C0> */

/*
 * Callbacks dos comandos C1, a partir de 0x80 (BKF).
 */
static
const CaptionClosedControls_t callbacks_ctrls_c1[] = {
  // Cor do texto
  CAPTION_CTRL_SET(CTRL_C1_BKF, callback_controls_c1_BKF), // array[0] (0x80)
  CAPTION_CTRL_SET(CTRL_C1_RDF, callback_controls_c1_RDF),
  CAPTION_CTRL_SET(CTRL_C1_GRF, callback_controls_c1_GRF),
  CAPTION_CTRL_SET(CTRL_C1_YLF, callback_controls_c1_YLF),
  CAPTION_CTRL_SET(CTRL_C1_BLF, callback_controls_c1_BLF),
  CAPTION_CTRL_SET(CTRL_C1_MGF, callback_controls_c1_MGF),
  CAPTION_CTRL_SET(CTRL_C1_CNF, callback_controls_c1_CNF),
  CAPTION_CTRL_SET(CTRL_C1_WHF, callback_controls_c1_WHF),

  // Tamanho dos caracteres
  CAPTION_CTRL_SET(CTRL_C1_SSZ, callback_controls_c1_SSZ),
  CAPTION_CTRL_SET(CTRL_C1_MSZ, callback_controls_c1_MSZ),
  CAPTION_CTRL_SET(CTRL_C1_NSZ, callback_controls_c1_NSZ),
  CAPTION_CTRL_SET(CTRL_C1_SZX, callback_controls_c1_SZX),
  CAPTION_CTRL_SET(CTRL_C1_PAD_0x8c, NULL), // <--+
  CAPTION_CTRL_SET(CTRL_C1_PAD_0x8d, NULL), //    +--> sem uso (0x8c - 0x8f)
  CAPTION_CTRL_SET(CTRL_C1_PAD_0x8e, NULL), //    |
  CAPTION_CTRL_SET(CTRL_C1_PAD_0x8f, NULL), // <--+
  CAPTION_CTRL_SET(CTRL_C1_COL, callback_controls_c1_COL),

  //
  CAPTION_CTRL_SET(CTRL_C1_FLC, callback_controls_c1_FLC),
  CAPTION_CTRL_SET(CTRL_C1_CDC, callback_controls_c1_CDC),
  CAPTION_CTRL_SET(CTRL_C1_POL, callback_controls_c1_POL),
  CAPTION_CTRL_SET(CTRL_C1_WMM, callback_controls_c1_WMM),
  CAPTION_CTRL_SET(CTRL_C1_MAC, callback_controls_c1_MAC),

  CAPTION_CTRL_SET(CTRL_C1_PAD_0x96, NULL), // sem uso
  CAPTION_CTRL_SET(CTRL_C1_HLC, callback_controls_c1_HLC),
  CAPTION_CTRL_SET(CTRL_C1_RPC, callback_controls_c1_RPC),
  CAPTION_CTRL_SET(CTRL_C1_SPL, callback_controls_c1_SPL),
  CAPTION_CTRL_SET(CTRL_C1_STL, callback_controls_c1_STL),

  // Control Sequence Introducer
  CAPTION_CTRL_SET(CTRL_C1_CSI, callback_controls_c1_CSI_load),

  CAPTION_CTRL_SET(CTRL_C1_PAD_0x9c, NULL), // sem uso

  CAPTION_CTRL_SET(CTRL_C1_TIM, callback_controls_c1_TIM),

  CAPTION_CTRL_SET(CTRL_C1_PAD_0x9e, NULL), // sem uso
  CAPTION_CTRL_SET(CTRL_C1_PAD_0x9f, NULL)  // sem uso
};

/* <C1> */

// BKF (0x80): Black Foreground Color
// sintaxe: [bkf]
bool
callback_controls_c1_BKF (const CC_ControlCodes_CallbackData_t *d)
{
  return caption_closed_controls_cmlat2rgb (SET_CMLA0_BLACK, &d->caption->foreground);
}

// RDF (0x81): Red Foreground Color
// sintaxe: [rdf]
bool
callback_controls_c1_RDF (const CC_ControlCodes_CallbackData_t *d)
{
  return caption_closed_controls_cmlat2rgb (SET_CMLA1_FULL_INTENSITY_RED, &d->caption->foreground);
}

// GRF (0x82): Green Foreground Color
// sintaxe: [grf]
bool
callback_controls_c1_GRF (const CC_ControlCodes_CallbackData_t *d)
{
  return caption_closed_controls_cmlat2rgb (SET_CMLA2_FULL_INTENSITY_GREEN, &d->caption->foreground);
}

// YLF (0x83): Yellow Foreground Color
// sintaxe: [ylf]
bool
callback_controls_c1_YLF (const CC_ControlCodes_CallbackData_t *d)
{
  return caption_closed_controls_cmlat2rgb (SET_CMLA3_FULL_INTENSITY_YELLOW, &d->caption->foreground);
}

// BLF (0x84): Blue Foreground Color
// sintaxe: [blf]
bool
callback_controls_c1_BLF (const CC_ControlCodes_CallbackData_t *d)
{
  return caption_closed_controls_cmlat2rgb (SET_CMLA4_FULL_INTENSITY_BLUE, &d->caption->foreground);
}

// MGF (0x85): Magenta Foreground Color
// sintaxe: [mgf]
bool
callback_controls_c1_MGF (const CC_ControlCodes_CallbackData_t *d)
{
  return caption_closed_controls_cmlat2rgb (SET_CMLA5_FULL_INTENSITY_MAGNETA, &d->caption->foreground);
}

// CNF (0x86): Cyan Foreground Color
// sintaxe: [cnf]
bool
callback_controls_c1_CNF (const CC_ControlCodes_CallbackData_t *d)
{
  return caption_closed_controls_cmlat2rgb (SET_CMLA6_FULL_INTENSITY_CYAN, &d->caption->foreground);
}

// WHF (0x87): White Foreground Format
// sintaxe: [whf]
bool
callback_controls_c1_WHF (const CC_ControlCodes_CallbackData_t *d)
{
  return caption_closed_controls_cmlat2rgb (SET_CMLA7_FULL_INTENSITY_WHITE, &d->caption->foreground);
}

// SSZ (0x88): Small Size
// sintaxe: [ssz]
bool
callback_controls_c1_SSZ (const CC_ControlCodes_CallbackData_t *d)
{
  d->caption->char_size = CC_CharacterSize_Small;

  return TRUE;
}

// MSZ (0x89): Middle Size
// sintaxe: [msz]
bool
callback_controls_c1_MSZ (const CC_ControlCodes_CallbackData_t *d)
{
  d->caption->char_size = CC_CharacterSize_Middle;

  return TRUE;
}

// NSZ (0x8a): Normal Size
// sintaxe: [nsz]
bool
callback_controls_c1_NSZ (const CC_ControlCodes_CallbackData_t *d)
{
  d->caption->char_size = CC_CharacterSize_Standard;

  return TRUE;
}

// SZX (0x8b): Character Size Controls
// sintaxe: [szx] [tamanho]
bool
callback_controls_c1_SZX (const CC_ControlCodes_CallbackData_t *d)
{
  const uint8_t *argv = &d->argv[*d->pos];

  if (!CTRL_HAS_PARAMS (d, 1))
    return ctrl_skip_params (d, 1);

  d->caption->char_size_custom = argv[1];
  *d->pos += 1;

  return TRUE; // Não é usado em closed caption, apenas em superimpose.
}

// COL (0x90): Colour Controls
// sintaxe: [col] [cor]
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
     DTV_DEBUG ("Valor de cor (0x%02x) inválido no comando COL.", argv[1]);
     return ctrl_skip_params (d, (argv[1] == 0x20) ? 2 : 1);
   }
   else
   {
     if (argv[1] == 0x20) // [col] [0x20] [P2]: cor da paleta indicada por P2
     {
       // TODO: a norma não é muito clara neste caso (ARIB STD-B24, Tabela 7-16,
       // comando COL); aqui P2 (0x40 a 0x4f) é tratado como a cor do texto.
       uint8_t cmla = (argv[2] - 0x40); // índice na paleta (0 a 15)

       *d->pos += 2;

       return caption_closed_controls_cmlat2rgb (cmla, &d->caption->foreground);
     }
     else
     {
       CC_Color_t *color = NULL;
       uint8_t cmla = argv[1];

       // Na ordem da ARIB STD-B24, Tabela 7-16 (comando COL)
       switch (cmla)
       {
         case 0x48 ... 0x4f: // cor do texto
           color = &d->caption->foreground;
           cmla -= 0x40;
         break;

         case 0x50 ... 0x5f: // cor de fundo
           color = &d->caption->background;
           cmla -= 0x50;
         break;

         case 0x60 ... 0x6f: // cor do texto, meia intensidade
           color = &d->caption->half_foreground;
           cmla -= 0x60;
         break;

         case 0x70 ... 0x7f: // cor de fundo, meia intensidade
           color = &d->caption->half_background;
           cmla -= 0x70;
         break;

         default:

           DTV_DEBUG ("Cor (0x%02x) inválida no comando COL.", cmla);
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
  return ctrl_skip_params (d, 1); // Não é usado em closed caption, apenas em superimpose.
}

// sintaxe: [cdc] [P1] ou [cdc] [0x20] [P1]
bool
callback_controls_c1_CDC (const CC_ControlCodes_CallbackData_t *d)
{
  bool two = CTRL_HAS_PARAMS (d, 1) && d->argv[*d->pos + 1] == 0x20;

  return ctrl_skip_params (d, two ? 2 : 1); // Não é usado em closed caption, apenas em superimpose.
}

bool
callback_controls_c1_POL (const CC_ControlCodes_CallbackData_t *d)
{
  return ctrl_skip_params (d, 1); // Não é usado em closed caption, apenas em superimpose.
}

bool
callback_controls_c1_WMM (const CC_ControlCodes_CallbackData_t *d)
{
  return ctrl_skip_params (d, 1); // Não é usado em closed caption, apenas em superimpose.
}

bool
callback_controls_c1_MAC (const CC_ControlCodes_CallbackData_t *d)
{
  return ctrl_skip_params (d, 1); // Não é usado em closed caption, apenas em superimpose.
}

bool
callback_controls_c1_HLC (const CC_ControlCodes_CallbackData_t *d)
{
  return ctrl_skip_params (d, 1); // Não é usado em closed caption, apenas em superimpose.
}

bool
callback_controls_c1_RPC (const CC_ControlCodes_CallbackData_t *d)
{
  return ctrl_skip_params (d, 1); // Não é usado em closed caption, apenas em superimpose.
}

bool
callback_controls_c1_SPL (const CC_ControlCodes_CallbackData_t UNUSED(*d))
{
  return FALSE; // Não é usado em closed caption, apenas em superimpose.
}

bool
callback_controls_c1_STL (const CC_ControlCodes_CallbackData_t UNUSED(*d))
{
  return FALSE; // Não é usado em closed caption, apenas em superimpose.
}

// sintaxe: [time] [0x20] [P1]
bool
callback_controls_c1_TIM (const CC_ControlCodes_CallbackData_t *d)
{
  return ctrl_skip_params (d, 2); // Não é usado em closed caption, apenas em superimpose.
}

/*
 * Os parâmetros Pn da norma ficam em Pn->argv[n - 1]: o P2, por exemplo, está
 * em argv[1].
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
      // Base 10: os parâmetros são decimais e podem ter zeros à esquerda (ex: "08").
      unsigned long v = strtoul (&buf[0], NULL, 10);

      Pn->argv[Pn->argc] = (v > UINT16_MAX) ? UINT16_MAX : (uint16_t)v;
      Pn->argc += 1;
      buf_n = 0;
    }
  }

  return (Pn->argc > 0);
}

/*
 * Callbacks dos subcomandos do CSI, a partir de 0x53 (SWF).
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

  // Procura o byte intermediário (0x20) seguido do byte final, a partir de
  // argv[1] (argv[0] é o próprio CSI, 0x9b).
  for (i = 1; i < (d->argc - 1); i++)
  {
    if (argv[i] == CTRL_CSI_FB)
    {
      const uint8_t cmd = argv[i+1];
      const uint8_t cmd_indice = (cmd - CTRL_C1_CSI_SWF); // índice do subcomando
      bool (*callback)(const CC_ControlCodes_CallbackData_t*) = NULL;

      if (cmd_indice >= CALLBACKS_CONTROLS_C1_CSI_LENGTH ||
              (callback = callbacks_ctrls_c1_csi[cmd_indice].callback) == NULL)
      {
        DTV_DEBUG("Subcomando CSI (0x%02x) não suportado.", cmd);

        // Pula toda a sequência, para não ser tratada como texto.
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
          .argv = &argv[1], // pula o CSI (argv[0])
          .argc = (i - 1),  // apenas os parâmetros, sem o 0x20 e o byte final
          .pos = NULL,
          .caption = d->caption,
          .data = &csi_pn,
          .current_code = 0
        };
        bool ret = FALSE;

        DTV_DEBUG("  [+] Processando o subcomando CSI::%s hex(0x%02x)",
            callbacks_ctrls_c1_csi[cmd_indice].code_name, cmd);

        if (!csi_arg_Pn_load (&arg_subset))
        {
          DTV_DEBUG("  [!!] CSI::%s hex(0x%02x) sem parâmetros.",
              callbacks_ctrls_c1_csi[cmd_indice].code_name, cmd);
        }

        // 1) Avança até o byte final da sequência
        *d->pos += i + 1;

        // 2) Executa o subcomando
        ret = callback (&arg_subset);
        if (!ret)
        {
          DTV_DEBUG("Falha ao processar CSI::%s hex(0x%02x)",
              callbacks_ctrls_c1_csi[cmd_indice].code_name, cmd);
          DTV_HEXDUMP (arg_subset.argv, arg_subset.argc);
        }

        *current_code = cmd; // o hook recebe o código do subcomando
        return ret;
      }
    }
  }

  // Sequência incompleta (sem o byte final): descarta até o final do buffer.
  return ctrl_skip_params (d, d->argc);
}

/*
 * CSI SWF (0x53): Set Writing Format
 * sintaxe: CSI P1 [; P2 ; P3 ; P4] 0x20 0x53
 *
 * P1: formato de exibição (veja CC_Format_t). Com mais parâmetros, P2 é o
 * tamanho dos caracteres, P3 a quantidade de caracteres por linha e P4 a
 * quantidade de linhas.
 */
bool
callback_controls_c1_CSI_swf (const CC_ControlCodes_CallbackData_t *d)
{
  struct csi_arg_Pn *Pn = d->data;
  uint16_t *P = Pn->argv;
  CaptionClosed_t *c = d->caption;

  switch (Pn->argc) // Quantos 'P'?
  {
    case 1: // apenas P1: o formato
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

      // P3 e P4 (zerados quando não informados)
      c->char_number_in_oneline = P[2];
      c->char_line_number = P[3];
    }
    break;

    default:
    {
      // TODO: a norma é pouco clara para outras quantidades de parâmetros e não
      // há amostras de legenda com esse caso.
      DTV_DEBUG("Quantidade de parâmetros (%d) não esperada no CSI SWF.", Pn->argc);
      return FALSE;
    }
  }

  return TRUE;
}

// CSI SSM (0x57): Character composition dot designation
// sintaxe: CSI P11 ~ P1i I1 P21 ~ P2j I2 F
bool
callback_controls_c1_CSI_ssm (const CC_ControlCodes_CallbackData_t *d)
{
  struct csi_arg_Pn *Pn = d->data;
  uint16_t *P = Pn->argv;

  d->caption->SSM.width  = P[0]; // P1
  d->caption->SSM.height = P[1]; // P2

  return TRUE;
}

// CSI SHS (0x58): Set Horizontal Spacing
// sintaxe: CSI P11 ~ P1iI1F
bool
callback_controls_c1_CSI_shs (const CC_ControlCodes_CallbackData_t *d)
{
  struct csi_arg_Pn *Pn = d->data;
  uint16_t *P = Pn->argv;

  d->caption->SHS = P[0]; // P1

  return TRUE;
}

// CSI SVS (0x59): Set Vertical Spacing
// sintaxe: CSI P11 ~ P1I I1 F
bool
callback_controls_c1_CSI_svs (const CC_ControlCodes_CallbackData_t *d)
{
  struct csi_arg_Pn *Pn = d->data;
  uint16_t *P = Pn->argv;

  d->caption->SVS = P[0]; // P1

  return TRUE;
}

/*
 * CSI SDF (0x56): Set Display Format
 * sintaxe: CSI P1 ; P2 0x20 0x56
 *
 * Tamanho da área de exibição: P1 pontos na horizontal e P2 na vertical.
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

// CSI SDP (0x5f): Set Display Position
// sintaxe: CSI P11 ~ P1i I1 P21 ~ P2j I2F
bool
callback_controls_c1_CSI_sdp (const CC_ControlCodes_CallbackData_t *d)
{
  struct csi_arg_Pn *Pn = d->data;
  uint16_t *P = Pn->argv;

  d->caption->SDP.width = P[0]; // P1
  d->caption->SDP.height   = P[1]; // P2

  return TRUE;
}

/*
 * CSI ORN (0x63): Ornament Control
 * sintaxe: CSI P1 ; P21 P22 P23 P24 0x20 0x63
 *
 * P1: 0 sem ornamento, 1 contorno, 2 sombra, 3 vazado. P21 P22 e P23 P24 são
 * os 4 bits mais e menos significativos da cor do ornamento na paleta.
 */
bool
callback_controls_c1_CSI_orn (const CC_ControlCodes_CallbackData_t UNUSED(*d))
{
//struct csi_arg_Pn *Pn = d->data;
//uint16_t *P = Pn->argv;
//CaptionClosed_t *c = d->caption;

  return FALSE; // Não implementado
}

// CSI CFS (0x65): Character Font Set
// sintaxe: CSI P11 ~ P1i I1 F
bool
callback_controls_c1_CSI_cfs (const CC_ControlCodes_CallbackData_t UNUSED(*d))
{
//struct csi_arg_Pn *Pn = d->data;
//uint16_t *P = Pn->argv;
//CaptionClosed_t *c = d->caption;

  return FALSE; // Não implementado
}

// CSI RCS (0x6e): Raster Colour Command
// sintaxe: CSI P11 ~ P1i IF
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

/*
 * Ponto de entrada dos comandos de controle: encontra a callback do comando,
 * executa e, se der certo, chama o hook de controles.
 */
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

  // As tabelas de callbacks têm uma entrada para cada código (com 'PAD' nos
  // códigos sem uso), então o código menos o início da tabela é o índice.
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

    if (ret)
    {
      caption_hooking_priv_run_controls (d.caption, d.current_code);
    }

    if (!ret)
    {
      DTV_DEBUG("[-] O Callback '%s'(0x%02x) retornou %s",
       caption_closed_controls_get_name (code), code, PRINT_BOOL(ret));
      DTV_DEBUG ("Comando inválido ou não suportado:");
      DTV_HEXDUMP (&d.argv[*pos], d.argc);
    }

    return ret;
  }

  DTV_DEBUG("Não existe callback para (%s) (0x%02x) (tabela 0x%02x)",
      caption_closed_controls_get_name (code), code, table);

  return FALSE;
}

/* Usado nas mensagens de depuração. */
const char *
caption_closed_controls_get_name (const uint8_t code)
{
  // Os subcomandos do CSI (0x53 ~ 0x6e) coincidem com caracteres, então só
  // são consultados quando o código não é um C0/C1.
  if (code >= CC_CTRLCODE_SET_C0 && code < CC_CTRLCODE_SET_C0 + ARRAY_SIZE (callbacks_ctrls_c0))
    return callbacks_ctrls_c0[code - CC_CTRLCODE_SET_C0].code_name;

  if (code >= CC_CTRLCODE_SET_C1 && code < CC_CTRLCODE_SET_C1 + ARRAY_SIZE (callbacks_ctrls_c1))
    return callbacks_ctrls_c1[code - CC_CTRLCODE_SET_C1].code_name;

  if (code >= CTRL_C1_CSI_SWF && code < CTRL_C1_CSI_SWF + ARRAY_SIZE (callbacks_ctrls_c1_csi))
    return callbacks_ctrls_c1_csi[code - CTRL_C1_CSI_SWF].code_name;

  DTV_DEBUG("Código (0x%02x) não encontrado.", code);

  return "UNKNOWN";
}
