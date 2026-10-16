/*
 * Harness de fuzz (libFuzzer): joga bytes aleatórios como PES de legenda.
 * make fuzz && build/fuzz-pes -max_total_time=60
 */
#include <stddef.h>
#include <stdint.h>

#include <closedcaption/caption.h>

static void on_line (const CaptionLine_t *line, void *userdata)
{
  (void) line;
  (void) userdata;
}

int LLVMFuzzerTestOneInput (const uint8_t *data, size_t size)
{
  CaptionScreen_t *screen = caption_screen_new (on_line, NULL);

  if (screen == NULL)
    return 0;

  caption_screen_push_pes (screen, data, size);
  caption_screen_flush (screen);
  caption_screen_free (screen);
  return 0;
}
