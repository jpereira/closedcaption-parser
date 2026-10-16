/**
 * @file caption.h
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @date Thu Aug 12 13:49:21 BRT 2010
 * @brief Header principal: inclui todos os módulos da biblioteca.
 *
 * <b>Visão geral do processamento</b>
 *
 * @verbatim
 *  arquivo .ts
 *     |  ts_reader_next()                       (ts-reader.h)
 *     v
 *  pacotes TS de 188 bytes
 *     |  ts_demux_push()                        (ts-demux.h)
 *     v
 *  PES completo do PID da legenda
 *     |  caption_screen_push_pes()              (caption-screen.h)
 *     |    packet_pes_load()                    (packet-pes.h)
 *     |    caption_data_load()                  (caption-data.h)
 *     |    caption_management_load()            (caption-management.h)
 *     |    caption_closed_run()                 (caption-closed.h)
 *     |      -> hooks de texto e de controle    (caption-hooking.h)
 *     v
 *  linhas de legenda em UTF-8
 * @endverbatim
 *
 * Quem já tem o próprio demux pode chamar caption_screen_push_pes() com os
 * PES da legenda, ou usar diretamente as funções de baixo nível com os hooks
 * de caption-hooking.h.
 */
#ifndef CAPTION_H_
#define CAPTION_H_

#include "caption-version.h"
#include "caption-common.h"

#include "packet-pes.h"
#include "caption-data.h"
#include "caption-management.h"
#include "caption-hooking.h"

#include "caption-closed.h"
#include "caption-closed-controls.h"
#include "caption-superimpose.h"
#include "caption-superimpose-1bytedrcs.h"
#include "caption-superimpose-2bytedrcs.h"
#include "caption-superimpose-bitmap.h"
#include "caption-superimpose-colormap.h"
#include "caption-superimpose-geometric.h"
#include "caption-superimpose-sound.h"

#include "caption-screen.h"
#include "ts-reader.h"
#include "ts-demux.h"

#ifndef DEBUG_CLOSEDCAPTION
# define Dump_CaptionClosed_t(cc)
# define Dump_CaptionManagement_t(m)
#else
/**
 * Imprime todos os campos de um CaptionClosed_t no stdout.
 * Só existe com @c -DDEBUG_CLOSEDCAPTION.
 */
# define Dump_CaptionClosed_t(cc)     __Dump_CaptionClosed_t(__FILE__, __LINE__, __func__, cc)
/**
 * Imprime todos os campos de um CaptionManagement_t no stdout.
 * Só existe com @c -DDEBUG_CLOSEDCAPTION.
 */
# define Dump_CaptionManagement_t(m)  __Dump_CaptionManagement_t(__FILE__, __LINE__, __func__, m)

/** @cond INTERNO */
static inline const char *
__Dump_basename (const char *path)
{
  const char *file = strrchr (path, '/');

  return (file != NULL) ? (file + 1) : path;
}

static inline void
__Dump_CaptionManagement_t (const char *file,
                            uint32_t line,
                            const char *func,
                            const CaptionManagement_t *m)
{
#define get_data(x) ((m->statement.unit_size > (x)) ? m->statement.unit_data[x] : 0x00)
  const uint8_t *l = (const uint8_t *)&m->ISO_639_lang_code[0];
  uint32_t iso_639_lang = (l[0] << 16 | l[1] << 8 | l[2]);

  printf ("<Dump_CaptionManagement_t() from %s:%u in %s()>\n"
         " struct CaptionManagement_t {\n"
         "   TMD = %d,\n"
         "   TMD_otm = %#llx,\n"
         "   reserved = %d (0x%02x),\n"
         "   num_lang = %d (0x%02x),\n"
         "   DMF = 0x%02x,\n"
         "   DC = 0x%02x,\n"
         "   ISO_639_lang_code = '%s' (0x%06x),\n"
         "   format = 0x%x,\n"
         "   TCS = 0x%02x,\n"
         "   rollup_mode = 0x%02x\n"
         "   struct statement {\n"
         "     unit_sep = %d (0x%02x)\n"
         "     unit_param = %d (0x%02x)\n"
         "     unit_size = %u (0x%06x),\n"
         "     unit_data = { [0]='0x%02x', [1]='0x%02x', [2]='0x%02x', [3]='0x%02x', ... },\n"
         "   };\n"
         " };\n"
         "<Dump_CaptionManagement_t>\n",
         __Dump_basename (file), line, func,
         m->TMD, (unsigned long long)m->TMD_otm, m->reserved, m->reserved, m->num_lang, m->num_lang, m->DMF,
         m->DC, m->ISO_639_lang_code, iso_639_lang, m->format, m->TCS, m->rollup_mode,
         m->statement.unit_sep, m->statement.unit_sep,
         m->statement.unit_param, m->statement.unit_param,
         m->statement.unit_size, m->statement.unit_size,
         get_data(0), get_data(1), get_data(2), get_data(3)
  );
#undef get_data
}

static inline void
__Dump_CaptionClosed_t (const char *file,
                        uint32_t line,
                        const char *func,
                        const CaptionClosed_t *cc)
{
  printf ("<Dump_CaptionClosed_t() from %s:%u in %s()>\n"
          " struct CaptionClosed_t {\n"
          "   CLS { need_clear = %s },\n"
          "   SWF {\n"
          "     format = %d (0x%02x)\n"
          "     char_size = %d (0x%02x)\n"
          "     char_number_in_oneline = %d (0x%02x)\n"
          "     char_line_number = %d (0x%02x)\n"
          "   };\n"
          "   struct SDF { width='%d' (0x%04x), height='%d' (0x%04x) };\n"
          "   struct SDP { width='%d' (0x%04x), height='%d' (0x%04x) };\n"
          "   struct SSM { width='%d' (0x%04x), height='%d' (0x%04x) };\n"
          "   struct APS { first='%d' (0x%02x), second='%d' (0x%02x) };\n"
          "   CC_Rcs_t RCS = %d,\n"
          "   SHS = %d (0x%02x),\n"
          "   SVS = %d (0x%02x),\n"
          "   CC_Color_t::foreground = { r=0x%02x, g=0x%02x, b=0x%02x, a=0x%02x },\n"
          "   CC_Color_t::background = { r=0x%02x, g=0x%02x, b=0x%02x, a=0x%02x },\n"
          "   CC_Color_t::half_background = { r=0x%02x, g=0x%02x, b=0x%02x, a=0x%02x },\n"
          "   CC_Color_t::half_foreground = { r=0x%02x, g=0x%02x, b=0x%02x, a=0x%02x },\n"
          " };\n",
      __Dump_basename (file), line, func,
      cc->need_clear ? "TRUE" : "FALSE",
      cc->format, cc->format,
      cc->char_size, cc->char_size,
      cc->char_number_in_oneline, cc->char_number_in_oneline,
      cc->char_line_number, cc->char_line_number,
      cc->SDF.width, cc->SDF.width, cc->SDF.height, cc->SDF.height,
      cc->SDP.width, cc->SDP.width, cc->SDP.height, cc->SDP.height,
      cc->SSM.width, cc->SSM.width, cc->SSM.height, cc->SSM.height,
      cc->APS.first, cc->APS.first, cc->APS.second, cc->APS.second,
      cc->RCS, cc->SHS, cc->SHS, cc->SVS, cc->SVS,
      cc->foreground.r, cc->foreground.g, cc->foreground.b, cc->foreground.a,
      cc->background.r, cc->background.g, cc->background.b, cc->background.a,
      cc->half_background.r, cc->half_background.g, cc->half_background.b, cc->half_background.a,
      cc->half_foreground.r, cc->half_foreground.g, cc->half_foreground.b, cc->half_foreground.a);

  if (cc->pts != NULL)
  {
    printf (" struct pts {\n"
            "   h=%d, m=%d, s=%d, u=%d, ts=%u // [90 kHz-Timestamp: %d:%02d:%02d.%04d]\n"
            " };\n", cc->pts->h, cc->pts->m, cc->pts->s, cc->pts->u, cc->pts->ts, cc->pts->h,
            cc->pts->m, cc->pts->s, cc->pts->u);
  }
  else
  {
    printf (" struct pts {\n   indisponível\n };\n");
  }

  printf ("</Dump_CaptionClosed_t()>\n");
}
/** @endcond */
#endif /* DEBUG_CLOSEDCAPTION */

#endif /* CAPTION_H_ */
