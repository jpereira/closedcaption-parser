/**
 * @file caption-version.c
 * @brief Versão da biblioteca, vem do arquivo VERSION via Makefile.
 */
#include "caption-version.h"

#ifndef CLOSEDCAPTION_VERSION
# define CLOSEDCAPTION_VERSION "unknown"
#endif

const char *
closedcaption_version (void)
{
  return CLOSEDCAPTION_VERSION;
}
