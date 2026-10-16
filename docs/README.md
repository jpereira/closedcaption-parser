# Samples {#amostras}

## tv-camara-isdbtb-legenda.ts

Snippet of an ISDB-Tb (SBTVD) capture from TV Câmara (Brazil), with closed captions in
Brazilian Portuguese on PID `0x03ed` (program 16512, `component_tag` 0x30). Packets are 204
bytes (188 of TS + 16 of Reed-Solomon).

Source: <https://streams.videolan.org/streams/ts/ARIB/brazil/TV_CAMARA_brazil.ts> (62 MB).

To make it small (2.2 MB) the sample was cut down like this:

- null packets (PID `0x1fff`) removed;
- kept only the first 200 packets of each video PID;
- every other PID (PAT, PMTs, SI, audios and the captions) kept whole.

Usage:

```
make run-example
```

or, step by step:

```
./example/example docs/tv-camara-isdbtb-legenda.ts          # lists the PIDs
./example/example docs/tv-camara-isdbtb-legenda.ts 0x03ed   # shows the captions
```
