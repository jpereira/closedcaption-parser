/**
 * @file ts-reader.h
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @brief Reading MPEG-TS files, packet by packet.
 *
 * The reader detects the file format by itself and always hands over
 * packets of @ref TS_PACKET_SIZE bytes, ready for ts_demux_push().
 *
 * Accepted formats:
 * - 188 bytes: standard MPEG-TS;
 * - 192 bytes: M2TS (4 bytes of timecode before each packet);
 * - 204 bytes: MPEG-TS with 16 bytes of Reed-Solomon at the end of each packet,
 *   common in ISDB-T captures.
 *
 * When sync is lost (the @ref TS_SYNC_BYTE byte is out of place), the
 * reader looks for the next @ref TS_SYNC_BYTE and keeps going from there.
 *
 * Example:
 * @code
 * TsReaderError_t error;
 * TsReader_t *reader = ts_reader_open ("video.ts", &error);
 * const uint8_t *packet;
 *
 * if (reader == NULL)
 *   return;
 *
 * while ((packet = ts_reader_next (reader)) != NULL)
 *   ts_demux_push (demux, packet);
 *
 * ts_reader_close (reader);
 * @endcode
 */
#ifndef TS_READER_H_
#define TS_READER_H_

#include "caption-common.h"

/** Size of an MPEG-TS packet, without the extra timecode or Reed-Solomon bytes. */
#define TS_PACKET_SIZE 188

/** Sync byte that starts every MPEG-TS packet. */
#define TS_SYNC_BYTE   0x47

/**
 * Errors returned by ts_reader_open().
 */
typedef enum {
  TS_READER_OK = 0,       /**< File opened fine. */
  TS_READER_ERROR_OPEN,   /**< Could not open the file; the reason is in @c errno. */
  TS_READER_ERROR_FORMAT, /**< The file doesn't look like an MPEG-TS (188, 192 or 204 bytes). */
  TS_READER_ERROR_MEMORY  /**< Failed to allocate memory. */
} TsReaderError_t;

/**
 * MPEG-TS file reader. Opaque struct, created by ts_reader_open().
 */
typedef struct TsReader_s TsReader_t;

/**
 * Detects the packet format from the start of a buffer.
 *
 * It needs at least 3 consecutive packets with the sync byte
 * in the right place for the format to be recognized.
 *
 * @param buf          Start of the file content.
 * @param len          Number of bytes in @p buf (recommended: 1024 or more).
 * @param packet_size  Receives the packet size: 188, 192 or 204.
 * @param offset       Receives the position of the @ref TS_SYNC_BYTE inside the packet
 *                     (4 for M2TS, 0 for the others).
 * @return TRUE if the format was recognized.
 */
bool ts_detect_packet_format (const uint8_t *buf,
                              size_t len,
                              size_t *packet_size,
                              size_t *offset);

/**
 * Opens an MPEG-TS file for reading.
 *
 * @param filename  File path.
 * @param error     Optional (can be NULL). Receives the reason of the failure.
 * @return The reader, or NULL on error. Free with ts_reader_close().
 */
TsReader_t *ts_reader_open (const char *filename,
                            TsReaderError_t *error);

/**
 * Reads the next packet of the file.
 *
 * @param reader  Reader created by ts_reader_open().
 * @return Pointer to the @ref TS_PACKET_SIZE bytes of the packet (already without the
 *         M2TS timecode and without the Reed-Solomon), or NULL at the end of the
 *         file. The pointer is only valid until the next call.
 */
const uint8_t *ts_reader_next (TsReader_t *reader);

/**
 * Tells the packet size in the file.
 *
 * @param reader  Reader created by ts_reader_open().
 * @return 188, 192 or 204 bytes.
 */
size_t ts_reader_get_packet_size (const TsReader_t *reader);

/**
 * Closes the file and frees the reader.
 *
 * @param reader  Reader created by ts_reader_open(). NULL is fine.
 */
void ts_reader_close (TsReader_t *reader);

#endif /* TS_READER_H_ */
