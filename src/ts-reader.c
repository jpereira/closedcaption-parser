/**
 * @file ts-reader.c
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @brief Leitura de arquivos MPEG-TS, pacote a pacote.
 */
#include "ts-reader.h"

/** Maior pacote suportado (TS com Reed-Solomon). */
#define TS_READER_MAX_PACKET_SIZE 204

struct TsReader_s {
  FILE *fp;
  size_t packet_size; /* 188, 192 ou 204 */
  size_t offset;      /* posição do 0x47 dentro do pacote */
  uint8_t packet[TS_READER_MAX_PACKET_SIZE];
};

bool
ts_detect_packet_format (const uint8_t *buf,
                         size_t len,
                         size_t *packet_size,
                         size_t *offset)
{
  static const struct { size_t size, offset; } formats[] = {
    { 188, 0 }, // MPEG-TS
    { 192, 4 }, // M2TS: 4 bytes de timecode antes do pacote
    { 204, 0 }  // MPEG-TS + 16 bytes de Reed-Solomon
  };
  unsigned i;

  assert (buf != NULL);
  assert (packet_size != NULL);
  assert (offset != NULL);

  for (i = 0; i < ARRAY_SIZE (formats); i++)
  {
    size_t size = formats[i].size, off = formats[i].offset;

    if (len >= size * 2 + off + 1 &&
        buf[off] == TS_SYNC_BYTE && buf[off + size] == TS_SYNC_BYTE && buf[off + size * 2] == TS_SYNC_BYTE)
    {
      *packet_size = size;
      *offset = off;
      return TRUE;
    }
  }

  return FALSE;
}

TsReader_t *
ts_reader_open (const char *filename,
                TsReaderError_t *error)
{
  uint8_t buf[TS_READER_MAX_PACKET_SIZE * 4];
  TsReaderError_t dummy;
  TsReader_t *reader;
  size_t n;

  assert (filename != NULL);

  if (error == NULL)
    error = &dummy;

  reader = calloc (1, sizeof (TsReader_t));
  if (reader == NULL)
  {
    *error = TS_READER_ERROR_MEMORY;
    return NULL;
  }

  reader->fp = fopen (filename, "rb");
  if (reader->fp == NULL)
  {
    int saved = errno;

    free (reader);
    errno = saved;
    *error = TS_READER_ERROR_OPEN;
    return NULL;
  }

  n = fread (buf, 1, sizeof (buf), reader->fp);
  rewind (reader->fp);

  if (!ts_detect_packet_format (buf, n, &reader->packet_size, &reader->offset))
  {
    ts_reader_close (reader);
    *error = TS_READER_ERROR_FORMAT;
    return NULL;
  }

  *error = TS_READER_OK;

  return reader;
}

const uint8_t *
ts_reader_next (TsReader_t *reader)
{
  assert (reader != NULL);

  while (fread (reader->packet, 1, reader->packet_size, reader->fp) == reader->packet_size)
  {
    int c;

    if (reader->packet[reader->offset] == TS_SYNC_BYTE)
      return &reader->packet[reader->offset];

    // Perdeu o sincronismo: procura o próximo 0x47 e volta o arquivo para
    // o início do pacote (descontando o timecode do M2TS).
    while ((c = fgetc (reader->fp)) != EOF && c != TS_SYNC_BYTE)
      ;

    if (c == EOF || fseek (reader->fp, -1 - (long)reader->offset, SEEK_CUR) != 0)
      break;
  }

  return NULL;
}

size_t
ts_reader_get_packet_size (const TsReader_t *reader)
{
  assert (reader != NULL);

  return reader->packet_size;
}

void
ts_reader_close (TsReader_t *reader)
{
  if (reader == NULL)
    return;

  if (reader->fp != NULL)
    fclose (reader->fp);

  free (reader);
}
