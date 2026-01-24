#include "main.h"

#define CONFIG_FILE "/config.json"

struct config_data {
  char device_name[32];
  char mqtt_server_ip[64];
  unsigned int mqtt_server_port;
  char mqtt_server_username[64];
  char mqtt_server_password[64];
  float loadcell_scale;
  long loadcell_offset;
  float loadcell_temp_coeff;
  float tare_temperature;
};
struct config_data config;

void save_config() {
  File configFile = LittleFS.open(CONFIG_FILE, "w");
  if (!configFile) {
    LOG_MSG("Failed to open config file for writing");
    return;
  }

  JsonDocument doc;
  doc["device_name"] = config.device_name;
  doc["mqtt_server_ip"] = config.mqtt_server_ip;
  doc["mqtt_server_port"] = config.mqtt_server_port;
  doc["mqtt_server_username"] = config.mqtt_server_username;
  doc["mqtt_server_password"] = config.mqtt_server_password;
  doc["loadcell_scale"] = config.loadcell_scale;
  doc["loadcell_offset"] = config.loadcell_offset;
  doc["loadcell_temp_coeff"] = config.loadcell_temp_coeff;
  doc["tare_temperature"] = config.tare_temperature;

  if (serializeJson(doc, configFile) == 0) {
    LOG_MSG("Failed to write to config file");
  }
  configFile.close();
}

void default_config() {
  config = {};
  strcpy(config.device_name, DEFAULT_DEVICE_NAME);
  config.mqtt_server_port = 1883;
  config.loadcell_scale = 400.0f;
  config.loadcell_offset = 0;
  config.loadcell_temp_coeff = 0.0f;
  config.tare_temperature = 25.0f;
}

void init_config() {
  if (LittleFS.exists(CONFIG_FILE)) {
    // File exists, reading values
    File configFile = LittleFS.open(CONFIG_FILE, "r");
    if (configFile) {
      JsonDocument doc;
      DeserializationError error = deserializeJson(doc, configFile);
      if (error) {
        LOG_MSG("Failed to read config file (%s), using default configuration",
                error.c_str());
        default_config();
      } else {
        strlcpy(config.device_name, doc["device_name"] | DEFAULT_DEVICE_NAME,
                sizeof(config.device_name));
        strlcpy(config.mqtt_server_ip, doc["mqtt_server_ip"] | "",
                sizeof(config.mqtt_server_ip));
        config.mqtt_server_port = doc["mqtt_server_port"] | 1883;
        strlcpy(config.mqtt_server_username, doc["mqtt_server_username"] | "",
                sizeof(config.mqtt_server_username));
        strlcpy(config.mqtt_server_password, doc["mqtt_server_password"] | "",
                sizeof(config.mqtt_server_password));
        config.loadcell_scale = doc["loadcell_scale"] | 400.0f;
        config.loadcell_offset = doc["loadcell_offset"] | 0;
        config.loadcell_temp_coeff = doc["loadcell_temp_coeff"] | 0.0f;
        config.tare_temperature = doc["tare_temperature"] | 25.0f;
      }
      configFile.close();
    }
  } else {
    // File does not exist, create it with default values
    default_config();
    save_config();
    LOG_MSG("using default configuration");
  }

  LOG_MSG("CONFIG OK");
}