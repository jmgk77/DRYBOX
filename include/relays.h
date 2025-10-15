#pragma once

#define HEATER_PIN 16
#define FAN_PIN D5

void init_relays();

void heater_on();
void heater_off();

void fan_on();
void fan_off();
