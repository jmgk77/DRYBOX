#include "main.h"

const char ESP_INFO[] PROGMEM =
    "ESP8266 - "
    "ESP.getFreeHeap(): %d, "
    "ESP.getChipId(): %08X, "
    "ESP.getSdkVersion(): %s, "
    "ESP.getBootVersion(): %d, "
    "ESP.getBootMode(): %d, "
    "ESP.getCpuFreqMHz(): %d, "
    "ESP.getFlashChipId(): %08X, "
    "ESP.getFlashChipRealSize(): %d, "
    "ESP.getFlashChipSize(): %d, "
    "ESP.getFlashChipSpeed(): %dMHz, "
    "ESP.getFlashChipSizeByChipId(): %d, "
    "ESP.getSketchSize(): %d, "
    "ESP.getFreeSketchSpace(): %d, "
    "ESP.getResetInfo(): %s";

const char FS_INFO[] PROGMEM =
    "FS - "
    "FS.totalBytes(): %d, "
    "FS.usedBytes(): %d, "
    "FS.blockSize(): %d, "
    "FS.pageSize(): %d, "
    "FS.maxOpenFiles(): %d, "
    "FS.maxPathLength(): %d";

const char config_INFO[] PROGMEM =
    "CONFIG - "
    "config.device_name: %s, "
    "config.mqtt_server_ip: %s, "
    "config.mqtt_server_port: %d, "
    "config.mqtt_server_username: %s, "
    "config.mqtt_server_password: %s";

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
  snprintf(buf, sizeof(buf), config_INFO, config.device_name,
           config.mqtt_server_ip, config.mqtt_server_port,
           config.mqtt_server_username, config.mqtt_server_password);
  return String(buf);
}

void dump_esp8266() { LOG_MSG("%s", __dump_esp8266().c_str()); }
void dump_fs() { LOG_MSG("%s", __dump_fs().c_str()); }
void dump_config() { LOG_MSG("%s", __dump_config().c_str()); }

String html_dump_esp8266() {
  String s = __dump_esp8266();
  s.replace(",", "<br>");
  s.replace("-", "<br>");
  return s;
}

String html_dump_fs() {
  String s = __dump_fs();
  s.replace(",", "<br>");
  s.replace("-", "<br>");
  return s;
}

String html_dump_config() {
  String s = __dump_config();
  s.replace(",", "<br>");
  s.replace("-", "<br>");
  return s;
}
