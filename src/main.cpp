#include "main.h"

void setup() {
#ifdef DEBUG
  Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY, 1);
  delay(1000);
  Serial.println();
  Serial.println(String(DEFAULT_DEVICE_NAME) + " " + VERSION);
#endif

  LittleFS.begin();

  init_config();

#ifdef DEBUG_HW
  dump_esp8266();
  dump_fs();
  dump_config();
#endif

  init_oled();

  init_time();

  init_web();

  init_mqtt();

  init_relays();

  init_servo();

  init_bme280();

  oled_clear();

  init_logger();

  init_dry();

  init_button();
}

void loop() {
  handle_web();

  handle_mqtt();

  handle_oled();

  handle_dry();

  handle_button();
}