#ifndef CAPTION_VERSION_H_
#define CAPTION_VERSION_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file caption-version.h
 * @brief Versão da biblioteca (semver, a mesma do arquivo VERSION).
 */

/**
 * @name Versão em tempo de compilação
 * Só pra quem quer checar com @c \#if.
 * @{
 */
#define CLOSEDCAPTION_VERSION_MAJOR 1
#define CLOSEDCAPTION_VERSION_MINOR 0
#define CLOSEDCAPTION_VERSION_PATCH 1
/** @} */

/**
 * Versão da lib que está carregada em runtime, ex.: "1.0.1".
 *
 * Bom pra comparar com o CLOSEDCAPTION_VERSION_* do header usado no build.
 *
 * @return String estática com a versão (não precisa liberar).
 */
const char *closedcaption_version (void);

#ifdef __cplusplus
}
#endif

#endif /* CAPTION_VERSION_H_ */
