#include "main.h"

#define TH_SENSOR_READ 1

Adafruit_BME280 bme;
Ticker get_th;

volatile float temperature;
volatile float humidity;

void __get_th_callback() {
  temperature = bme.readTemperature();
  humidity = bme.readHumidity();
#ifdef DEBUG_VERBOSE
  LOG_MSG("T:%.2f H:%.2f", temperature, humidity);
#endif
}

void init_bme280() {
  if (!bme.begin(0x76)) {
    LOG_MSG("BME280 NOK");
    while (1) {
    };
  } else {
    LOG_MSG("BME280 OK");
  }

  __get_th_callback();
  get_th.attach_scheduled(TH_SENSOR_READ, __get_th_callback);
}

float get_temperature() { return temperature; }

float get_humidity() { return humidity; }
