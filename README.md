# closedcaption-parser

Biblioteca em C para extrair o closed caption (legenda oculta) do padrão ISDB-Tb/SBTVD
(ARIB STD-B24 / ABNT NBR 15606-1) a partir dos pacotes PES de legenda de um MPEG-TS.

A biblioteca não faz o demux do TS: ela recebe o PES da legenda e entrega o texto
(em UTF-8) e os comandos de controle através de callbacks. O `example/main.c` mostra
como fazer o demux e usar a API.

## Estrutura

```
include/   headers da biblioteca (caption.h inclui todos)
src/       implementação
example/   exemplo de uso: lê um .ts e imprime as legendas
docs/      amostra de TS com legenda (veja docs/README.md)
```

## Compilando

```
make                 # gera libclosedcaption.so
make example         # gera example/example
make run-example     # roda o exemplo com a amostra de docs/
make clean
```

No macOS o Makefile passa o SDK (`xcrun --show-sdk-path`) para o compilador, então
funciona também com compiladores fora do Xcode, como o LLVM do Homebrew.

## Rodando o exemplo

1. Compile o exemplo:

   ```
   make example
   ```

2. Rode sem informar o PID para listar os PIDs do arquivo. O PID da legenda é
   identificado pela PMT e marcado com `<- legenda`:

   ```
   ./example/example docs/tv-camara-isdbtb-legenda.ts
   ```

   ```
   PIDs encontrados em 'docs/tv-camara-isdbtb-legenda.ts':
     PID      Pacotes  Programa  Tipo
     0x0000       153  -         PAT
     ...
     0x03e8        34  16512     PMT
     0x03ea       200  16512     0x1b H.264 video (component_tag 0x00)
     0x03eb      1353  16512     0x11 AAC LATM audio (component_tag 0x10)
     0x03ed       120  16512     0x06 PES privado (component_tag 0x30, legenda)  <- legenda
     ...

   Para exibir as legendas:
     ./example/example docs/tv-camara-isdbtb-legenda.ts 0x03ed
   ```

3. Rode novamente passando o PID para exibir as legendas:

   ```
   ./example/example docs/tv-camara-isdbtb-legenda.ts 0x03ed
   ```

   ```
   [00:00:00.000] QUE RESISTAM AO
   [00:00:00.066] DESENTENDIMENTO ENTRE OS
   [00:00:02.102] PREFEITOS, NO CASO
   [00:00:03.370] ESPECÍFICO AQUI
   [00:00:03.870] METROPOLITANO. SE NÓS
   [00:00:04.637] DIZEMOS QUE A SOLUÇÃO É
   [00:00:06.172] ENTRAR NUMA MESA E CHEGAR
   [00:00:07.173] NO ACORDO, NÓS NÃO
   [00:00:08.341] PRECISAMOS DE LEI PARA
   [00:00:09.309] ISSO, ELES JÁ PODEM FAZER

   10 linha(s) de legenda extraída(s) do PID 0x03ed.
   ```

   O tempo é relativo à primeira legenda. As legendas são impressas no stdout e as
   mensagens no stderr, então `> legendas.txt` grava apenas o texto.

O exemplo aceita pacotes de 188 bytes (TS), 192 (M2TS) e 204 (TS com Reed-Solomon,
comum em capturas de ISDB-T).

## Usando a API

```c
#include "caption.h"

static void on_line (const CaptionLine_t *line, void *userdata)
{
  printf ("[%u ms] %s\n", line->start_ms, line->text);
}

static void on_pes (const uint8_t *pes, size_t len, void *userdata)
{
  caption_screen_push_pes (userdata, pes, len);
}

TsReader_t *reader = ts_reader_open ("video.ts", NULL);
TsDemux_t *demux = ts_demux_new ();
CaptionScreen_t *screen = caption_screen_new (on_line, NULL);
const uint8_t *packet;

ts_demux_set_pes_callback (demux, 0x3ed, on_pes, screen);

while ((packet = ts_reader_next (reader)) != NULL)
  ts_demux_push (demux, packet);

ts_demux_flush (demux);
caption_screen_flush (screen);
```

Quem já tem o próprio demux pode usar só o `caption_screen_push_pes()`, ou as
funções de baixo nível com os hooks de `caption-hooking.h`. A documentação
completa é gerada com `make docs` e publicada em
<https://jpereira.github.io/closedcaption-parser/>.

## Testes

```
make test
```

Compara a saída do exemplo com a amostra real e com os TS sintéticos de `tests/`.

## Limitações

- Apenas texto em ISO-8859-15 (legendas brasileiras); legendas japonesas não são suportadas.
- Os módulos de superimpose (bitmap, DRCS, som, etc.) ainda são apenas esqueletos.
- Do "caption management data" só o primeiro idioma é lido.
