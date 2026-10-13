# Revisão {#revisao}

Resumo dos problemas encontrados e corrigidos na revisão da biblioteca.

## Memória e travamentos

- `caption_superimpose*_free()` usava memória já liberada (`obj->next` após `free`).
- Comandos de controle liam parâmetros além do fim do buffer (APS, COL, SS3, SZX).
- `caption_closed_controls_get_name()` lia fora das tabelas.
- `assert` sobre dados do sinal derrubava o processo (SWF, parâmetros do CSI).
- Overflow em deslocamentos de bits no PTS e no OTM/STM.
- Paleta aceitava o índice 16 (fora da tabela).

## Texto incorreto ou perdido

- Parâmetros de comandos não suportados (FLC, HLC, TIME, ESC, CSI desconhecido, SZX) viravam texto.
- Legendas com mais de 255 bytes eram cortadas (`uint8_t`).
- `~` era rejeitado; um caractere desconhecido descartava o resto da legenda.
- Offset do payload do PES era fixo (só funcionava com uma emissora).
- CRC-16 nunca era verificado e era lido do lugar errado quando havia stuffing.
- Management e statement data eram lidos sem distinção e sem checar tamanhos;
  constantes `MANAGEMENT_TMD_*` erradas; apenas a primeira data unit era lida.
- PES com PTS+DTS era aceito sem payload; parâmetros do CSI com zero à
  esquerda eram lidos como octal; SS3 dependia de máquina little-endian.
- Cores de meia intensidade erradas na paleta (amarelo, ciano, branco).

## Build

- Faltava `-fPIC` (a `.so` não linkava no Linux) e dependência dos headers.
- Build de depuração (`-DDEBUG_CLOSEDCAPTION`) não compilava.
- Flags de depuração fixas nos `.c` e restos do log antigo (`atvLog-mute.h`).

## Limitações conhecidas

- Apenas texto em ISO-8859-15; superimpose e DRCS são esqueletos.
- Hooks globais: não é seguro usar em mais de uma thread.
- Valores exatos da paleta de meia intensidade não foram conferidos na norma.
