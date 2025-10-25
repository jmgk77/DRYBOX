#pragma once

#if !defined(ESP8266)
#error This code is designed to run on ESP8266 and ESP8266-based boards! Please check your Tools->Board setting.
#endif

#include <Adafruit_BME280.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <Arduino.h>
#include <CRC32.h>
#include <ESP8266LLMNR.h>
#include <ESP8266NetBIOS.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncHTTPUpdateServer.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>
#include <ESP_EEPROM.h>
#include <LittleFS.h>
#include <PicoMQTT.h>
#include <Ticker.h>

#define DEFAULT_DEVICE_NAME "DRYBOX"

#define DEBUG
// #define DEBUG_SENSORS
// #define DEBUG_HW
// #define ENABLE_EXTRA_DISCOVERY
#define WWW_FILESERVER

//
#include "html.h"
#include "oled.h"
#include "version.h"

//
#include "bme280.cpp"
#include "config.cpp"
#include "datetime.cpp"
#include "dump.cpp"
#include "logger.cpp"
#include "mqtt.cpp"
#include "oled.cpp"
#include "relays.cpp"
#include "web.cpp"
