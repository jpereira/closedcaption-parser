/**
 * @file ts-demux.h
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @brief Demultiplexador MPEG-TS: tabelas PAT/PMT, estatísticas por PID e
 *        remontagem dos pacotes PES de um PID.
 *
 * O demux recebe os pacotes TS de 188 bytes (veja ts-reader.h) e:
 *
 * - conta os pacotes de cada PID;
 * - lê a PAT e as PMTs de todos os programas, identificando o tipo de cada
 *   PID (vídeo, áudio, legenda, PCR, etc.);
 * - sugere o PID da legenda, a partir do @c component_tag da PMT;
 * - remonta os pacotes PES de um PID escolhido e entrega cada PES completo
 *   para uma callback (por exemplo, caption_screen_push_pes()).
 *
 * Exemplo:
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

/** PID da PAT (Program Association Table). */
#define TS_PID_PAT  0x0000

/** PID dos pacotes nulos (stuffing). */
#define TS_PID_NULL 0x1fff

/** Quantidade de PIDs possíveis (13 bits). */
#define TS_PID_MAX  0x2000

/** Valor utilizado para indicar "nenhum PID". */
#define TS_PID_NONE 0xffff

/**
 * Informações coletadas sobre um PID.
 */
typedef struct {
  uint32_t packets;      /**< Quantidade de pacotes TS recebidos neste PID. */
  uint16_t program;      /**< @c program_number ao qual o PID pertence, ou 0 quando desconhecido. */
  bool is_pmt;           /**< O PID carrega a PMT de um programa. */
  bool is_pcr;           /**< O PID é o PCR de um programa. */
  bool is_es;            /**< O PID está listado como stream (ES) em alguma PMT. */
  uint8_t stream_type;   /**< @c stream_type da PMT (válido quando @ref is_es). */
  int16_t component_tag; /**< @c component_tag da PMT, ou -1 quando não informado. */
} TsPidInfo_t;

/**
 * Callback chamada para cada PES completo do PID escolhido em
 * ts_demux_set_pes_callback().
 *
 * @param pes       Pacote PES completo, a partir do @c packet_start_code_prefix
 *                  (00 00 01). Só é válido durante a chamada.
 * @param len       Tamanho de @p pes em bytes.
 * @param userdata  Ponteiro informado em ts_demux_set_pes_callback().
 */
typedef void (*TsDemuxPesCallback_t) (const uint8_t *pes,
                                      size_t len,
                                      void *userdata);

/**
 * Demultiplexador MPEG-TS. Estrutura opaca, criada por ts_demux_new().
 */
typedef struct TsDemux_s TsDemux_t;

/**
 * Cria um demultiplexador.
 *
 * @return O demux, ou NULL se faltar memória. Liberar com ts_demux_free().
 */
TsDemux_t *ts_demux_new (void);

/**
 * Libera o demultiplexador. Não chama ts_demux_flush().
 *
 * @param demux  Demux criado por ts_demux_new(). Aceita NULL.
 */
void ts_demux_free (TsDemux_t *demux);

/**
 * Escolhe o PID cujos pacotes PES serão remontados e entregues à @p callback.
 *
 * Apenas um PID é remontado por vez. Uma nova chamada substitui a anterior e
 * descarta o PES que estava incompleto.
 *
 * @param demux     Demux criado por ts_demux_new().
 * @param pid       PID a ser remontado, ou @ref TS_PID_NONE para desligar.
 * @param callback  Função chamada para cada PES completo.
 * @param userdata  Repassado para a @p callback.
 */
void ts_demux_set_pes_callback (TsDemux_t *demux,
                                uint16_t pid,
                                TsDemuxPesCallback_t callback,
                                void *userdata);

/**
 * Processa um pacote TS.
 *
 * Pacotes com o @c transport_error_indicator ligado são ignorados. Pacotes
 * perdidos (falha no @c continuity_counter) descartam o PES incompleto.
 *
 * @param demux   Demux criado por ts_demux_new().
 * @param packet  Pacote de @ref TS_PACKET_SIZE bytes, iniciando em @ref TS_SYNC_BYTE.
 */
void ts_demux_push (TsDemux_t *demux,
                    const uint8_t *packet);

/**
 * Entrega o último PES, caso ainda esteja pendente.
 *
 * Um PES só é considerado completo quando o próximo começa ou quando atinge o
 * @c PES_packet_length; por isso, chame esta função no final do arquivo.
 *
 * @param demux  Demux criado por ts_demux_new().
 */
void ts_demux_flush (TsDemux_t *demux);

/**
 * Retorna as informações coletadas sobre um PID.
 *
 * @param demux  Demux criado por ts_demux_new().
 * @param pid    PID entre 0 e @ref TS_PID_MAX - 1.
 * @return As informações do PID, ou NULL se @p pid for inválido.
 */
const TsPidInfo_t *ts_demux_get_pid_info (const TsDemux_t *demux,
                                          uint16_t pid);

/**
 * Retorna o PID de legenda sugerido pela PMT.
 *
 * É escolhido o stream privado (@c stream_type 0x06) com @c component_tag de
 * legenda: 0x30 a 0x37 (full-seg) ou 0x87 (one-seg), dando preferência ao
 * menor @c component_tag, ou seja, à legenda principal do full-seg.
 *
 * @param demux  Demux criado por ts_demux_new().
 * @return O PID da legenda, ou @ref TS_PID_NONE se nenhum foi encontrado.
 */
uint16_t ts_demux_get_caption_pid (const TsDemux_t *demux);

/**
 * Monta uma descrição legível do PID, por exemplo
 * <tt>"0x06 PES privado (component_tag 0x30, legenda)"</tt>.
 *
 * @param demux  Demux criado por ts_demux_new().
 * @param pid    PID entre 0 e @ref TS_PID_MAX - 1.
 * @param buf    Buffer que recebe a descrição.
 * @param len    Tamanho de @p buf (128 bytes são suficientes).
 * @return O próprio @p buf.
 */
const char *ts_demux_describe_pid (const TsDemux_t *demux,
                                   uint16_t pid,
                                   char *buf,
                                   size_t len);

/**
 * Nome do @c stream_type da PMT (ISO/IEC 13818-1, Tabela 2-34).
 *
 * @param stream_type  Valor do @c stream_type.
 * @return O nome, por exemplo <tt>"H.264 video"</tt>, ou <tt>"desconhecido"</tt>.
 */
const char *ts_stream_type_name (uint8_t stream_type);

/**
 * Nome do @c component_tag no ISDB-T (ARIB TR-B14 / ABNT NBR 15608).
 *
 * @param component_tag  Valor do @c component_tag.
 * @return <tt>"legenda"</tt>, <tt>"legenda one-seg"</tt>, <tt>"superimpose"</tt>,
 *         ou NULL para os demais valores.
 */
const char *ts_component_tag_name (int16_t component_tag);

/**
 * Nome dos PIDs reservados para tabelas do sistema (PAT, NIT, SDT, EIT, etc.).
 *
 * @param pid  PID a ser consultado.
 * @return O nome da tabela, ou NULL se o PID não é reservado.
 */
const char *ts_system_pid_name (uint16_t pid);

#endif /* TS_DEMUX_H_ */
