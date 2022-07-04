/**
 * @file: caption-closed.c
 * @autor: Jorge Pereira <jorge.pereira@tqtvd.com>
 * @date: Thu Aug 12 13:49:21 BRT 2010
 * @desc: Mais informações em 'arquitetura-closedcaption.txt'
 */

//#define DEBUG_CLOSEDCAPTION
#define DEBUG_CAPTION_CLOSED_CONTROLCODE
#include "caption-closed.h"
#include "caption-closed-controls.h"
#include "caption-hooking-priv.h"
#include "caption-common.h"
//#define TQTVD_ENABLE_ATVDEBUG_LOG 1
//#include "atvLog-priv.h"
#include "atvLog-mute.h"

#define CHAR_UTF8_BUFFER_LEN 8


/* Create a dynamically allocated copy of string,
 * changing the encoding from ISO-8859-15 to UTF-8.
*/
uint8_t char_latin9_to_utf8(const char *const from, char *const to, uint8_t to_len)
{
  /* Clear the tail of the string, setting the trailing NUL. */
  memset(to, 0, to_len);

	const unsigned char  *s = (const unsigned char *)from;
	unsigned char        *d = (unsigned char *)to;

	if (*s < 128) {
		*(d) = *(s);
		return 1;
	} else {
		if (*s < 192) switch (*s) {
			case 164: *(d++) = 226; *(d++) = 130; *(d) = 172; return 3;
			case 166: *(d++) = 197; *(d) = 160; return 2;
			case 168: *(d++) = 197; *(d) = 161; return 2;
			case 180: *(d++) = 197; *(d) = 189; return 2;
			case 184: *(d++) = 197; *(d) = 190; return 2;
			case 188: *(d++) = 197; *(d) = 146; return 2;
			case 189: *(d++) = 197; *(d) = 147; return 2;
			case 190: *(d++) = 197; *(d) = 184; return 2;
			default:  *(d++) = 194; *(d) = *(s); return 2;
		} else {
			*(d++) = 195;
			*(d) = *(s) - 64;
			return 2;
		}
	}
}

bool
caption_closed_run (const CaptionManagement_t *m,
                    CaptionClosed_t *cc)
{
  assert (m != NULL);
  assert (cc != NULL);
  const uint8_t *argv = m->statement.unit_data;
  const uint8_t argc = m->statement.unit_size;
  uint8_t pos = 0; // Posicao atual em argv[]
  enum CC_ControlCodes_TableJump_e table = CC_CTRLCODE_SET_UNKNOWN;

//  assert (m->pts != NULL);
//  assert (m->statement.unit_data != NULL);
//  assert (m->statement.unit_size > 0);
  
  cc->pts = m->pts; // apenas referencia
  char tmp[8];

  for (pos = 0; pos < argc; pos++)
  { 
    ATV_DEBUG("Iniciando tudo: argv[%d]=0x%02x", pos, argv[pos]);
    
     /**
      * <Conforme a norma>
      * Control code, which can be added or deleted without effecting to
      * information content.
      * </Conforme a norma>
      */
    if (argv[pos] == CTRL_C0_NULL || argv[pos] == CTRL_C0_BELL)
      continue;

    if (caption_closed_controls_is_table (&argv[pos], &table)) // Comando de controle?
    {
      ATV_DEBUG("[CONTROL] Processando ch(%c) hex(0x%02x) argv[%d]=0x%02x",
              isprint(argv[pos])?argv[pos]:'?', argv[pos], pos, argv[pos]);
      assert (argv[pos] != CTRL_CSI_FB);
      
      caption_closed_controls_load (table, argv, (argc - pos), &pos, cc);
    }
    else if (is_latin_iso8859_15 (argv[pos]))// então e um char ISO-8859-15,
    {

      ATV_DEBUG("[STRING] Copiando argv[%03d] ch(%c) hex(0x%2x) como string.",
            pos, isprint(argv[pos])?argv[pos]:'?', argv[pos]);

      uint8_t len = char_latin9_to_utf8((const char*)&argv[pos],&tmp[0],CHAR_UTF8_BUFFER_LEN);
   	  if (len > 0)
   	  {
   		  caption_hooking_priv_run_string (cc, &tmp[0], &len);
	      ATV_DEBUG("[STRING] convertido para [%s] tam=%d",&tmp[0], len);
      }
      // </rever este codigo>
    }
    else
    {
      printf("CLOSED CAPTION: Unknown character found: (0x%02x) \n", argv[pos]);
      return FALSE;
    }
  }

  return TRUE;
}

//#undef TQTVD_ENABLE_ATVDEBUG_LOG
