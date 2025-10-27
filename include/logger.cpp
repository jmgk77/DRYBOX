#include "main.h"

#define MAX_TH_INFO 60

#define TH_LOG_TIME 60

struct TH_INFO {
  float temperature;
  float humidity;
};

TH_INFO th_info[MAX_TH_INFO];
volatile unsigned int th_index = 0;
char th_log_name[64];

Ticker th;

void __dump_log() {
  File f = LittleFS.open(th_log_name, "a");

  if (f) {
    // Write the entire buffer as a binary block
    f.write((uint8_t*)th_info, sizeof(th_info));
    f.close();
  } else {
#ifdef DEBUG
    Serial.println("* LOGGER ERROR (" + String(th_log_name) + ")");
#endif
  }
  th_index = 0;
}

void __th_callback() {
  th_info[th_index].temperature = get_temperature();
  th_info[th_index].humidity = get_humidity();
  th_index++;
  if (th_index >= MAX_TH_INFO) {
    __dump_log();
  }
}

void init_logger() {
  struct tm now;
  time_t t = time(NULL);
  localtime_r(&t, &now);

  // LittleFS.format();

  // create binary log name
  strftime(th_log_name, sizeof(th_log_name), "/%Y%m%d_%H%M.bin", &now);

  // save
  __th_callback();
  th.attach_scheduled(TH_LOG_TIME, __th_callback);

#ifdef DEBUG
  Serial.println("* LOGGER OK (" + String(th_log_name) + ")");
#endif
}
