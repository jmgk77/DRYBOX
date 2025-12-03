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

bool heater_status;
bool fan_status;

void heater_on() {
  digitalWrite(HEATER_PIN, RELAY_ON);
  heater_status = true;
#ifdef DEBUG
  // Serial.println("! Heater ON");
#endif
}

void heater_off() {
  digitalWrite(HEATER_PIN, RELAY_OFF);
  heater_status = false;
#ifdef DEBUG
  // Serial.println("! Heater OFF");
#endif
}

bool get_heater() { return digitalRead(HEATER_PIN) == RELAY_ON; }

void fan_on() {
  digitalWrite(FAN_PIN, RELAY_ON);
  fan_status = true;
#ifdef DEBUG
  // Serial.println("! Fan ON");
#endif
};

void fan_off() {
  digitalWrite(FAN_PIN, RELAY_OFF);
  fan_status = false;
#ifdef DEBUG
  // Serial.println("! Fan OFF");
#endif
};

bool get_fan() { return digitalRead(FAN_PIN) == RELAY_ON; }

void init_relays() {
  pinMode(HEATER_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);

  heater_off();
  fan_off();
#ifdef DEBUG
  Serial.println("* RELAYS OK");
#endif
}
