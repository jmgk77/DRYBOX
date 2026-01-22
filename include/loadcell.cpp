#include "main.h"

#define LOADCELL_DAT 12  // D6
#define LOADCELL_CLK 13  // D7
#define LOADCELL_READ_INTERVAL 0.1

HX711 loadcell;
Ticker loadcell_timer;

volatile float _cached_weight = 0.0f;

void __loadcell_callback() {
  // Non-blocking check. If hardware is ready, read it.
  if (loadcell.is_ready()) {
    // Rolling average of 5 samples to smooth noise without blocking
    // get_units(5) would block for ~500ms, triggering WDT
    static float samples[5];
    static uint8_t idx = 0;
    static uint8_t count = 0;

    samples[idx] = loadcell.get_units(1);
    idx = (idx + 1) % 5;
    if (count < 5) count++;

    float sum = 0;
    for (uint8_t i = 0; i < count; i++) sum += samples[i];
    _cached_weight = sum / float(count);
  }
}

void init_loadcell() {
  loadcell.begin(LOADCELL_DAT, LOADCELL_CLK);
  loadcell.set_scale(config.loadcell_scale);
  loadcell.set_offset(config.loadcell_offset);

  loadcell_timer.attach_scheduled(LOADCELL_READ_INTERVAL, __loadcell_callback);

  LOG_MSG("LOADCELL OK");
}

float get_weight() { return _cached_weight; }
