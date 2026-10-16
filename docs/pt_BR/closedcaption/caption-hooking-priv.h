/**
 * @file caption-hooking-priv.h
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @date Thu Aug 12 13:49:21 BRT 2010
 * @brief Uso interno: chamada dos hooks registrados em caption-hooking.h.
 *
 * Estas funções são usadas pelo caption-closed.c e pelo
 * caption-closed-controls.c e não fazem parte da API pública.
 */
#ifndef CAPTION_HOOKING_PRIV_H_
#define CAPTION_HOOKING_PRIV_H_

#include "caption-common.h"
#include "caption-closed.h"
#include "caption-hooking.h"

/**
 * Pega o hook de controles que está registrado agora (ou NULL), para poder restaurar
 * depois de um caption_hooking_set_controls() temporário.
 *
 * @return O hook, ou NULL.
 */
CaptionHookingControls_t caption_hooking_priv_get_controls (void);

/**
 * Pega o hook de texto que está registrado agora (ou NULL), para poder restaurar
 * depois de um caption_hooking_set_string() temporário.
 *
 * @return O hook, ou NULL.
 */
CaptionHookingString_t caption_hooking_priv_get_string (void);

/**
 * Chama o hook de controles, se houver um registrado.
 *
 * @param caption  Estado da legenda.
 * @param cmd      Código do comando.
 */
void caption_hooking_priv_run_controls (CaptionClosed_t *caption,
                                        const uint8_t cmd);

/**
 * Chama o hook de texto, se houver um registrado.
 *
 * @param caption  Estado da legenda.
 * @param string   Caractere em UTF-8.
 * @param len      Quantidade de bytes em @p string.
 */
void caption_hooking_priv_run_string (CaptionClosed_t *caption,
                                      const char *string,
                                      uint8_t *len);

#endif /* CAPTION_HOOKING_PRIV_H_ */
