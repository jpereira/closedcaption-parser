# libclosedcaption

🇺🇸 **English** · 🇧🇷 [Português (pt_BR)](README.pt_BR.md)

[![CI](https://github.com/jpereira/libclosedcaption/actions/workflows/ci.yml/badge.svg)](https://github.com/jpereira/libclosedcaption/actions/workflows/ci.yml)
[![Release](https://img.shields.io/github/v/release/jpereira/libclosedcaption?display_name=tag)](https://github.com/jpereira/libclosedcaption/releases)
[![License: LGPL v3](https://img.shields.io/badge/license-LGPL--3.0-blue)](LICENSE)
[![Docs](https://img.shields.io/badge/docs-doxygen-informational)](https://jpereira.github.io/libclosedcaption/)

C library to extract closed captions from the subtitle PES packets of an MPEG-TS, following
the **Brazilian digital TV standard** (ISDB-Tb / SBTVD, ARIB STD-B24 and ABNT NBR 15606-1).

> **Heads up:** this is a personal project made for the Brazilian DTV world. It targets
> ISDB-Tb captions (Brazilian Portuguese, ISO-8859-15 text), not the Japanese ISDB-T
> charsets, and not ATSC/DVB. The code, the comments and the sample stream started in pt_BR
> (the internal `src/` comments still are) and the English docs are a translation, so some
> terms come straight from the Brazilian specs (SBTVD, ABNT...). Everything is also
> available in [pt_BR](README.pt_BR.md).

The lib doesn't demux the TS for you: you give it the caption PES and it hands back the text
(UTF-8) and the control commands through callbacks. `example/main.c` shows how to do the
demux and use the API.

## Installing

```
make
sudo make install            # PREFIX=/usr/local by default (DESTDIR and PREFIX work)
```

It installs the `.so` (soname `libclosedcaption.so.1`), the `.a`, the headers in
`$PREFIX/include/closedcaption` (use `#include <closedcaption/caption.h>`) and the
`libclosedcaption.pc`, so you can build like this:

```
cc my-program.c $(pkg-config --cflags --libs libclosedcaption)
```

There is also a container image on GitHub Packages:
`docker run --rm -v "$PWD:/d" ghcr.io/jpereira/libclosedcaption /d/video.ts`.

Runtime version with `closedcaption_version()` (and `CLOSEDCAPTION_VERSION_*` in the header).

## Layout

```
include/closedcaption/   library headers (caption.h includes everything)
src/       implementation
example/   usage example: reads a .ts and prints the captions
docs/      sample TS with captions (see docs/README.md)
```

## Building

```
make                 # builds libclosedcaption.so
make example         # builds example/example
make run-example     # runs the example with the sample from docs/
make clean
```

On macOS the Makefile passes the SDK (`xcrun --show-sdk-path`) to the compiler, so it also
works with compilers outside Xcode, like Homebrew's LLVM.

## Running the example

1. Build the example:

   ```
   make example
   ```

2. Run it without a PID to list the PIDs in the file. The caption PID is found through the
   PMT and marked with `<- legenda`:

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

   (The example program output is in pt_BR on purpose, the tests compare against it.)

3. Run it again passing the PID to show the captions:

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

   Time is relative to the first caption. Captions go to stdout and messages go to stderr,
   so `> captions.txt` saves only the text.

The example takes 188 byte packets (TS), 192 (M2TS) and 204 (TS with Reed-Solomon, common in
ISDB-T captures).

## Using the API

```c
#include <closedcaption/caption.h>

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

If you already have your own demux you can use just `caption_screen_push_pes()`, or the
low level functions with the hooks from `caption-hooking.h`. The full docs are generated with
`make docs` and published at <https://jpereira.github.io/libclosedcaption/> (English, default)
and <https://jpereira.github.io/libclosedcaption/pt_BR/> (pt_BR).

## Documentation

Install Doxygen and Graphviz and run:

```
make docs
```

Open `build/docs/html/index.html` (English) or `build/docs/html/pt_BR/index.html` (pt_BR)
to browse the site locally, with the usage guide and the API reference.

The pt_BR API docs are built from a copy of the headers in `docs/pt_BR/`. If you change a
declaration, `make check-i18n` tells you when the copy drifted from `include/`.

## Tests

```
make test           # compares the example with the real sample and the synthetic TS + API tests
make test-sanitize  # same thing with ASan + UBSan
make lint           # cppcheck
make fuzz           # libFuzzer (clang): build/fuzz-pes -max_total_time=60
```

CI runs all of it on Linux (gcc/clang) and macOS on every push. See [CONTRIBUTING](CONTRIBUTING.md).

## Limitations

- Only ISO-8859-15 text (Brazilian captions); Japanese captions are not supported.
- The superimpose modules (bitmap, DRCS, sound, etc.) are still just skeletons.
- From the "caption management data" only the first language is read.

## Releases

Version in the `VERSION` file (SemVer), history in the [CHANGELOG](CHANGELOG.md).
`./bump.sh` closes the current version (commit + tag) and opens the next one; the tag fires
the release with the tarball and the container image.

## License

LGPL-3.0, see [LICENSE](LICENSE). Found a security bug? See [SECURITY](SECURITY.md).
