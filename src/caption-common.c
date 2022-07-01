/**
 * @file: caption-common.c
 * @autor: Jorge Pereira <jorge.pereira@tqtvd.com>
 * @date: Thu Aug 12 13:49:21 BRT 2010
 * @desc: Mais informações em 'arquitetura-closedcaption.txt'
 */
#include "caption-common.h"

#ifdef DEBUG_CLOSEDCAPTION
void
helper_print (const char *platform,
              const char *fileFull,
              const char *func,
              uint32_t line,
              const char *format,
              ...)
{
  va_list ap;
  char *file;

  file = (char*)strrchr (fileFull, '/');
  file = (file != NULL) ? (file + 1) : (char*)fileFull;
  (void)platform;

#if 0
  struct timeval tv;
  struct timezone tz;

  gettimeofday(&tv, &tz);

  printf ("[%05ld.%03d] *** DUMP[%s]: %s:%u %s() ** ",
      (long)tv.tv_sec, (int)(tv.tv_usec / 1000), platform, file, line, func);
#else
  printf ("** DUMP: %s:%u %s() ** ", file, line, func);
#endif

  fflush_unlocked(stdout);
  va_start(ap, format);
  vprintf (format, ap);
  fflush_unlocked(stdout);
  va_end(ap);
  putchar ('\n');
  fflush_unlocked(stdout);
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
  uint32_t line = 0;
  uint32_t count = 1;
  uint32_t pos = 0;
  uint32_t i = 0;

  helper_print(platform, fileFull, func, fileLine,
          "<hexdump begin %s %s=%d (%#x)>", namebuff, namebufflen, buffLen, buffLen);

  printf("%08x  ", line);

  if (buffLen <= 8192)
  {
      while (pos < buffLen)
      {
          printf ("%02x\x20", buff[pos++] & 0xff);

          if (count == 8)
          {
              putchar ('\x20');
          }
          else if (count==16)
          {
              uint32_t log;

              printf("|");

              if (pos >= 16)
              {
                  log = pos - 16;
              }
              else
              {
                  log = pos;
              }

              for( ;log < pos; log++)
              {
                  printf("%c", IS_PRINTABLE(buff[log]) ? buff[log] : '.');
              }

              puts("|");
              printf("%08x\x20\x20",line += 16);

              if (pos > (buffLen-16))
              {
                  i = pos;
              }

              count = 0;
          }
          count += 1;
      }
      if (buffLen != i)
      {
          for(;(pos-i) < 16; pos++)
          {
              printf("\x20\x20\x20");
          }
          printf("\x20|");

          for(;i<buffLen;i++)
          {
              printf("%c", IS_PRINTABLE(buff[i]) ? buff[i] : '.');
          }

          puts("|");
      }

      if (buffLen != pos)
      {
          printf("%08x\n", line += --count);
      }
      else
      {
          putchar('\n');
      }
  }

  helper_print(platform, fileFull, func, fileLine,
          "</hexdump end %s %s=%d (%#x)>", namebuff, namebufflen, buffLen, buffLen);
  fflush_unlocked(stdout);
}
#endif /*DEBUG_CLOSEDCAPTION*/
