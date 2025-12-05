#include "main.h"

#define HEATER_PIN 16  // D0
#define FAN_PIN 2      // D4

#define LOW_TRIGGER

#ifdef LOW_TRIGGER
#define RELAY_ON LOW
#define RELAY_OFF HIGH
#else
#define RELAY_ON HIGH
#define RELAY_OFF LOW
#endif

void heater_on() {
  digitalWrite(HEATER_PIN, RELAY_ON);
  // LOG_MSG("HEATER ON");
}

void heater_off() {
  digitalWrite(HEATER_PIN, RELAY_OFF);
  // LOG_MSG("HEATER OFF");
}

bool get_heater() { return digitalRead(HEATER_PIN) == RELAY_ON; }

void fan_on() {
  digitalWrite(FAN_PIN, RELAY_ON);
  // LOG_MSG("FAN ON");
};

void fan_off() {
  digitalWrite(FAN_PIN, RELAY_OFF);
  // LOG_MSG("FAN OFF");
};

bool get_fan() { return digitalRead(FAN_PIN) == RELAY_ON; }

void init_relays() {
  pinMode(HEATER_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);

  heater_off();
  fan_off();
  LOG_MSG("RELAYS OK");
}
