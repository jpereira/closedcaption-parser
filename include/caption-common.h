/**
 * @file caption-common.h
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @date Thu Aug 12 13:49:21 BRT 2010
 * @brief Headers padrão e macros utilitárias usadas por toda a biblioteca.
 *
 * <b>Depuração</b>
 *
 * Compilando com @c -DDEBUG_CLOSEDCAPTION, as macros @ref ATV_DEBUG e
 * @ref ATV_HEXDUMP passam a imprimir mensagens e dumps hexadecimais no
 * stdout. Sem essa flag, elas não geram código.
 */
#ifndef CAPTION_HELPER_H_
#define CAPTION_HELPER_H_

#ifndef _GNU_SOURCE
/** Habilita as extensões GNU da libc (ex: declaração de funções POSIX/GNU). */
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <errno.h>

/**
 * Verifica se @p x é um caractere imprimível do ISO-8859-15, ou seja, se está
 * nas faixas GL (0x20 a 0x7e) ou GR (0xa1 a 0xfe) da tabela de caracteres.
 */
#define is_latin_iso8859_15(x) (((x) >= 0x20 && (x) <= 0x7e) || ((x) >= 0xa1 && (x) <= 0xfe))

#ifdef ARRAY_SIZE
# undef ARRAY_SIZE
#endif
/** Quantidade de elementos de um vetor estático. */
#define ARRAY_SIZE(x)   ((unsigned)(sizeof(x)/sizeof((x)[0])))

/** Converte um valor booleano em texto, para mensagens de depuração. */
#define PRINT_BOOL(x)   (((x) == TRUE)?"TRUE":(((x) == FALSE)?"FALSE":"UNKNOWN"))

#undef UNUSED
/** Marca um parâmetro como não utilizado, evitando o warning do compilador. */
#define UNUSED(x)       x __attribute__ ((__unused__))

#ifndef FALSE
/** Falso. Mantido por compatibilidade; equivale a @c false. */
#  define FALSE 0
#endif

#ifndef TRUE
/** Verdadeiro. Mantido por compatibilidade; equivale a @c true. */
#  define TRUE 1
#endif

#ifdef DEBUG_CLOSEDCAPTION

/** Caractere ASCII visível (sem o espaço), usado no dump hexadecimal. */
#define IS_PRINTABLE(a)     ((a) >= 33 && (a) <= 126)

#ifndef ATV_DEBUG
/**
 * Imprime uma mensagem de depuração, no formato do printf(), com o arquivo,
 * a linha e a função de origem. Só existe com @c -DDEBUG_CLOSEDCAPTION.
 */
# define ATV_DEBUG(fmt, ...)   \
    helper_print("DEBUG", __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)
#endif

#ifndef ATV_HEXDUMP
/**
 * Imprime o dump hexadecimal de um buffer. Só existe com
 * @c -DDEBUG_CLOSEDCAPTION.
 */
# define ATV_HEXDUMP(buff, buffLen)\
    helper_hexdump("DEBUG", __FILE__, __func__, __LINE__, #buff, #buffLen, (const char*)(buff), (buffLen))
#endif

/**
 * Imprime o dump hexadecimal de um buffer. Use a macro @ref ATV_HEXDUMP.
 *
 * @param platform     Prefixo da mensagem (não utilizado).
 * @param fileFull     Arquivo de origem (@c __FILE__).
 * @param func         Função de origem (@c __func__).
 * @param fileLine     Linha de origem (@c __LINE__).
 * @param namebuff     Nome da variável do buffer.
 * @param namebufflen  Nome da variável do tamanho.
 * @param buff         Buffer.
 * @param buffLen      Tamanho do buffer; acima de 8192 bytes o conteúdo não é impresso.
 */
void helper_hexdump (const char *platform,
                     const char *fileFull,
                     const char *func,
                     uint32_t fileLine,
                     const char *namebuff,
                     const char *namebufflen,
                     const char *buff,
                     size_t buffLen);

/**
 * Imprime uma mensagem de depuração. Use a macro @ref ATV_DEBUG.
 *
 * @param platform  Prefixo da mensagem (não utilizado).
 * @param fileFull  Arquivo de origem (@c __FILE__).
 * @param func      Função de origem (@c __func__).
 * @param line      Linha de origem (@c __LINE__).
 * @param format    Formato, como no printf().
 */
void helper_print (const char *platform,
                   const char *fileFull,
                   const char *func,
                   uint32_t line,
                   const char *format,
                   ...) __attribute__ ((format (printf, 5, 6)));

#else

# ifndef ATV_DEBUG
#  define ATV_DEBUG(fmt, ...)
# endif

# ifndef ATV_HEXDUMP
#  define ATV_HEXDUMP(buff, buffLen)
# endif

#endif /* DEBUG_CLOSEDCAPTION */

#endif /* CAPTION_HELPER_H_ */
