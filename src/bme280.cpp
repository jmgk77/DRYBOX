#include "main.h"

Adafruit_BME280 bme;
Ticker get_th;

volatile float temperature;
volatile float humidity;

void __get_th_callback() {
  temperature = bme.readTemperature();
  humidity = bme.readHumidity();
#ifdef DEBUG_SENSORS
  Serial.println("T:" + String(temperature));
  Serial.println("H:" + String(humidity));
#endif
}

void init_bme280() {
  if (!bme.begin(0x76)) {
#ifdef DEBUG
    Serial.println("* BME280 OK\n");
#endif
    while (1) {
    };
  } else {
#ifdef DEBUG
    Serial.println("* BME280 NOK");
#endif
  }

  __get_th_callback();
  get_th.attach_scheduled(TH_SENSOR_READ, __get_th_callback);
}

float get_temperature() { return temperature; }

float get_humidity() { return humidity; }
