#pragma once

#define HEATER_PIN 16  // D0
#define FAN_PIN 2      // D4

void init_relays();

void heater_on();
void heater_off();
bool get_heater();

void fan_on();
void fan_off();
bool get_fan();