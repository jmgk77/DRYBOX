#include "main.h"

#define MAX_TH_INFO 60

#define TH_LOG_TIME 5

struct TH_INFO {
  float temperature;
  float humidity;
};

TH_INFO th_info[MAX_TH_INFO];
unsigned int th_index = 0;
char th_log_name[64];

Ticker th;

void __dump_csv() {
  File f = LittleFS.open(th_log_name, "a");

  // loop database
  if (f) {
    for (unsigned int i = 0; i < sizeof(th_info) / sizeof(th_info[0]); i++) {
      f.printf("%.01f, %.01f\n", th_info[i].temperature, th_info[i].humidity);
    }
    f.close();
  }
#ifdef DEBUG
  Serial.println("* LOGGER ERROR");
#endif
  th_index = 0;
}

void init_logger() {
  struct tm now;
  time_t t = time(NULL);
  localtime_r(&t, &now);

  LittleFS.format();

  // create csv file
  strftime(th_log_name, sizeof(th_log_name), "/%Y%m%d_%H%M.csv", &now);
  File f = LittleFS.open(th_log_name, "w");
  if (f) {
    f.printf("Temperatura, Umidade\n");
    f.close();
  } else {
#ifdef DEBUG
    Serial.println("* LOGGER NOK");
#endif
    return;
  }

  // save
  th.attach_scheduled(TH_LOG_TIME, []() {
    th_info[th_index].temperature = get_temperature();
    th_info[th_index].humidity = get_humidity();
    th_index++;
    if (th_index >= MAX_TH_INFO) {
      __dump_csv();
    }
  });

#ifdef DEBUG
  Serial.println("* LOGGER OK (" + String(th_log_name) + ")");
#endif
}
