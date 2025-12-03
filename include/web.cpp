#include "main.h"

DNSServer dns;
AsyncWebServer server(80);
AsyncWiFiManager wm(&server, &dns);
ESPAsyncHTTPUpdateServer updateServer;

// Declarações de função (Forward declarations) para resolver dependências
void start_dry_cycle();
void stop_dry_cycle();
String get_dry_cycle_state_str();
String get_remaining_time_str();
void servo_on();
void servo_off();
bool get_servo_status();
const char* get_current_profile_name();

String __add_buttons() { return String(html_buttons); }

String get_vent_state_str() {
  return get_servo_status() ? "ABERTA" : "FECHADA";
}

void __handle_root(AsyncWebServerRequest* request) {
  AsyncResponseStream* response = request->beginResponseStream("text/html");

  response->print(html_header);

  String root_page = html_root;
  root_page.replace("%PROFILE_NAME%", get_current_profile_name());
  root_page.replace("%HEATER_STATE%", get_heater() ? "ON" : "OFF");
  root_page.replace("%FAN_STATE%", get_fan() ? "ON" : "OFF");
  root_page.replace("%STATE%", get_dry_cycle_state_str());
  root_page.replace("%TIME%", get_remaining_time_str());
  root_page.replace("%VENT_STATE%", get_vent_state_str());
  response->print(root_page);

  bool is_running = (get_dry_cycle_state() != DryCycleState::IDLE &&
                     get_dry_cycle_state() != DryCycleState::DONE);

  String profile_options;
  for (int i = 0; i < num_profiles; i++) {
    profile_options += "<option value=\"" + String(i) + "\">" +
                       String(dry_profiles[i].nome) + "</option>";
  }
  String commands_html = html_commands;
  commands_html.replace("%PROFILE_OPTIONS%", profile_options);

  commands_html.replace("%DISABLED%", is_running ? "disabled" : "");
  commands_html.replace("%START_DISABLED%", is_running ? "disabled" : "");
  commands_html.replace("%STOP_DISABLED%", !is_running ? "disabled" : "");

  response->print(commands_html);

  response->print(html_buttons);

  // generate js t
  response->print("<script>const t=[");

  // Read historical data from the binary log file
  File f = LittleFS.open(th_log_name, "r");
  bool first = true;
  if (f) {
    // Pula para a posição onde começam os últimos 180 registros, a partir do
    // final do arquivo
    f.seek(-sizeof(TH_INFO) * 180, SeekEnd);

    TH_INFO history_buffer;
    while (f.read((uint8_t*)&history_buffer, sizeof(TH_INFO)) ==
           sizeof(TH_INFO)) {
      if (!first) response->print(",");
      response->print(history_buffer.temperature);
      first = false;
    }
    f.close();
  }

  // Append last sensor reads from memory
  if (th_index > 0) {
    for (unsigned int i = 0; i < th_index; i++) {
      if (!first) response->print(",");
      response->print(th_info[i].temperature);
      first = false;
    }
  }

  // generate js h
  response->print("];const h=[");

  // Read historical data from the binary log file
  f = LittleFS.open(th_log_name, "r");
  first = true;
  // We need to re-read the data for humidity. This is not ideal but avoids
  // storing all data in memory. A better long-term solution might be to
  // restructure the data or the page logic.
  if (f) {
    f.seek(-sizeof(TH_INFO) * 180, SeekEnd);
    TH_INFO history_buffer;
    while (f.read((uint8_t*)&history_buffer, sizeof(TH_INFO)) ==
           sizeof(TH_INFO)) {
      if (!first) response->print(",");
      response->print(history_buffer.humidity);
      first = false;
    }
    f.close();
  }
  if (th_index > 0) {
    for (unsigned int i = 0; i < th_index; i++) {
      if (!first) response->print(",");
      response->print(th_info[i].humidity);
      first = false;
    }
  }
  response->print("];");

  response->print(html_js);

  response->print(html_footer);

  // send root page
  request->send(response);
}

void __handle_info(AsyncWebServerRequest* request) {
  AsyncResponseStream* response = request->beginResponseStream("text/html");
  response->print(html_header);

  response->print("<div>");
  response->printf("HEATER: %s<br>", get_heater() ? "ON" : "OFF");
  response->printf("FAN: %s<br>", get_fan() ? "ON" : "OFF");
  response->printf("TEMPERATURE: %f<br>", get_temperature());
  response->printf("HUMIDITY: %f<br>", get_humidity());
  response->print("<br></div>");

  response->printf("IP: <i>%s</i><br>\n", WiFi.localIP().toString().c_str());
  response->printf("Data de ínicio: <i>%s</i><br>\n", boot_time);
  response->printf("Versão: %s", VERSION);
#ifdef DEBUG
  response->print("<FONT color=red><b> DEBUG</b></FONT>");
#endif
  response->print("<br><br>\n");

  response->print(html_dump_esp8266());
  response->print(html_dump_config());
  response->print(html_dump_fs());

  response->print(__add_buttons());
  response->print(html_footer);
  request->send(response);
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
  if (request->hasParam("start_cycle")) {
    if (request->hasParam("profile")) {
      int profile_index = request->getParam("profile")->value().toInt();
      set_current_profile_by_index(profile_index);
    }
    start_dry_cycle();
  } else if (request->hasParam("stop_cycle")) {
    stop_dry_cycle();
  } else if (request->hasParam("fan_off")) {
    fan_off();
  } else if (request->hasParam("fan_on")) {
    fan_on();
  } else if (request->hasParam("heater_off")) {
    heater_off();
  } else if (request->hasParam("heater_on")) {
    heater_on();
  } else if (request->hasParam("vent_open")) {
    servo_on();
  } else if (request->hasParam("vent_close")) {
    servo_off();
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
