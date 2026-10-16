# Amostras {#amostras}

## tv-camara-isdbtb-legenda.ts

Trecho de uma captura ISDB-Tb (SBTVD) da TV Câmara, com closed caption em português
no PID `0x03ed` (programa 16512, `component_tag` 0x30). Os pacotes são de 204 bytes
(188 do TS + 16 de Reed-Solomon).

Origem: <https://streams.videolan.org/streams/ts/ARIB/brazil/TV_CAMARA_brazil.ts> (62 MB).

Para ficar pequena (2,2 MB), a amostra foi reduzida assim:

- removidos os pacotes nulos (PID `0x1fff`);
- mantidos apenas os primeiros 200 pacotes de cada PID de vídeo;
- todos os outros PIDs (PAT, PMTs, SI, áudios e a legenda) foram mantidos inteiros.

Uso:

```
make run-example
```

ou, passo a passo:

```
./example/example docs/tv-camara-isdbtb-legenda.ts          # lista os PIDs
./example/example docs/tv-camara-isdbtb-legenda.ts 0x03ed   # exibe as legendas
```
