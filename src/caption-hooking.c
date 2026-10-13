/**
 * @file: caption-hooking.c
 * @autor: Jorge Pereira <jpereiran@gmail.com>
 * @date: Thu Aug 12 13:49:21 BRT 2010
 * @desc: Mais informações em 'arquitetura-closedcaption.txt'
 */
#include "caption-hooking.h"
#include "caption-closed.h"

static void (*__priv_callback_controls)(CaptionClosed_t *, const uint8_t) = NULL;
static void (*__priv_callback_string)(CaptionClosed_t *, const char *, const uint8_t) = NULL;

void
caption_hooking_set_controls (void (*callback_hooking)(CaptionClosed_t *, const uint8_t))
{  
  __priv_callback_controls = callback_hooking;
}

void
caption_hooking_set_string (void (*callback_hooking)(CaptionClosed_t *, const char *, const uint8_t))
{  
  __priv_callback_string = callback_hooking;
}

void
caption_hooking_priv_run_controls (CaptionClosed_t *caption,
                                   const uint8_t cmd)
{
  if (__priv_callback_controls != NULL)
  {
    __priv_callback_controls (caption, cmd);
  }
  else
  {
    ATV_DEBUG("Oops!! Nenhum callback foi registrado para run_controls()");
  }
}

void
caption_hooking_priv_run_string (CaptionClosed_t *caption,
                                 const char *string,
                                 uint8_t *len)
{ 
  if (__priv_callback_string != NULL)
  {      
    __priv_callback_string (caption, string, *len);
  }
  else
  {
      ATV_DEBUG("Oops!! Nenhum callback foi registrado para run_string()");
  }
}

