/**
 * @file caption-superimpose-2bytedrcs.c
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @date Thu Aug 12 13:49:21 BRT 2010
 * @brief Data unit de DRCS de 2 bytes.
 */
#include "caption-superimpose-2bytedrcs.h"

// A sintaxe é a mesma do DRCS de 1 byte (ARIB STD-B24, Volume 1, Part 2, Annex D).
CaptionSuperImpose2ByteDrcs_t *
caption_superimpose_2bytedrcs_new (const CaptionManagement_t *m)
{
#ifdef DEBUG_CAPTION_SUPERIMPOSE_2BYTEDRCS
  DTV_HEXDUMP (m->statement.unit_data, m->statement.unit_size);
#endif

  return caption_superimpose_1bytedrcs_new (m);
}

void
caption_superimpose_2bytedrcs_free (CaptionSuperImpose2ByteDrcs_t *cc)
{
  caption_superimpose_1bytedrcs_free (cc);
}
