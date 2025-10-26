#include "main.h"

const char ESP_INFO[] PROGMEM =
    "ESP8266\n"
    "ESP.getFreeHeap(): %d\n"
    "ESP.getChipId(): %08X\n"
    "ESP.getSdkVersion(): %s\n"
    "ESP.getBootVersion(): %d\n"
    "ESP.getBootMode(): %d\n"
    "ESP.getCpuFreqMHz(): %d\n"
    "ESP.getFlashChipId(): %08X\n"
    "ESP.getFlashChipRealSize(): %d\n"
    "ESP.getFlashChipSize(): %d\n"
    "ESP.getFlashChipSpeed(): %dMHz\n"
    "ESP.getFlashChipSizeByChipId(): %d\n"
    "ESP.getSketchSize(): %d\n"
    "ESP.getFreeSketchSpace(): %d\n"
    "ESP.getResetInfo(): %s\n\n";

const char FS_INFO[] PROGMEM =
    "FS\n"
    "FS.totalBytes(): %d\n"
    "FS.usedBytes(): %d\n"
    "FS.blockSize(): %d\n"
    "FS.pageSize(): %d\n"
    "FS.maxOpenFiles(): %d\n"
    "FS.maxPathLength(): %d\n\n";

const char config_INFO[] PROGMEM =
    "CONFIG\n"
    "config.sign: %c\n"
    "config.checksum: %08x\n"
    "config.device_name: %s\n"
    "config.mqtt_server_ip: %s\n"
    "config.mqtt_server_port: %d\n"
    "config.mqtt_server_username: %s\n"
    "config.mqtt_server_password: %s\n\n";

String __dump_esp8266() {
  char buf[1024];
  snprintf(buf, sizeof(buf), ESP_INFO, ESP.getFreeHeap(), ESP.getChipId(),
           ESP.getSdkVersion(), ESP.getBootVersion(), ESP.getBootMode(),
           ESP.getCpuFreqMHz(), ESP.getFlashChipId(),
           ESP.getFlashChipRealSize(), ESP.getFlashChipSize(),
           ESP.getFlashChipSpeed() / 1000000, ESP.getFlashChipSizeByChipId(),
           ESP.getSketchSize(), ESP.getFreeSketchSpace(),
           ESP.getResetInfo().c_str());
  return String(buf);
}

String __dump_fs() {
  char buf[1024];
  FSInfo fs_info;
  LittleFS.info(fs_info);
  snprintf(buf, sizeof(buf), FS_INFO, fs_info.totalBytes, fs_info.usedBytes,
           fs_info.blockSize, fs_info.pageSize, fs_info.maxOpenFiles,
           fs_info.maxPathLength);
  return String(buf);
}

String __dump_config() {
  char buf[1024];
  snprintf(buf, sizeof(buf), config_INFO, config.sign, config.checksum,
           config.device_name, config.mqtt_server_ip, config.mqtt_server_port,
           config.mqtt_server_username, config.mqtt_server_password);
  return String(buf);
}

void dump_esp8266() { Serial.print(__dump_esp8266()); }
void dump_fs() { Serial.print(__dump_fs()); }
void dump_config() { Serial.print(__dump_config()); }

String html_dump_esp8266() {
  String s = __dump_esp8266();
  s.replace("\n", "<br>");
  return s;
}

String html_dump_fs() {
  String s = __dump_fs();
  s.replace("\n", "<br>");
  return s;
}

String html_dump_config() {
  String s = __dump_config();
  s.replace("\n", "<br>");
  return s;
}
