#include "main.h"

void init_relays() {
  pinMode(HEATER_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);

  heater_off();
  fan_off();
}

void heater_on() {
  digitalWrite(HEATER_PIN, LOW);
  heather_status = true;
}
void heater_off() {
  digitalWrite(HEATER_PIN, HIGH);
  fan_status = false;
}

void fan_on() {
  digitalWrite(FAN_PIN, HIGH);
  fan_status = true;
};
void fan_off() {
  digitalWrite(FAN_PIN, LOW);
  fan_status = false;
};
