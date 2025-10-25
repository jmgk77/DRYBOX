#include "main.h"

#define config_SIGNATURE 'J'

struct config_data {
  unsigned char sign = config_SIGNATURE;
  unsigned int checksum;
  //
  char device_name[32];
  char mqtt_server_ip[64];
  unsigned int mqtt_server_port;
  char mqtt_server_username[64];
  char mqtt_server_password[64];
};

struct config_data config;

unsigned int __calculate_config_checkum() {
  //
  unsigned char buffer[sizeof(config_data)];
  CRC32 crc;

  // save old checksum
  unsigned int temp_checksum = config.checksum;
  config.checksum = 0;

  // copy config data
  memcpy(buffer, &config, sizeof(config_data));

  for (unsigned int i = 0; i < sizeof(config_data); i++) {
    crc.update(buffer[i]);
  }

  // restore old checksum
  config.checksum = temp_checksum;
  return crc.finalize();
}

void save_config() {
  config.checksum = __calculate_config_checkum();
  EEPROM.put(0, config);
  EEPROM.commit();
}

void default_config() {
  config = {};
  config.sign = config_SIGNATURE;
  strcpy(config.device_name, DEFAULT_DEVICE_NAME);
  config.mqtt_server_port = 1883;
  config.checksum = __calculate_config_checkum();
}

bool __verify_config() {
  unsigned int checksum = __calculate_config_checkum();
  if ((config.sign != config_SIGNATURE) || (config.checksum != checksum)) {
#ifdef DEBUG
    Serial.printf("* CONFIG NOK (want %08x has %08x)\n", config.checksum,
                  checksum);
#endif
    return false;
  } else {
#ifdef DEBUG
    Serial.println("* CONFIG OK");
#endif
    return true;
  }
}

void init_config() {
  // init config
  EEPROM.begin(sizeof(config_data));

  // if there's valid config config, load it
  EEPROM.get(0, config);
  if (!__verify_config()) {
    default_config();
  }
}