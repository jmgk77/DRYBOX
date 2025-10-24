#include "main.h"

bool heater_status;
bool fan_status;

void init_relays() {
  pinMode(HEATER_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);

  heater_off();
  fan_off();
#ifdef DEBUG
  Serial.println("* RELAYS OK");
#endif
}

void heater_on() {
  digitalWrite(HEATER_PIN, LOW);
  heater_status = true;
#ifdef DEBUG
  Serial.println("! Heater ON");
#endif
}

void heater_off() {
  digitalWrite(HEATER_PIN, HIGH);
  heater_status = false;
#ifdef DEBUG
  Serial.println("! Heater OFF");
#endif
}

bool get_heater() { return heater_status; }

void fan_on() {
  digitalWrite(FAN_PIN, HIGH);
  fan_status = true;
#ifdef DEBUG
  Serial.println("! Fan ON");
#endif
};

void fan_off() {
  digitalWrite(FAN_PIN, LOW);
  fan_status = false;
#ifdef DEBUG
  Serial.println("! Fan OFF");
#endif
};

bool get_fan() { return fan_status; }
