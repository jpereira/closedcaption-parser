/**
 * @file caption-hooking.c
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @date Thu Aug 12 13:49:21 BRT 2010
 * @brief Registro e chamada dos hooks de texto e de controles.
 */
#include "caption-hooking.h"
#include "caption-hooking-priv.h"
#include "caption-closed.h"

static CaptionHookingControls_t __priv_callback_controls = NULL;
static CaptionHookingString_t __priv_callback_string = NULL;

void
caption_hooking_set_controls (CaptionHookingControls_t callback)
{
  __priv_callback_controls = callback;
}

void
caption_hooking_set_string (CaptionHookingString_t callback)
{
  __priv_callback_string = callback;
}

void
caption_hooking_priv_run_controls (CaptionClosed_t *caption,
                                   const uint8_t cmd)
{
  if (__priv_callback_controls != NULL)
    __priv_callback_controls (caption, cmd);
  else
    ATV_DEBUG("Nenhum hook de controles registrado.");
}

void
caption_hooking_priv_run_string (CaptionClosed_t *caption,
                                 const char *string,
                                 uint8_t *len)
{
  if (__priv_callback_string != NULL)
    __priv_callback_string (caption, string, *len);
  else
    ATV_DEBUG("Nenhum hook de texto registrado.");
}
