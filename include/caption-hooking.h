/**
 * @file: caption-closed-controls.h
 * @autor: Jorge Pereira <jorge.pereira@tqtvd.com>
 * @date: Thu Aug 12 13:49:21 BRT 2010
 * @desc: Mais informações em 'arquitetura-closedcaption.txt'
 */
#ifndef CAPTION_HOOKING_H_
#define CAPTION_HOOKING_H_

#include "caption-common.h"
#include "caption-closed.h"

/**
 * Funcoes a serem utilizadas para registrar hooking para os
 * comandos recebidos C0/C1
 */
void caption_hooking_set_controls(void (*callback)(CaptionClosed_t *, const uint8_t));
                               
/**
 * Funcoes a serem utilizadas para registrar hooking para as strings recebidas.
 */
void caption_hooking_set_string(void (*callback)(CaptionClosed_t *, const char *, const uint8_t));

#endif /*CAPTION_HOOKING_H_*/

