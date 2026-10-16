/**
 * @file caption-screen.h
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @brief Converte os pacotes PES de legenda em linhas de texto.
 *
 * É a forma mais simples de usar a biblioteca: recebe os PES completos do PID
 * da legenda (veja ts-demux.h), executa todo o processamento
 * (packet_pes_load(), caption_data_load(), caption_management_load() e
 * caption_closed_run()) e entrega cada linha de legenda pronta, em UTF-8,
 * para uma callback.
 *
 * <b>Como as linhas são montadas</b>
 *
 * - Os comandos APR (0x0d) e APS (0x1c) quebram a linha; os espaços de
 *   preenchimento no final de cada linha são removidos.
 * - Cada PES redesenha a tela inteira. Legendas ao vivo (roll-up) reenviam a
 *   mesma linha a cada poucas letras, por isso uma linha só é entregue quando
 *   deixa a tela, ou seja, quando nenhuma linha da tela nova é uma
 *   continuação dela.
 * - A comparação ignora acentos: é comum a emissora enviar a letra sem acento
 *   e corrigi-la na tela seguinte (ex: "ESPECI" → "ESPECÍFICO").
 * - O tempo de cada linha é o PTS do PES em que ela apareceu, relativo ao
 *   primeiro PTS de legenda recebido.
 *
 * Exemplo:
 * @code
 * static void
 * on_line (const CaptionLine_t *line, void *userdata)
 * {
 *   printf ("[%u ms] %s\n", line->start_ms, line->text);
 * }
 *
 * CaptionScreen_t *screen = caption_screen_new (on_line, NULL);
 *
 * // para cada PES completo do PID da legenda:
 * caption_screen_push_pes (screen, pes, len);
 *
 * caption_screen_flush (screen); // entrega as linhas que ainda estão na tela
 * caption_screen_free (screen);
 * @endcode
 *
 * @warning Os hooks de caption-hooking.h são globais: caption_screen_push_pes()
 *          registra os seus próprios hooks, substituindo os que estiverem
 *          registrados. Pelo mesmo motivo, não é seguro usar a biblioteca em
 *          mais de uma thread ao mesmo tempo.
 */
#ifndef CAPTION_SCREEN_H_
#define CAPTION_SCREEN_H_

#include "caption-common.h"

/** Máximo de linhas mantidas na tela. */
#define CAPTION_SCREEN_MAX_LINES 16

/** Tamanho máximo de uma linha, em bytes UTF-8 (incluindo o '\\0'). */
#define CAPTION_SCREEN_LINE_LEN  256

/**
 * Uma linha de legenda.
 */
typedef struct {
  const char *text;  /**< Texto em UTF-8, sem os espaços de preenchimento do final. */
  uint32_t start_ms; /**< Momento em que a linha apareceu, em milissegundos, relativo ao primeiro PTS de legenda. */
} CaptionLine_t;

/**
 * Callback chamada para cada linha de legenda finalizada.
 *
 * @param line      A linha. Só é válida durante a chamada.
 * @param userdata  Ponteiro informado em caption_screen_new().
 */
typedef void (*CaptionScreenLineCallback_t) (const CaptionLine_t *line,
                                             void *userdata);

/**
 * Tela de legenda. Estrutura opaca, criada por caption_screen_new().
 */
typedef struct CaptionScreen_s CaptionScreen_t;

/**
 * Cria uma tela de legenda.
 *
 * @param callback  Chamada para cada linha finalizada.
 * @param userdata  Repassado para a @p callback.
 * @return A tela, ou NULL se faltar memória. Liberar com caption_screen_free().
 */
CaptionScreen_t *caption_screen_new (CaptionScreenLineCallback_t callback,
                                     void *userdata);

/**
 * Libera a tela. Não chama caption_screen_flush().
 *
 * @param screen  Tela criada por caption_screen_new(). Aceita NULL.
 */
void caption_screen_free (CaptionScreen_t *screen);

/**
 * Processa um PES completo do PID da legenda.
 *
 * PES inválidos, com CRC incorreto, ou que não são "caption statement data"
 * (por exemplo, o "caption management data") são ignorados.
 *
 * @param screen  Tela criada por caption_screen_new().
 * @param pes     PES completo, a partir do @c packet_start_code_prefix (00 00 01).
 * @param len     Tamanho de @p pes em bytes.
 * @return TRUE se o PES continha texto de legenda e foi processado.
 */
bool caption_screen_push_pes (CaptionScreen_t *screen,
                              const uint8_t *pes,
                              size_t len);

/**
 * Entrega todas as linhas que ainda estão na tela e limpa a tela.
 * Chame no final do arquivo.
 *
 * @param screen  Tela criada por caption_screen_new().
 */
void caption_screen_flush (CaptionScreen_t *screen);

/**
 * Verifica se @p line começa com @p prefix, ignorando os acentos das letras
 * do Latin-1 (U+00C0 a U+00FF). Por exemplo, "ESPECÍFICO" começa com
 * "ESPECI".
 *
 * @param line    Texto em UTF-8.
 * @param prefix  Prefixo em UTF-8.
 * @return TRUE se @p line começa com @p prefix.
 */
bool caption_screen_line_continues (const char *line,
                                    const char *prefix);

#endif /* CAPTION_SCREEN_H_ */
