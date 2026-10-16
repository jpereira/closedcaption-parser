# Review {#revisao}

Quick summary of the problems found and fixed during the library review.

## Memory and crashes

- `caption_superimpose*_free()` used memory after free (`obj->next` after `free`).
- Control commands read parameters past the end of the buffer (APS, COL, SS3, SZX).
- `caption_closed_controls_get_name()` read outside the tables.
- An `assert` on signal data killed the process (SWF, CSI parameters).
- Overflow in bit shifts on the PTS and on OTM/STM.
- The palette accepted index 16 (outside the table).

## Wrong or lost text

- Parameters of unsupported commands (FLC, HLC, TIME, ESC, unknown CSI, SZX) turned into text.
- Captions bigger than 255 bytes got cut (`uint8_t`).
- `~` was rejected; an unknown character dropped the rest of the caption.
- PES payload offset was fixed (worked with only one broadcaster).
- CRC-16 was never checked and was read from the wrong place when there was stuffing.
- Management and statement data were read without telling them apart and without checking
  sizes; wrong `MANAGEMENT_TMD_*` constants; only the first data unit was read.
- PES with PTS+DTS was accepted without payload; CSI parameters with leading zero were read
  as octal; SS3 depended on a little-endian machine.
- Wrong half intensity colors in the palette (yellow, cyan, white).

## Build

- Missing `-fPIC` (the `.so` didn't link on Linux) and header dependencies.
- Debug build (`-DDEBUG_CLOSEDCAPTION`) didn't compile.
- Debug flags hardcoded in the `.c` files and leftovers from the old log (`atvLog-mute.h`).

## Known limitations

- Only ISO-8859-15 text; the color map data unit is a skeleton.
- DRCS, bitmap, geometric and sound parsers only tested with synthetic streams.
- Global hooks: not safe to use from more than one thread.
- The exact half intensity palette values were not checked against the spec.
