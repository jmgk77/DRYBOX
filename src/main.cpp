#include "main.h"

void setup() {
  md_initial_heap = ESP.getFreeHeap();

#ifdef DEBUG
  Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY, 1);
  delay(1000);
  LOG_MSG("__------------------------------------------------------------__");
  LOG_MSG("%s %s", DEFAULT_DEVICE_NAME, VERSION);
#endif

  LittleFS.begin();

#if defined(DEBUG) && defined(LOG_TO_FILE)
  init_file_logging();
#endif

  init_config();

  dump_esp8266();
  dump_fs();
  dump_config();

  init_oled();

  init_time();

  init_web();

  init_mqtt();

  init_relays();

  init_servo();

  init_bme280();

  oled_clear();

  init_dry();

  init_button();

  // init_logger();
  // init_memdebug();
}

void loop() {
  handle_web();

  handle_mqtt();

  handle_oled();

  handle_dry();

  handle_button();

  handle_logger();

  handle_memdebug();
}