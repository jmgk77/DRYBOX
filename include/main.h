#pragma once

#if !defined(ESP8266)
#error This code is designed to run on ESP8266 and ESP8266-based boards! Please check your Tools->Board setting.
#endif

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

#include "config.h"
#include "datetime.h"
#include "dump.h"
#include "html.h"
#include "mqtt.h"
#include "relays.h"
#include "version.h"
#include "web.h"

#define DEFAULT_DEVICE_NAME "DRYBOX"

#define DEBUG
// #define ENABLE_EXTRA_DISCOVERY
// #define WWW_FILESERVER
