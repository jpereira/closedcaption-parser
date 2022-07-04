/**
 * @file: caption-closed-controls.h
 * @autor: Jorge Pereira <jorge.pereira@tqtvd.com>
 * @date: Thu Aug 12 13:49:21 BRT 2010
 * @desc: Mais informações em 'arquitetura-closedcaption.txt'
 */
#ifndef CAPTION_HOOKING_PRIV_H_
#define CAPTION_HOOKING_PRIV_H_

#include "caption-common.h"
#include "caption-closed.h"

void caption_hooking_priv_run_controls (CaptionClosed_t *, const uint8_t cmd);

void caption_hooking_priv_run_string (CaptionClosed_t *, const char *, uint8_t *);

#endif /*CAPTION_HOOKING_PRIV_H_*/

