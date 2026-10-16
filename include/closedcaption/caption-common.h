/**
 * @file caption-common.h
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @date Thu Aug 12 13:49:21 BRT 2010
 * @brief Standard headers and helper macros used all over the library.
 *
 * <b>Debugging</b>
 *
 * Building with @c -DDEBUG_CLOSEDCAPTION, the macros @ref DTV_DEBUG and
 * @ref DTV_HEXDUMP start printing messages and hex dumps to
 * stdout. Without that flag they generate no code.
 */
#ifndef CAPTION_HELPER_H_
#define CAPTION_HELPER_H_

#ifndef _GNU_SOURCE
/** Enables the libc GNU extensions (e.g. POSIX/GNU function declarations). */
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
 * Checks if @p x is a printable ISO-8859-15 character, that is, if it is
 * in the GL (0x20 to 0x7e) or GR (0xa1 to 0xfe) ranges of the character table.
 */
#define is_latin_iso8859_15(x) (((x) >= 0x20 && (x) <= 0x7e) || ((x) >= 0xa1 && (x) <= 0xfe))

#ifdef ARRAY_SIZE
# undef ARRAY_SIZE
#endif
/** Number of elements of a static array. */
#define ARRAY_SIZE(x)   ((unsigned)(sizeof(x)/sizeof((x)[0])))

/** Turns a boolean into text, for debug messages. */
#define PRINT_BOOL(x)   (((x) == TRUE)?"TRUE":(((x) == FALSE)?"FALSE":"UNKNOWN"))

#undef UNUSED
/** Marks a parameter as unused, to avoid the compiler warning. */
#define UNUSED(x)       x __attribute__ ((__unused__))

#ifndef FALSE
/** False. Kept for compatibility; same as @c false. */
#  define FALSE 0
#endif

#ifndef TRUE
/** True. Kept for compatibility; same as @c true. */
#  define TRUE 1
#endif

#ifdef DEBUG_CLOSEDCAPTION

/** Visible ASCII character (no space), used in the hex dump. */
#define IS_PRINTABLE(a)     ((a) >= 33 && (a) <= 126)

#ifndef DTV_DEBUG
/**
 * Prints a debug message, printf() style, with the file,
 * line and function it came from. Only exists with @c -DDEBUG_CLOSEDCAPTION.
 */
# define DTV_DEBUG(fmt, ...)   \
    helper_print("DEBUG", __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)
#endif

#ifndef DTV_HEXDUMP
/**
 * Prints the hex dump of a buffer. Only exists with
 * @c -DDEBUG_CLOSEDCAPTION.
 */
# define DTV_HEXDUMP(buff, buffLen)\
    helper_hexdump("DEBUG", __FILE__, __func__, __LINE__, #buff, #buffLen, (const char*)(buff), (buffLen))
#endif

/**
 * Prints the hex dump of a buffer. Use the @ref DTV_HEXDUMP macro.
 *
 * @param platform     Message prefix (not used).
 * @param fileFull     Source file (@c __FILE__).
 * @param func         Source function (@c __func__).
 * @param fileLine     Source line (@c __LINE__).
 * @param namebuff     Name of the buffer variable.
 * @param namebufflen  Name of the size variable.
 * @param buff         Buffer.
 * @param buffLen      Buffer size; above 8192 bytes the content is not printed.
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
 * Prints a debug message. Use the @ref DTV_DEBUG macro.
 *
 * @param platform  Message prefix (not used).
 * @param fileFull  Source file (@c __FILE__).
 * @param func      Source function (@c __func__).
 * @param line      Source line (@c __LINE__).
 * @param format    Format, like printf().
 */
void helper_print (const char *platform,
                   const char *fileFull,
                   const char *func,
                   uint32_t line,
                   const char *format,
                   ...) __attribute__ ((format (printf, 5, 6)));

#else

# ifndef DTV_DEBUG
#  define DTV_DEBUG(fmt, ...)         do { } while (0)
# endif

# ifndef DTV_HEXDUMP
#  define DTV_HEXDUMP(buff, buffLen)  do { } while (0)
# endif

#endif /* DEBUG_CLOSEDCAPTION */

#endif /* CAPTION_HELPER_H_ */
