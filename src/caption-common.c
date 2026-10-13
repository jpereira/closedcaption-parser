/**
 * @file caption-common.c
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @date Thu Aug 12 13:49:21 BRT 2010
 * @brief Funções de depuração (apenas com @c -DDEBUG_CLOSEDCAPTION).
 */
#include "caption-common.h"

#ifdef DEBUG_CLOSEDCAPTION

/** Retorna apenas o nome do arquivo, sem o diretório. */
static const char *
file_basename (const char *path)
{
  const char *file = strrchr (path, '/');

  return (file != NULL) ? (file + 1) : path;
}

void
helper_print (const char *platform,
              const char *fileFull,
              const char *func,
              uint32_t line,
              const char *format,
              ...)
{
  va_list ap;

  (void)platform;

  printf ("** DUMP: %s:%u %s() ** ", file_basename (fileFull), line, func);

  va_start (ap, format);
  vprintf (format, ap);
  va_end (ap);

  putchar ('\n');
  fflush (stdout);
}

void
helper_hexdump (const char *platform,
                const char *fileFull,
                const char *func,
                uint32_t fileLine,
                const char *namebuff,
                const char *namebufflen,
                const char *buff,
                size_t buffLen)
{
  size_t offset, i;

  helper_print (platform, fileFull, func, fileLine,
                "<hexdump begin %s %s=%zu (%#zx)>", namebuff, namebufflen, buffLen, buffLen);

  // Formato: "00000000  xx xx xx xx xx xx xx xx  xx xx xx xx xx xx xx xx |................|"
  for (offset = 0; buffLen <= 8192 && offset < buffLen; offset += 16)
  {
    printf ("%08zx  ", offset);

    for (i = 0; i < 16; i++)
    {
      if (offset + i < buffLen)
        printf ("%02x ", buff[offset + i] & 0xff);
      else
        printf ("   ");

      if (i == 7)
        putchar (' ');
    }

    putchar ('|');

    for (i = 0; i < 16 && offset + i < buffLen; i++)
      putchar (IS_PRINTABLE (buff[offset + i]) ? buff[offset + i] : '.');

    puts ("|");
  }

  helper_print (platform, fileFull, func, fileLine,
                "</hexdump end %s %s=%zu (%#zx)>", namebuff, namebufflen, buffLen, buffLen);
}

#endif /* DEBUG_CLOSEDCAPTION */
