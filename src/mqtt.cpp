#include "main.h"

PicoMQTT::Client* mqtt_client;

Ticker mqtt;
bool mqtt_announce = false;

String mqtt_topic_ip;
String mqtt_topic_buttons;

extern struct config_data config;

void __callback(const char* payload) {
  // ***
}

void init_mqtt() {
  // mqtt client
  mqtt_client = new PicoMQTT::Client(
      config.mqtt_server_ip, config.mqtt_server_port, config.device_name,
      config.mqtt_server_username, config.mqtt_server_password);
  mqtt_client->begin();

  // periodic announce
  mqtt.attach_scheduled(MQTT_ANNOUNCE_TIMER, []() { mqtt_announce = true; });

  mqtt_client->subscribe(String(config.device_name) + "/COMMAND", __callback);
#ifdef DEBUG
  Serial.println("* MQTT OK");
#endif
}

void handle_mqtt() {
  // handle mqtt client
  mqtt_client->loop();

  // periodic publish IP
  if (mqtt_announce) {
    mqtt_announce = false;

    mqtt_client->publish(
        String(config.device_name) + "/DESCRIPTION",
        String(DEFAULT_DEVICE_NAME) + String(" ") + String(VERSION));

    // send IP
    mqtt_client->publish(String(config.device_name) + "/IP",
                         WiFi.localIP().toString());

    // send status
    mqtt_client->publish(String(config.device_name) + "/TEMP",
                         String(get_temperature()));
    mqtt_client->publish(String(config.device_name) + "/HUMIDITY",
                         String(get_humidity()));
    // mqtt_client->publish(String(config.device_name) + "/STATUS", "IDLE");
  }
}