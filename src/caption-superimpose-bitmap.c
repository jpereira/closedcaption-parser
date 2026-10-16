/**
 * @file caption-superimpose-bitmap.c
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @date Thu Aug 12 13:49:21 BRT 2010
 * @brief Data unit de bitmap (PNG com posição e cores que piscam).
 */
#include "caption-superimpose-bitmap.h"

static const uint8_t png_signature[8] = { 0x89, 'P', 'N', 'G', 0x0d, 0x0a, 0x1a, 0x0a };

// Assinatura (8) + tamanho do chunk (4) + "IHDR" (4) + largura (4) + altura (4)
#define PNG_HEADER_LEN 24

CaptionSuperImposeBitmap_t *
caption_superimpose_bitmap_new (const CaptionManagement_t *m)
{
  CaptionSuperImposeBitmap_t *obj;
  const uint8_t *data, *png;
  uint32_t len, pos = 0;
  uint8_t nflc;

  assert (m != NULL);
  assert (m->statement.unit_data != NULL);
  assert (m->statement.unit_size > 0);

  data = m->statement.unit_data;
  len  = m->statement.unit_size;

#ifdef DEBUG_CAPTION_SUPERIMPOSE_BITMAP
  DTV_HEXDUMP (data, len);
#endif

  // bitmap_data (ARIB STD-B24, Volume 1, Part 3, 6.2, Tabela 6-1, pág. 156): position_header + flc_header + PNG
  if (len < 5)
    return NULL;

  nflc = data[4];
  pos  = 5;

  if (pos + nflc > len || len - pos - nflc < PNG_HEADER_LEN)
    return NULL;

  png = &data[pos + nflc];

  if (memcmp (png, png_signature, sizeof (png_signature)) != 0 || memcmp (&png[12], "IHDR", 4) != 0)
    return NULL;

  obj = calloc (1, sizeof (CaptionSuperImposeBitmap_t));
  if (obj == NULL)
    return NULL;

  obj->x = (int16_t)(data[0] << 8 | data[1]);
  obj->y = (int16_t)(data[2] << 8 | data[3]);
  obj->num_flc_colors = nflc;
  obj->pts = m->pts; // apenas referência

  obj->png_len = len - pos - nflc;
  obj->width   = (uint32_t)png[16] << 24 | png[17] << 16 | png[18] << 8 | png[19];
  obj->height  = (uint32_t)png[20] << 24 | png[21] << 16 | png[22] << 8 | png[23];

  obj->png = malloc (obj->png_len);
  if (nflc > 0)
    obj->flc_colors = malloc (nflc);

  if (obj->png == NULL || (nflc > 0 && obj->flc_colors == NULL))
  {
    caption_superimpose_bitmap_free (obj);
    return NULL;
  }

  memcpy (obj->png, png, obj->png_len);
  if (nflc > 0)
    memcpy (obj->flc_colors, &data[pos], nflc);

  return obj;
}

void
caption_superimpose_bitmap_free (CaptionSuperImposeBitmap_t *cc)
{
  CaptionSuperImposeBitmap_t *obj = cc;

  while (obj != NULL)
  {
    CaptionSuperImposeBitmap_t *next = obj->next;

    free (obj->flc_colors);
    free (obj->png);
    free (obj);

    obj = next;
  }
}
