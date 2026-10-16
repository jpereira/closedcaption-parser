/**
 * @file caption-superimpose.h
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @date Thu Aug 12 13:49:21 BRT 2010
 * @brief Legendas do tipo superimpose.
 *
 * Superimpose são as mensagens sobrepostas ao vídeo que não fazem parte da
 * legenda do programa (avisos da emissora, alertas). Elas usam o mesmo
 * formato de @c data_group da legenda (ARIB STD-B24, Volume 1, Part 3, Capítulo 9),
 * mas em um PID próprio (@c component_tag 0x38 a 0x3f).
 *
 * Cada data unit de texto (@ref CAPTION_HANDLER_TYPE_STATEMENT_BODY) do data group vira uma
 * mensagem, com o texto convertido para UTF-8 como o caption_closed_run() faz.
 *
 * @note Enquanto roda, o caption_superimpose_new() troca os hooks globais do
 *       caption-hooking.h e coloca os anteriores de volta antes de retornar.
 */
#ifndef CAPTION_SUPERIMPOSE_H_
#define CAPTION_SUPERIMPOSE_H_

#include "caption-common.h"
#include "caption-data.h"

/** Mensagem de superimpose. Veja @ref CaptionSuperImpose_s. */
typedef struct CaptionSuperImpose_s CaptionSuperImpose_t;

/**
 * Mensagem de superimpose.
 */
struct CaptionSuperImpose_s {
  const uint8_t *data;  /**< Dados do data_group (CaptionData_t::data); válidos enquanto o PacketPES_t existir. */
  char *text_utf8;      /**< Texto em UTF-8, terminado em '\\0', liberado por caption_superimpose_free(). */
  size_t len;           /**< Tamanho de @ref text_utf8, sem o '\\0'. */

  CaptionSuperImpose_t *next; /**< Próxima mensagem do mesmo CaptionData_t, ou NULL. */
};

/**
 * Cria a struct a partir de um @c data_group de superimpose.
 *
 * @param caption  @c data_group carregado por caption_data_load(), não vazio.
 * @return A primeira mensagem (as outras ficam encadeadas em @c next), ou NULL se faltou
 *         memória, se o data group é inválido ou se não tem texto. Libere com
 *         caption_superimpose_free().
 */
CaptionSuperImpose_t *caption_superimpose_new (const CaptionData_t *caption);

/**
 * Libera a struct criada por caption_superimpose_new(), incluindo todas as
 * mensagens encadeadas em @c next.
 *
 * @param csi  A struct. NULL é aceito.
 */
void caption_superimpose_free (CaptionSuperImpose_t *csi);

#endif /* CAPTION_SUPERIMPOSE_H_ */
