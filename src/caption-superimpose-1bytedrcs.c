/**
 * @file caption-superimpose-1bytedrcs.c
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @date Thu Aug 12 13:49:21 BRT 2010
 * @brief Data unit de DRCS de 1 byte (e, na prática, também a de 2 bytes).
 */
#include "caption-superimpose-1bytedrcs.h"

// Bits por pixel: o menor n tal que 2^n >= depth + 2 (número de gradações).
static unsigned
bits_per_pixel (uint8_t depth)
{
  unsigned bpp = 1;

  while ((1u << bpp) < (unsigned)depth + 2)
    bpp++;

  return bpp;
}

CaptionSuperImpose1ByteDrcs_t *
caption_superimpose_1bytedrcs_new (const CaptionManagement_t *m)
{
  CaptionSuperImpose1ByteDrcs_t *head = NULL, **tail = &head;
  const uint8_t *data;
  uint32_t len, pos = 0;
  unsigned ncode, i;

  assert (m != NULL);
  assert (m->statement.unit_data != NULL);
  assert (m->statement.unit_size > 0);

  data = m->statement.unit_data;
  len  = m->statement.unit_size;

#ifdef DEBUG_CAPTION_SUPERIMPOSE_1BYTEDRCS
  DTV_HEXDUMP (data, len);
#endif

  // Drcs_data_structure (ARIB STD-B24, Volume 1, Part 2, Annex D, Tabela D-1, pág. 134)
  ncode = data[pos++];

  for (i = 0; i < ncode; i++)
  {
    uint16_t code;
    unsigned nfont, j;

    if (pos + 3 > len)
      goto invalid;

    code  = (uint16_t)(data[pos] << 8 | data[pos+1]);
    nfont = data[pos+2];
    pos  += 3;

    for (j = 0; j < nfont; j++)
    {
      CaptionSuperImpose1ByteDrcs_t *obj;

      if (pos + 1 > len)
        goto invalid;

      obj = calloc (1, sizeof (CaptionSuperImpose1ByteDrcs_t));
      if (obj == NULL)
        goto invalid;

      *tail = obj;
      tail  = &obj->next;

      obj->character_code = code;
      obj->font_id = (data[pos] & 0xf0) >> 4;
      obj->mode    = (data[pos] & 0x0f);
      obj->pts     = m->pts; // apenas referência
      pos += 1;

      if (obj->mode == CAPTION_DRCS_MODE_2_GRADATION || obj->mode == CAPTION_DRCS_MODE_MULTI_GRAD)
      {
        uint32_t npix, nbits, k;

        if (pos + 3 > len)
          goto invalid;

        obj->depth  = data[pos];
        obj->width  = data[pos+1];
        obj->height = data[pos+2];
        pos += 3;

        npix  = (uint32_t)obj->width * obj->height;
        nbits = npix * bits_per_pixel (obj->depth);

        if (pos + (nbits + 7) / 8 > len)
          goto invalid;

        if (npix > 0)
        {
          unsigned bpp = bits_per_pixel (obj->depth);

          obj->pixels = malloc (npix);
          if (obj->pixels == NULL)
            goto invalid;

          // Pixels em sequência, sem preenchimento entre as linhas, do b8 ao b1.
          for (k = 0; k < npix; k++)
          {
            uint8_t v = 0;
            unsigned b;

            for (b = 0; b < bpp; b++)
            {
              uint32_t bit = k * bpp + b;

              v = (uint8_t)((v << 1) | ((data[pos + bit / 8] >> (7 - bit % 8)) & 1));
            }

            obj->pixels[k] = v;
          }
        }

        pos += (nbits + 7) / 8;
      }
      else
      {
        uint16_t glen;

        if (pos + 4 > len)
          goto invalid;

        obj->region_x = data[pos];
        obj->region_y = data[pos+1];
        glen = (uint16_t)(data[pos+2] << 8 | data[pos+3]);
        pos += 4;

        if (pos + glen > len)
          goto invalid;

        if (glen > 0)
        {
          obj->geometric = malloc (glen);
          if (obj->geometric == NULL)
            goto invalid;

          memcpy (obj->geometric, &data[pos], glen);
          obj->geometric_len = glen;
        }

        pos += glen;
      }
    }
  }

  return head; // NULL se não veio nenhum caractere

invalid:
  caption_superimpose_1bytedrcs_free (head);
  return NULL;
}

void
caption_superimpose_1bytedrcs_free (CaptionSuperImpose1ByteDrcs_t *cc)
{
  CaptionSuperImpose1ByteDrcs_t *obj = cc;

  while (obj != NULL)
  {
    CaptionSuperImpose1ByteDrcs_t *next = obj->next;

    free (obj->pixels);
    free (obj->geometric);
    free (obj);

    obj = next;
  }
}
