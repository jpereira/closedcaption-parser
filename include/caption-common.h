/**
 * @file: caption-common.h
 * @autor: Jorge Pereira <jorge.pereira@tqtvd.com>
 * @date: Thu Aug 12 13:49:21 BRT 2010
 * @desc: Mais informações em 'arquitetura-closedcaption.txt'
 */
#ifndef CAPTION_HELPER_H_
#define CAPTION_HELPER_H_

// Default headers
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <assert.h>
#include <pthread.h>
#include <getopt.h>
#include <errno.h>

#include <iconv.h>

// Try it:
// # man 7 ascii
#define ASCII_G3        0x1d

#define is_latin_iso8859_15(x) ((x >= 0x20 && x < 0x7e) || (x >= 0xa1 && x <= 0xfe))

#ifdef ARRAY_SIZE
# undef ARRAY_SIZE
#endif
#define ARRAY_SIZE(x)   ((unsigned)sizeof(x)/sizeof(x[0]))

#define PRINT_BOOL(x)   ((x == TRUE)?"TRUE":((x == FALSE)?"FALSE":"UNKNOWN"))

// helper
#define CHAR(x)         ((char)x)

#undef UNUSED
#define UNUSED(x)       x __attribute__ ((__unused__))

#ifndef FALSE
#  define FALSE 0
#endif

#ifndef TRUE
#  define TRUE 1
#endif

#ifndef bool
#  define bool int
#endif

#ifdef DEBUG_CLOSEDCAPTION
#define IS_PRINTABLE(a)     ((a) >= 33 && (a) <= 126)
//#define ATV_DEBUG(fmt, ...) printf("%s:%d %s() ** "fmt"\n", basename(__FILE__),__LINE__,__func__,##__VA_ARGS__)
#ifndef ATV_DEBUG
# define ATV_DEBUG(fmt, ...)   \
    helper_print("TESTE", __FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#endif

#ifndef ATV_HEXDUMP
# define ATV_HEXDUMP(buff, buffLen)\
    helper_hexdump("TESTE", __FILE__, __FUNCTION__, __LINE__, #buff, #buffLen, (const char*)buff, buffLen)
#endif

void
helper_hexdump (const char *platform,
                const char *fileFull,
                const char *func,
                uint32_t fileLine,
                const char *namebuff,
                const char *namebufflen,
                const char *buff,
                size_t buffLen);
                   
void
helper_print (const char *platform,
              const char *fileFull,
              const char *func,
              uint32_t line,
              const char *format,
              ...);

#else

# ifndef ATV_DEBUG
#  define ATV_DEBUG(fmt, ...)
# endif

# ifndef ATV_HEXDUMP
#  define ATV_HEXDUMP(buff, buffLen)
# endif

#endif /* DEBUG_CLOSEDCAPTION */

char *
string_convert (const char *from,
                const char *to,
                const char *text,
                size_t len);

#endif /* CAPTION_HELPER_H_ */
