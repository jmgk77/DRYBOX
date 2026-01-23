#include "main.h"

#define MAX_TH_INFO 60
#define TH_HISTORY 180
#define TH_LOG_TIME 60

struct TH_INFO {
  float temperature;
  float humidity;
  float weight;
};

TH_INFO th_info[MAX_TH_INFO];
volatile unsigned int th_index = 0;
char th_log_name[64];
bool __do_dump_log = false;

Ticker th;

void __dump_log() {
  File f = LittleFS.open(th_log_name, "a");

  if (f) {
    // Write the entire buffer as a binary block
    f.write((uint8_t*)th_info, sizeof(th_info));
    f.close();
  } else {
    LOG_MSG("LOGGER ERROR (%s)", th_log_name);
  }
  // memory log was dumped to disk, now can accept more entries
  th_index = 0;
  LOG_MSG("DUMPED LOG");
}

void __th_callback() {
  if ((th_index < MAX_TH_INFO)) {
    th_info[th_index].temperature = get_temperature();
    th_info[th_index].humidity = get_humidity();
    th_info[th_index].weight = get_weight();
    th_index++;
  } else {
    __do_dump_log = true;
  }
}

void init_logger() {
  // This function now assumes time is already synchronized.
  struct tm now;
  time_t t = time(NULL);
  localtime_r(&t, &now);

  // create binary log name
  strftime(th_log_name, sizeof(th_log_name), "/%Y%m%d_%H%M.bin", &now);

  // Start the periodic logging
  __th_callback();
  th.attach_scheduled(TH_LOG_TIME, __th_callback);

  LOG_MSG("LOGGER OK (%s)", th_log_name);
}

void handle_logger() {
  if (__do_dump_log) {
    __do_dump_log = false;
    __dump_log();
  }
}