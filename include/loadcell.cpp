#include "main.h"

#define LOADCELL_DAT 12  // D6
#define LOADCELL_CLK 13  // D7
#define LOADCELL_READ_INTERVAL 1.0

HX711 loadcell;
Ticker loadcell_timer;

volatile float _cached_weight = 0.0f;
bool _tare_request = false;
volatile bool _read_request = false;

void __loadcell_callback() { _read_request = true; }

void init_loadcell() {
  loadcell.begin(LOADCELL_DAT, LOADCELL_CLK);
  loadcell.set_scale(config.loadcell_scale);
  loadcell.set_offset(config.loadcell_offset);

  loadcell_timer.attach_scheduled(LOADCELL_READ_INTERVAL, __loadcell_callback);

  LOG_MSG("LOADCELL OK");
}

float get_weight() { return _cached_weight; }

void tare_loadcell() { _tare_request = true; }

void handle_loadcell() {
  if (_tare_request) {
    _tare_request = false;
    loadcell_timer.detach();
    yield();          // Feed watchdog before blocking operation
    loadcell.tare();  // Reset the scale to 0
    config.loadcell_offset = loadcell.get_offset();
    save_config();
    _cached_weight = 0.0f;
    LOG_MSG("LOADCELL TARED. New Offset: %ld", config.loadcell_offset);
    loadcell_timer.attach_scheduled(LOADCELL_READ_INTERVAL,
                                    __loadcell_callback);
  }

  if (_read_request) {
    _read_request = false;
    _cached_weight = loadcell.get_units(5);
  }
}
