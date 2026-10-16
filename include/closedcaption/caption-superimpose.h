/**
 * @file caption-superimpose.h
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @date Thu Aug 12 13:49:21 BRT 2010
 * @brief Legendas do tipo superimpose. <b>Não implementado</b>.
 *
 * Superimpose são as mensagens sobrepostas ao vídeo que não fazem parte da
 * legenda do programa (avisos da emissora, alertas). Elas usam o mesmo
 * formato de @c data_group da legenda (ARIB STD-B24, Volume 3, Capítulo 9),
 * mas em um PID próprio (@c component_tag 0x38 a 0x3f).
 *
 * Por enquanto apenas aloca a estrutura, sem interpretar os dados.
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
  const uint8_t *data;  /**< Dados do data_group. Não preenchido. */
  char *text_utf8;      /**< Texto em UTF-8, liberado por caption_superimpose_free(). Não preenchido. */
  size_t len;           /**< Tamanho de @ref text_utf8. */

  CaptionSuperImpose_t *next; /**< Próxima mensagem do mesmo CaptionData_t, ou NULL. */
};

/**
 * Cria a estrutura a partir de um @c data_group de superimpose.
 *
 * @param caption  @c data_group carregado por caption_data_load(), não vazio.
 * @return A estrutura, ou NULL se faltar memória. Liberar com
 *         caption_superimpose_free().
 */
CaptionSuperImpose_t *caption_superimpose_new (const CaptionData_t *caption);

/**
 * Libera a estrutura criada por caption_superimpose_new(), incluindo todas as
 * mensagens encadeadas em @c next.
 *
 * @param csi  A estrutura. Aceita NULL.
 */
void caption_superimpose_free (CaptionSuperImpose_t *csi);

#endif /* CAPTION_SUPERIMPOSE_H_ */
