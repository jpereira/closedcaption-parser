/**
 * @file ts-reader.h
 * @author Jorge Pereira <jpereiran@gmail.com>
 * @brief Leitura de arquivos MPEG-TS, pacote a pacote.
 *
 * O leitor detecta automaticamente o formato do arquivo e entrega sempre
 * pacotes de @ref TS_PACKET_SIZE bytes, prontos para o ts_demux_push().
 *
 * Formatos aceitos:
 * - 188 bytes: MPEG-TS padrão;
 * - 192 bytes: M2TS (4 bytes de timecode antes de cada pacote);
 * - 204 bytes: MPEG-TS com 16 bytes de Reed-Solomon no final de cada pacote,
 *   comum em capturas de ISDB-T.
 *
 * Quando o sincronismo é perdido (byte @ref TS_SYNC_BYTE fora do lugar), o
 * leitor procura o próximo @ref TS_SYNC_BYTE e continua a partir dele.
 *
 * Exemplo:
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

/** Tamanho de um pacote MPEG-TS, sem bytes extras de timecode ou Reed-Solomon. */
#define TS_PACKET_SIZE 188

/** Byte de sincronismo que inicia todo pacote MPEG-TS. */
#define TS_SYNC_BYTE   0x47

/**
 * Erros retornados por ts_reader_open().
 */
typedef enum {
  TS_READER_OK = 0,       /**< Arquivo aberto com sucesso. */
  TS_READER_ERROR_OPEN,   /**< Não foi possível abrir o arquivo; o motivo está em @c errno. */
  TS_READER_ERROR_FORMAT, /**< O arquivo não parece ser um MPEG-TS (188, 192 ou 204 bytes). */
  TS_READER_ERROR_MEMORY  /**< Falha ao alocar memória. */
} TsReaderError_t;

/**
 * Leitor de arquivo MPEG-TS. Estrutura opaca, criada por ts_reader_open().
 */
typedef struct TsReader_s TsReader_t;

/**
 * Detecta o formato dos pacotes a partir do início de um buffer.
 *
 * São necessários pelo menos 3 pacotes consecutivos com o byte de
 * sincronismo no lugar certo para que o formato seja reconhecido.
 *
 * @param buf          Início do conteúdo do arquivo.
 * @param len          Quantidade de bytes em @p buf (recomendado: 1024 ou mais).
 * @param packet_size  Recebe o tamanho do pacote: 188, 192 ou 204.
 * @param offset       Recebe a posição do @ref TS_SYNC_BYTE dentro do pacote
 *                     (4 para M2TS, 0 para os demais).
 * @return TRUE se o formato foi reconhecido.
 */
bool ts_detect_packet_format (const uint8_t *buf,
                              size_t len,
                              size_t *packet_size,
                              size_t *offset);

/**
 * Abre um arquivo MPEG-TS para leitura.
 *
 * @param filename  Caminho do arquivo.
 * @param error     Opcional (pode ser NULL). Recebe o motivo da falha.
 * @return O leitor, ou NULL em caso de erro. Liberar com ts_reader_close().
 */
TsReader_t *ts_reader_open (const char *filename,
                            TsReaderError_t *error);

/**
 * Lê o próximo pacote do arquivo.
 *
 * @param reader  Leitor criado por ts_reader_open().
 * @return Ponteiro para os @ref TS_PACKET_SIZE bytes do pacote (já sem o
 *         timecode do M2TS e sem o Reed-Solomon), ou NULL no final do
 *         arquivo. O ponteiro só é válido até a próxima chamada.
 */
const uint8_t *ts_reader_next (TsReader_t *reader);

/**
 * Informa o tamanho dos pacotes no arquivo.
 *
 * @param reader  Leitor criado por ts_reader_open().
 * @return 188, 192 ou 204 bytes.
 */
size_t ts_reader_get_packet_size (const TsReader_t *reader);

/**
 * Fecha o arquivo e libera o leitor.
 *
 * @param reader  Leitor criado por ts_reader_open(). Aceita NULL.
 */
void ts_reader_close (TsReader_t *reader);

#endif /* TS_READER_H_ */
