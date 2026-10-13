/**
 * @file caption-closed.c
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @date Thu Aug 12 13:49:21 BRT 2010
 * @brief Interpretação do texto da legenda (closed caption).
 */
#include "caption-closed.h"
#include "caption-closed-controls.h"
#include "caption-hooking-priv.h"
#include "caption-common.h"

/* Maior caractere UTF-8 gerado (3 bytes) + '\0', com folga. */
#define CHAR_UTF8_BUFFER_LEN 8

uint8_t
char_latin9_to_utf8 (const char *const from,
                     char *const to,
                     uint8_t to_len)
{
  const unsigned char *s = (const unsigned char *)from;
  unsigned char *d = (unsigned char *)to;

  // Zera o destino, garantindo o '\0' no final.
  memset (to, 0, to_len);

  // ASCII: igual no UTF-8.
  if (*s < 0x80)
  {
    d[0] = s[0];
    return 1;
  }

  // 0xa0 - 0xbf: igual ao Latin-1 (U+00A0 - U+00BF), exceto os 8 caracteres
  // que o ISO-8859-15 trocou em relação ao ISO-8859-1.
  if (*s < 0xc0)
  {
    switch (*s)
    {
      case 0xa4: d[0] = 0xe2; d[1] = 0x82; d[2] = 0xac; return 3; // € U+20AC
      case 0xa6: d[0] = 0xc5; d[1] = 0xa0; return 2;              // Š U+0160
      case 0xa8: d[0] = 0xc5; d[1] = 0xa1; return 2;              // š U+0161
      case 0xb4: d[0] = 0xc5; d[1] = 0xbd; return 2;              // Ž U+017D
      case 0xb8: d[0] = 0xc5; d[1] = 0xbe; return 2;              // ž U+017E
      case 0xbc: d[0] = 0xc5; d[1] = 0x92; return 2;              // Œ U+0152
      case 0xbd: d[0] = 0xc5; d[1] = 0x93; return 2;              // œ U+0153
      case 0xbe: d[0] = 0xc5; d[1] = 0xb8; return 2;              // Ÿ U+0178
      default:   d[0] = 0xc2; d[1] = s[0]; return 2;
    }
  }

  // 0xc0 - 0xff: igual ao Latin-1 (U+00C0 - U+00FF).
  d[0] = 0xc3;
  d[1] = s[0] - 0x40;

  return 2;
}

bool
caption_closed_run (const CaptionManagement_t *m,
                    CaptionClosed_t *cc)
{
  assert (m != NULL);
  assert (cc != NULL);
  const uint8_t *argv = m->statement.unit_data;
  // O payload de um PES nunca passa de 64KB.
  const uint16_t argc = (m->statement.unit_size > UINT16_MAX) ? UINT16_MAX : m->statement.unit_size;
  uint16_t pos = 0; // Posição atual em argv[]
  enum CC_ControlCodes_TableJump_e table = CC_CTRLCODE_SET_UNKNOWN;
  char tmp[CHAR_UTF8_BUFFER_LEN];

  cc->pts = m->pts; // apenas referência

  for (pos = 0; pos < argc; pos++)
  {
    ATV_DEBUG("Processando argv[%d]=0x%02x", pos, argv[pos]);

    // NUL e BEL podem ser incluídos ou removidos sem alterar o conteúdo
    // (ARIB TR-B14), então são ignorados.
    if (argv[pos] == CTRL_C0_NULL || argv[pos] == CTRL_C0_BELL)
      continue;

    if (caption_closed_controls_is_table (&argv[pos], &table)) // Comando de controle?
    {
      ATV_DEBUG("[CONTROLE] Processando ch(%c) hex(0x%02x) argv[%d]=0x%02x",
                isprint(argv[pos])?argv[pos]:'?', argv[pos], pos, argv[pos]);

      // A callback avança 'pos' até o último parâmetro do comando.
      caption_closed_controls_load (table, argv, (argc - pos), &pos, cc);
    }
    else if (is_latin_iso8859_15 (argv[pos])) // Caractere ISO-8859-15
    {
      uint8_t len = char_latin9_to_utf8 ((const char*)&argv[pos], &tmp[0], CHAR_UTF8_BUFFER_LEN);

      ATV_DEBUG("[TEXTO] argv[%03d] ch(%c) hex(0x%02x) convertido para [%s] tam=%d",
                pos, isprint(argv[pos])?argv[pos]:'?', argv[pos], &tmp[0], len);

      caption_hooking_priv_run_string (cc, &tmp[0], &len);
    }
    else
    {
      // Caractere fora do conjunto suportado: ignora e segue com o restante.
      ATV_DEBUG("Caractere desconhecido: (0x%02x)", argv[pos]);
    }
  }

  return TRUE;
}
