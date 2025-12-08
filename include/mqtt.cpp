#include "main.h"

#define MQTT_ANNOUNCE_TIMER 5

PicoMQTT::Client* mqtt_client;

Ticker mqtt;
volatile bool mqtt_announce = false;

// Forward declarations
bool get_heater();
bool get_servo_status();
void servo_on();
void servo_off();

// Callback for incoming MQTT command messages.
void __callback(const char* topic, const char* payload) {
  String command = payload;
  LOG_MSG("MQTT Command Received: [%s] %s", topic, command.c_str());
  if (command == "START_CYCLE") {
    start_dry_cycle();
  } else if (command == "STOP_CYCLE") {
    stop_dry_cycle();
  } else if (command == "FAN_ON") {
    fan_on();
  } else if (command == "FAN_OFF") {
    fan_off();
  } else if (command == "HEATER_ON") {
    heater_on();
  } else if (command == "HEATER_OFF") {
    heater_off();
  } else if (command == "VENT_OPEN") {
    servo_on();
  } else if (command == "VENT_CLOSE") {
    servo_off();
  }
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
  LOG_MSG("MQTT OK");
}

void handle_mqtt() {
  // handle mqtt client
  mqtt_client->loop();

  // periodic publish IP
  if (mqtt_announce) {
    mqtt_announce = false;
    String base_topic = String(config.device_name);
    mqtt_client->publish(base_topic + "/DESCRIPTION",
                         String(DEFAULT_DEVICE_NAME) + " " + String(VERSION));
    mqtt_client->publish(base_topic + "/IP", WiFi.localIP().toString());
    mqtt_client->publish(base_topic + "/TEMP", String(get_temperature()));
    mqtt_client->publish(base_topic + "/HUMIDITY", String(get_humidity()));
    mqtt_client->publish(base_topic + "/HEATER", get_heater() ? "ON" : "OFF");
    mqtt_client->publish(base_topic + "/FAN", get_fan() ? "ON" : "OFF");
    mqtt_client->publish(base_topic + "/STATUS", get_dry_cycle_state_str());
    mqtt_client->publish(base_topic + "/VENT",
                         get_servo_status() ? "OPEN" : "CLOSED");
#ifdef DEBUG_VERBOSE
    LOG_MSG("MQTT PUBLISH");
#endif
  }
}