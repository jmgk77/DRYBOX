#include "main.h"

#define CONFIG_FILE "/config.json"

struct config_data {
  char device_name[32];
  char mqtt_server_ip[64];
  unsigned int mqtt_server_port;
  char mqtt_server_username[64];
  char mqtt_server_password[64];
};
struct config_data config;

void save_config() {
  File configFile = LittleFS.open(CONFIG_FILE, "w");
  if (!configFile) {
#ifdef DEBUG
    Serial.println("Failed to open config file for writing");
#endif
    return;
  }

  JsonDocument doc;
  doc["device_name"] = config.device_name;
  doc["mqtt_server_ip"] = config.mqtt_server_ip;
  doc["mqtt_server_port"] = config.mqtt_server_port;
  doc["mqtt_server_username"] = config.mqtt_server_username;
  doc["mqtt_server_password"] = config.mqtt_server_password;

  if (serializeJson(doc, configFile) == 0) {
#ifdef DEBUG
    Serial.println("Failed to write to config file");
#endif
  }
  configFile.close();
}

void default_config() {
  config = {};
  strcpy(config.device_name, DEFAULT_DEVICE_NAME);
  config.mqtt_server_port = 1883;
}

void init_config() {
  if (LittleFS.exists(CONFIG_FILE)) {
    // File exists, reading values
    File configFile = LittleFS.open(CONFIG_FILE, "r");
    if (configFile) {
      JsonDocument doc;
      DeserializationError error = deserializeJson(doc, configFile);
      if (error) {
#ifdef DEBUG
        Serial.println(
            "Failed to read config file, using default configuration");
#endif
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
      }
      configFile.close();
    }
  } else {
    // File does not exist, create it with default values
    default_config();
    save_config();
  }

#ifdef DEBUG
  Serial.println("* CONFIG OK");
#endif
}