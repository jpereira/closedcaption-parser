/**
 * @file caption-superimpose-geometric.c
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @date Thu Aug 12 13:49:21 BRT 2010
 * @brief Data unit de gráficos geométricos: separa os comandos.
 */
#include "caption-superimpose-geometric.h"

const char *
caption_superimpose_geometric_get_name (uint8_t opcode)
{
  switch (opcode)
  {
    case CAPTION_GEOMETRIC_OPCODE_RESET:        return "RESET";
    case CAPTION_GEOMETRIC_OPCODE_DOMAIN:       return "DOMAIN";
    case CAPTION_GEOMETRIC_OPCODE_TEXTURE:      return "TEXTURE";
    case CAPTION_GEOMETRIC_OPCODE_SET_COLOR:    return "SET COLOR";
    case CAPTION_GEOMETRIC_OPCODE_SET_PATTERN:  return "SET PATTERN";
    case CAPTION_GEOMETRIC_OPCODE_SELECT_COLOR: return "SELECT COLOR";
    case CAPTION_GEOMETRIC_OPCODE_BLINK:        return "BLINK";
  }

  // Figura 8-3 (pág. 121): 4 opcodes por comando
  if (opcode >= 0x24 && opcode <= 0x27) return "POINT";
  if (opcode >= 0x28 && opcode <= 0x2b) return "LINE";
  if (opcode >= 0x2c && opcode <= 0x2f) return "ARC";
  if (opcode >= 0x30 && opcode <= 0x33) return "RECT";
  if (opcode >= 0x34 && opcode <= 0x37) return "POLY";

  return "UNKNOWN";
}

// Fecha o comando que estava sendo montado, copiando só os bytes de operando.
static bool
command_done (CaptionSuperImposeGeometric_t *obj,
              const uint8_t *operands,
              size_t len)
{
  size_t i, n = 0;

  for (i = 0; i < len; i++)
    if (operands[i] >= 0x40 && operands[i] <= 0x7f)
      n++;

  if (n == 0)
    return TRUE;

  obj->operands = malloc (n);
  if (obj->operands == NULL)
    return FALSE;

  for (i = 0, n = 0; i < len; i++)
    if (operands[i] >= 0x40 && operands[i] <= 0x7f)
      obj->operands[n++] = operands[i];

  obj->operands_len = n;

  return TRUE;
}

CaptionSuperImposeGeometric_t *
caption_superimpose_geometric_new (const CaptionManagement_t *m)
{
  CaptionSuperImposeGeometric_t *head = NULL, **tail = &head, *cur = NULL;
  const uint8_t *data;
  uint32_t len, pos, start = 0;

  assert (m != NULL);
  assert (m->statement.unit_data != NULL);
  assert (m->statement.unit_size > 0);

  data = m->statement.unit_data;
  len  = m->statement.unit_size;

#ifdef DEBUG_CAPTION_SUPERIMPOSE_GEOMETRIC
  DTV_HEXDUMP (data, len);
#endif

  // Opcode: 0x20 - 0x3f; operando: 0x40 - 0x7f (b7 = 1). O resto (C0/C1) é ignorado.
  for (pos = 0; pos < len; pos++)
  {
    if (data[pos] >= 0x20 && data[pos] <= 0x3f)
    {
      if (cur != NULL && !command_done (cur, &data[start], pos - start))
        goto fail;

      cur = calloc (1, sizeof (CaptionSuperImposeGeometric_t));
      if (cur == NULL)
        goto fail;

      cur->opcode = data[pos];
      cur->pts    = m->pts; // apenas referência
      *tail = cur;
      tail  = &cur->next;
      start = pos + 1;
    }
  }

  if (cur != NULL && !command_done (cur, &data[start], pos - start))
    goto fail;

  return head;

fail:
  caption_superimpose_geometric_free (head);
  return NULL;
}

void
caption_superimpose_geometric_free (CaptionSuperImposeGeometric_t *cc)
{
  CaptionSuperImposeGeometric_t *obj = cc;

  while (obj != NULL)
  {
    CaptionSuperImposeGeometric_t *next = obj->next;

    free (obj->operands);
    free (obj);

    obj = next;
  }
}
