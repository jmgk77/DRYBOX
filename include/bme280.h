#pragma once

#define TH_SENSOR_READ 60

void init_bme280();
float get_temperature();
float get_humidity();