#pragma once

#define HEATER_PIN D0
#define FAN_PIN D5

bool heather_status;
bool fan_status;

void init_relays();

void heater_on();
void heater_off();

void fan_on();
void fan_off();
