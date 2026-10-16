/**
 * @file caption-hooking.h
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @date Thu Aug 12 13:49:21 BRT 2010
 * @brief Registro das callbacks (hooks) que recebem o texto e os comandos da
 *        legenda durante o caption_closed_run().
 *
 * Durante o caption_closed_run(), cada caractere de texto é convertido para
 * UTF-8 e entregue ao hook de texto, e cada comando de controle C0/C1
 * reconhecido é entregue ao hook de controles.
 *
 * Exemplo:
 * @code
 * static void
 * on_string (CaptionClosed_t *cc, const char *utf8, const uint8_t len)
 * {
 *   fwrite (utf8, 1, len, stdout);
 * }
 *
 * static void
 * on_controls (CaptionClosed_t *cc, const uint8_t cmd)
 * {
 *   if (cmd == CTRL_C0_APR) // Active Position Return: nova linha
 *     putchar ('\n');
 * }
 *
 * caption_hooking_set_string (on_string);
 * caption_hooking_set_controls (on_controls);
 * @endcode
 *
 * @warning Os hooks são globais: existe apenas um de cada tipo para todo o
 *          processo, e eles não devem ser alterados enquanto outra thread
 *          executa o caption_closed_run(). Use CaptionClosed_t::userdata para
 *          identificar o contexto dentro do hook.
 */
#ifndef CAPTION_HOOKING_H_
#define CAPTION_HOOKING_H_

#include "caption-common.h"
#include "caption-closed.h"

/**
 * Hook chamado para cada comando de controle reconhecido.
 *
 * @param cc   Estado da legenda, já atualizado pelo comando.
 * @param cmd  Código do comando: C0 (@ref CaptionClosed_CtrlCodes_C0), C1
 *             (@ref CaptionClosed_CtrlCodes_C1) ou, para o CSI, o código do
 *             subcomando (@ref CaptionClosed_CtrlCodes_C1_CSI).
 */
typedef void (*CaptionHookingControls_t) (CaptionClosed_t *cc,
                                          const uint8_t cmd);

/**
 * Hook chamado para cada caractere de texto.
 *
 * @param cc      Estado da legenda (cores, tamanho, posição) no momento do caractere.
 * @param string  Caractere em UTF-8. Não termina com '\\0'.
 * @param len     Quantidade de bytes em @p string (1 a 3).
 */
typedef void (*CaptionHookingString_t) (CaptionClosed_t *cc,
                                        const char *string,
                                        const uint8_t len);

/**
 * Registra o hook de comandos de controle (C0/C1).
 *
 * @param callback  O hook, ou NULL para desligar.
 */
void caption_hooking_set_controls (CaptionHookingControls_t callback);

/**
 * Registra o hook de texto.
 *
 * @param callback  O hook, ou NULL para desligar.
 */
void caption_hooking_set_string (CaptionHookingString_t callback);

#endif /* CAPTION_HOOKING_H_ */
