/**
 * @file ts-demux.h
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @brief MPEG-TS demuxer: PAT/PMT tables, per PID stats and
 *        reassembly of the PES packets of one PID.
 *
 * The demuxer takes the 188 byte TS packets (see ts-reader.h) and:
 *
 * - counts the packets of each PID;
 * - reads the PAT and the PMTs of all programs, finding out the type of each
 *   PID (video, audio, caption, PCR, etc.);
 * - suggests the caption PID, from the PMT @c component_tag;
 * - reassembles the PES packets of a chosen PID and hands each whole PES
 *   to a callback (for example, caption_screen_push_pes()).
 *
 * Example:
 * @code
 * static void
 * on_pes (const uint8_t *pes, size_t len, void *userdata)
 * {
 *   caption_screen_push_pes (userdata, pes, len);
 * }
 *
 * TsDemux_t *demux = ts_demux_new ();
 *
 * ts_demux_set_pes_callback (demux, 0x3ed, on_pes, screen);
 *
 * while ((packet = ts_reader_next (reader)) != NULL)
 *   ts_demux_push (demux, packet);
 *
 * ts_demux_flush (demux);
 * ts_demux_free (demux);
 * @endcode
 */
#ifndef TS_DEMUX_H_
#define TS_DEMUX_H_

#include "caption-common.h"
#include "ts-reader.h"

/** PAT PID (Program Association Table). */
#define TS_PID_PAT  0x0000

/** PID of the null packets (stuffing). */
#define TS_PID_NULL 0x1fff

/** Number of possible PIDs (13 bits). */
#define TS_PID_MAX  0x2000

/** Value used to say "no PID". */
#define TS_PID_NONE 0xffff

/**
 * Info collected about a PID.
 */
typedef struct {
  uint32_t packets;      /**< Number of TS packets received on this PID. */
  uint16_t program;      /**< @c program_number the PID belongs to, or 0 when unknown. */
  bool is_pmt;           /**< The PID carries the PMT of a program. */
  bool is_pcr;           /**< The PID is the PCR of a program. */
  bool is_es;            /**< The PID is listed as a stream (ES) in some PMT. */
  uint8_t stream_type;   /**< PMT @c stream_type (valid when @ref is_es). */
  int16_t component_tag; /**< PMT @c component_tag, or -1 when not informed. */
} TsPidInfo_t;

/**
 * Callback called for each whole PES of the PID chosen in
 * ts_demux_set_pes_callback().
 *
 * @param pes       Whole PES packet, starting at the @c packet_start_code_prefix
 *                  (00 00 01). Only valid during the call.
 * @param len       Size of @p pes in bytes.
 * @param userdata  Pointer given to ts_demux_set_pes_callback().
 */
typedef void (*TsDemuxPesCallback_t) (const uint8_t *pes,
                                      size_t len,
                                      void *userdata);

/**
 * MPEG-TS demuxer. Opaque struct, created by ts_demux_new().
 */
typedef struct TsDemux_s TsDemux_t;

/**
 * Creates a demuxer.
 *
 * @return The demuxer, or NULL if out of memory. Free with ts_demux_free().
 */
TsDemux_t *ts_demux_new (void);

/**
 * Frees the demuxer. Does not call ts_demux_flush().
 *
 * @param demux  Demuxer created by ts_demux_new(). NULL is fine.
 */
void ts_demux_free (TsDemux_t *demux);

/**
 * Chooses the PID whose PES packets will be reassembled and handed to the @p callback.
 *
 * Only one PID is reassembled at a time. A new call replaces the previous one and
 * drops the PES that was incomplete.
 *
 * @param demux     Demuxer created by ts_demux_new().
 * @param pid       PID to be reassembled, or @ref TS_PID_NONE to turn it off.
 * @param callback  Function called for each whole PES.
 * @param userdata  Passed on to the @p callback.
 */
void ts_demux_set_pes_callback (TsDemux_t *demux,
                                uint16_t pid,
                                TsDemuxPesCallback_t callback,
                                void *userdata);

/**
 * Processes a TS packet.
 *
 * Packets with the @c transport_error_indicator set are ignored. Lost
 * packets (@c continuity_counter failure) drop the incomplete PES.
 *
 * @param demux   Demuxer created by ts_demux_new().
 * @param packet  Packet of @ref TS_PACKET_SIZE bytes, starting with @ref TS_SYNC_BYTE.
 */
void ts_demux_push (TsDemux_t *demux,
                    const uint8_t *packet);

/**
 * Delivers the last PES, if it is still pending.
 *
 * A PES is only considered complete when the next one starts or when it reaches the
 * @c PES_packet_length; so call this function at the end of the file.
 *
 * @param demux  Demuxer created by ts_demux_new().
 */
void ts_demux_flush (TsDemux_t *demux);

/**
 * Returns the info collected about a PID.
 *
 * @param demux  Demuxer created by ts_demux_new().
 * @param pid    PID between 0 and @ref TS_PID_MAX - 1.
 * @return The PID info, or NULL if @p pid is invalid.
 */
const TsPidInfo_t *ts_demux_get_pid_info (const TsDemux_t *demux,
                                          uint16_t pid);

/**
 * Returns the caption PID suggested by the PMT.
 *
 * It picks the private stream (@c stream_type 0x06) with a caption
 * @c component_tag: 0x30 to 0x37 (full-seg) or 0x87 (one-seg), preferring the
 * lowest @c component_tag, that is, the main full-seg caption.
 *
 * @param demux  Demuxer created by ts_demux_new().
 * @return The caption PID, or @ref TS_PID_NONE if none was found.
 */
uint16_t ts_demux_get_caption_pid (const TsDemux_t *demux);

/**
 * Builds a readable description of the PID, for example
 * <tt>"0x06 PES privado (component_tag 0x30, legenda)"</tt>.
 *
 * @param demux  Demuxer created by ts_demux_new().
 * @param pid    PID between 0 and @ref TS_PID_MAX - 1.
 * @param buf    Buffer that receives the description.
 * @param len    Size of @p buf (128 bytes is enough).
 * @return @p buf itself.
 */
const char *ts_demux_describe_pid (const TsDemux_t *demux,
                                   uint16_t pid,
                                   char *buf,
                                   size_t len);

/**
 * Name of the PMT @c stream_type (ISO/IEC 13818-1, Table 2-34).
 *
 * @param stream_type  @c stream_type value.
 * @return The name, for example <tt>"H.264 video"</tt>, or <tt>"desconhecido"</tt> (unknown).
 */
const char *ts_stream_type_name (uint8_t stream_type);

/**
 * Name of the @c component_tag in ISDB-T (ARIB TR-B14 / ABNT NBR 15608).
 *
 * @param component_tag  @c component_tag value.
 * @return <tt>"legenda"</tt>, <tt>"legenda one-seg"</tt>, <tt>"superimpose"</tt>,
 *         or NULL for the other values.
 */
const char *ts_component_tag_name (int16_t component_tag);

/**
 * Name of the PIDs reserved for system tables (PAT, NIT, SDT, EIT, etc.).
 *
 * @param pid  PID to look up.
 * @return The table name, or NULL if the PID is not reserved.
 */
const char *ts_system_pid_name (uint16_t pid);

#endif /* TS_DEMUX_H_ */
