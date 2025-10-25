#include "main.h"

DNSServer dns;
AsyncWebServer server(80);
AsyncWiFiManager wm(&server, &dns);
ESPAsyncHTTPUpdateServer updateServer;

extern struct config_data config;
extern char boot_time[32];
extern bool oled_update_pending;

String __add_buttons() { return String(html_buttons); }

String __get_status() {
  String s = "<div>";
  s += "HEATER:" + String(get_heater() ? "ON" : "OFF") + "<br>";
  s += "FAN:" + String(get_fan() ? "ON" : "OFF") + "<br>";
  s += "TEMPERATURE:" + String(get_temperature()) + "<br>";
  s += "HUMIDITY:" + String(get_humidity()) + "<br>";
  s += "<br></div>";
  return s;
}

void __handle_root(AsyncWebServerRequest* request) {
  String s;

  s = +"<script>const t = [";
  // for (int i = 0; i < count; i++) {
  //   snprintf_P(buf, sizeof(buf), "%.01f,", th_info[start + i].temperature);
  //   server.sendContent(buf);
  // }
  s += "];\nconst h = [";
  // for (int i = 0; i < count; i++) {
  //   snprintf_P(buf, sizeof(buf), "%.01f,", th_info[start + i].humidity);
  //   server.sendContent(buf);
  // }

  s += "];";

  s += String(html_js);

  //***
  s += String(html_commands);

  // buttons
  s += __add_buttons();
  // send root page
  request->send(200, "text/html", html_header + s + html_footer);
}

void __handle_info(AsyncWebServerRequest* request) {
  String s;

  s += __get_status();
  //
  s += "IP: <i>" + WiFi.localIP().toString() + "</i><br>\n";
  s += "Data de ínicio: <i>" + String(boot_time) + "</i><br>\n";
  // version
  s += "Versão: " + String(VERSION) +
#ifdef DEBUG
       "<FONT color=red><b> DEBUG</b></FONT>" +
#endif
       "<br><br>\n";

  s += html_dump_esp8266();
  s += html_dump_config();
  s += html_dump_fs();

  // buttons
  s += __add_buttons();
  // send root page
  request->send(200, "text/html", html_header + s + html_footer);
}

void __handle_config(AsyncWebServerRequest* request) {
  //
  if (request->hasParam("s", true)) {
    // read options
    FORM_SAVE_STRING(config.device_name)
    FORM_SAVE_STRING(config.mqtt_server_ip)
    FORM_SAVE_INT(config.mqtt_server_port)
    FORM_SAVE_STRING(config.mqtt_server_username)
    FORM_SAVE_STRING(config.mqtt_server_password)
    // save data to config
    save_config();
    dump_config();
    request->send(200, "text/html",
                  "<meta http-equiv='refresh' content='0; url=/config'/>");
  } else {
    String s;
    FORM_START("/config")
    FORM_ASK_VALUE(config.device_name, "Device name")
    FORM_ASK_VALUE(config.mqtt_server_ip, "MQTT Broker fixed IP")
    FORM_ASK_VALUE(config.mqtt_server_port, "MQTT Broker Port")

    FORM_ASK_VALUE(config.mqtt_server_username, "MQTT remote username")
    FORM_ASK_VALUE(config.mqtt_server_password, "MQTT remote password")

    FORM_END("SALVAR")

    // update
    s +=
        "<br>\n<form action='/update?name=firmware' "
        "enctype=multipart/form-data "
        "method=POST>Firmware:<br><input type=file accept=.bin,.bin.gz "
        "name=firmware> <input type=submit value='Update Firmware'></form><br>";

    // buttons
    s += __add_buttons();

    // send config page
    request->send(200, "text/html", html_header + s + html_footer);
  }
}

void __handle_command(AsyncWebServerRequest* request) {
  if (request->hasParam("status")) {
    //
    oled_update_pending = true;
  } else if (request->hasParam("fan_off")) {
    fan_off();
  } else if (request->hasParam("fan_on")) {
    fan_on();
  } else if (request->hasParam("heater_off")) {
    heater_off();
  } else if (request->hasParam("heater_on")) {
    heater_on();
  } else {  // default
  }

  request->send(200, "text/html",
                "<meta http-equiv='refresh' content='0; url=/' />");
}

void __handle_reboot(AsyncWebServerRequest* request) {
  request->send(200, "text/html",
                "<meta http-equiv='refresh' content='15; url=/' />");
  delay(1 * 1000);
  ESP.restart();
  delay(2 * 1000);
}

void __handle_reset(AsyncWebServerRequest* request) {
  // erase config
  default_config();
  save_config();
  // reset wifi
  wm.resetSettings();
  __handle_reboot(request);
}

void __handle_404(AsyncWebServerRequest* request) {
  request->send(404, "text/plain", "Not found");
}

#ifdef WWW_FILESERVER

String __b_kb_mb_gb(const size_t bytes) {
  if (bytes < 1024)
    return String(bytes) + " B";
  else if (bytes < (1024 * 1024))
    return String(bytes / 1024.0) + " KB";
  else if (bytes < (1024 * 1024 * 1024))
    return String(bytes / 1024.0 / 1024.0) + " MB";
  else
    return String(bytes / 1024.0 / 1024.0 / 1024.0) + " GB";
}

void __handle_files(AsyncWebServerRequest* request) {
  if (request->hasParam("n")) {
    // download
    String fileName = request->getParam("n")->value();
#ifdef DEBUG
    Serial.printf("### Download: %s\n", fileName.c_str());
#endif
    request->send(LittleFS, fileName, "application/octet-stream");
  } else if (request->hasParam("x")) {
    // delete
    String fileName = request->getParam("x")->value();
#ifdef DEBUG
    Serial.printf("### Delete: %s\n", fileName.c_str());
#endif
    LittleFS.remove(fileName);
    request->redirect("/files");
  } else {
    // dir
#ifdef DEBUG
    Serial.println("### Dir:\n");
#endif
    String s;
    s += "<div style='border: 1px solid black'>\n<div style='border: 1px solid "
         "black'>\n";
    // scan files
    Dir dir = LittleFS.openDir("");
    while (dir.next()) {
      if (dir.isFile()) {
#ifdef DEBUG
        Serial.printf("### \t%s\n", dir.fileName().c_str());
#endif
        s += "<a download='" + dir.fileName() +
             "' href='files?n=" + dir.fileName() + "'>" + dir.fileName() +
             "</a>";
        s += "    (" + __b_kb_mb_gb(dir.fileSize()) + ")    ";
        const time_t t = dir.fileTime();
        s += String(ctime(&t));
        s += "<a href='files?x=" + dir.fileName() + "'>x</a><br>\n";
      }
    }
    // upload form
    s += "</div>\n<form action='/upload' method='POST' "
         "enctype='multipart/form-data'><input type='file' name='name'><input "
         "class='button' type='submit' value='UPLOAD'></form>\n</div><br>";
    // buttons
    s += __add_buttons();
    // send dir page
    request->send(200, "text/html", html_header + s + html_footer);
  }
}

void __handle_upload(AsyncWebServerRequest* request, String filename,
                     size_t index, uint8_t* data, size_t len, bool final) {
  if (!index) {
#ifdef DEBUG
    Serial.printf("### UploadStart: %s\n", filename.c_str());
#endif
    request->_tempFile = LittleFS.open("/" + filename, "w");
  }
#ifdef DEBUG
  Serial.printf("### UploadMiddle: %u bytes\n", len);
#endif
  request->_tempFile.write(data, len);
  if (final) {
#ifdef DEBUG
    Serial.printf("### UploadEnd: %s size: %u\n", filename.c_str(),
                  index + len);
#endif
    request->_tempFile.close();
    request->redirect("/files");
  }
}

#endif

void init_web() {
  // connect to internet
  WiFi.mode(WIFI_STA);
  delay(10);
  wm.setDebugOutput(false);
  WiFi.hostname(config.device_name);
  wm.setConfigPortalTimeout(180);
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);

  // captive portal
  if (!wm.autoConnect(config.device_name)) {
    ESP.restart();
    delay(1 * 1000);
  }
#ifdef DEBUG
  Serial.println("* WIFI OK\n  Got IP: " + WiFi.localIP().toString());
#endif

  // install www handlers
  server.onNotFound(__handle_404);
  server.on("/", HTTP_ANY, __handle_root);
  server.on("/config", HTTP_ANY, __handle_config);
  server.on("/info", HTTP_ANY, __handle_info);
  server.on("/reboot", HTTP_ANY, __handle_reboot);
  server.on("/reset", HTTP_ANY, __handle_reset);
  server.on("/command", HTTP_ANY, __handle_command);
#ifdef WWW_FILESERVER
  server.on("/files", HTTP_ANY, __handle_files);
  server.on(
      "/upload", HTTP_POST,
      [](AsyncWebServerRequest* request) { request->send(200); },
      __handle_upload);
#endif
  updateServer.setup(&server, "/update");
  server.begin();

  // discovery protocols
  MDNS.begin(config.device_name);
  MDNS.addService("http", "tcp", 80);

#ifdef ENABLE_EXTRA_DISCOVERY
  LLMNR.begin(config.device_name);
  NBNS.begin(config.device_name);
#endif
}

void handle_web() { MDNS.update(); }
