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
float get_weight();
void tare_loadcell();

String __add_buttons() { return FPSTR(html_buttons); }

String get_vent_state_str() {
  return get_servo_status() ? "ABERTA" : "FECHADA";
}

void __handle_root(AsyncWebServerRequest* request) {
  LOG_MSG(".");

  AsyncResponseStream* response = request->beginResponseStream("text/html");
  response->print(FPSTR(html_header));

  // send page
  String root_page = FPSTR(html_root);
  root_page.replace("%PROFILE_NAME%", get_current_profile_name());
  root_page.replace("%HEATER_STATE%", get_heater() ? "ON" : "OFF");
  root_page.replace("%FAN_STATE%", get_fan() ? "ON" : "OFF");
  root_page.replace("%STATE%", get_dry_cycle_state_str());
  root_page.replace("%TIME%", get_remaining_time_str());
  root_page.replace("%VENT_STATE%", get_vent_state_str());
  response->print(root_page);

  // build profiles
  String profile_options;
  for (int i = 0; i < num_profiles; i++) {
    profile_options += "<option value=\"" + String(i) + "\"";
    if (i == current_profile_index) {
      profile_options += " selected";
    }
    profile_options += ">" + String(dry_profiles[i].nome) + "</option>";
  }
  String commands_html = FPSTR(html_commands);
  commands_html.replace("%PROFILE_OPTIONS%", profile_options);

  // disable buttons
  bool is_running = (get_dry_cycle_state() != DryCycleState::IDLE &&
                     get_dry_cycle_state() != DryCycleState::DONE);

  commands_html.replace("%DISABLED%", is_running ? "disabled" : "");
  commands_html.replace("%START_DISABLED%", is_running ? "disabled" : "");
  commands_html.replace("%STOP_DISABLED%", !is_running ? "disabled" : "");

  // send commands and buttons
  response->print(commands_html);
  response->print(FPSTR(html_buttons));

  // gen js
  response->print("<script>");

  bool first = true;
  response->print("const t=[");
  File f = LittleFS.open(th_log_name, "r");
  if (f) {
    // Seek to the position of the last TH_HISTORY records from the end of the
    // file
    size_t history_size = sizeof(TH_INFO) * TH_HISTORY;
    if (f.size() > history_size) {
      f.seek(f.size() - history_size, SeekSet);
    }

    TH_INFO history_buffer;
    while (f.read((uint8_t*)&history_buffer, sizeof(TH_INFO)) ==
           sizeof(TH_INFO)) {
      if (!first) response->print(",");
      response->print(history_buffer.temperature);
      first = false;
    }
    f.close();
  }

  // Append recent sensor reads from the memory buffer
  if (th_index > 0) {
    for (unsigned int i = 0; i < th_index; i++) {
      if (!first) response->print(",");
      response->print(th_info[i].temperature);
      first = false;
    }
  }

  first = true;
  response->print("];const h=[");

  // Re-open file and print humidity data
  f = LittleFS.open(th_log_name, "r");
  if (f) {
    size_t history_size = sizeof(TH_INFO) * TH_HISTORY;
    if (f.size() > history_size) {
      f.seek(f.size() - history_size, SeekSet);
    }

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

  response->print(FPSTR(html_js));
  response->print(FPSTR(html_footer));

  // send root page
  request->send(response);
}

void __handle_info(AsyncWebServerRequest* request) {
  LOG_MSG(".");

  AsyncResponseStream* response = request->beginResponseStream("text/html");
  response->print(FPSTR(html_header));

  response->print("<div>");
  response->printf("HEATER: %s<br>", get_heater() ? "ON" : "OFF");
  response->printf("FAN: %s<br>", get_fan() ? "ON" : "OFF");
  response->printf("TEMPERATURE: %.2f<br>", get_temperature());
  response->printf("HUMIDITY: %.2f<br>", get_humidity());
  response->printf("WEIGHT: %.2f g<br>", get_weight());
  response->print("<br></div>");

  response->printf("IP: <i>%s</i><br>\n", WiFi.localIP().toString().c_str());
  response->printf("Data de ínicio: <i>%s</i><br>\n", boot_time);
  response->printf("Versão: %s", VERSION);
#ifdef DEBUG
  response->print("<FONT color=red><b> DEBUG</b></FONT>");
#endif
  response->print("<br><br>\n");

  response->print(html_dump_esp8266());
  response->print("<br><br>\n");

  response->print(html_dump_config());
  response->print("<br><br>\n");

  response->print(html_dump_fs());
  response->print("<br><br>\n");

  response->print(__add_buttons());
  response->print(FPSTR(html_footer));
  request->send(response);
}

void __handle_config(AsyncWebServerRequest* request) {
  LOG_MSG(".");

  //
  if (request->hasParam("s", true)) {
    // read options
    FORM_SAVE_STRING(config.device_name)
    FORM_SAVE_STRING(config.mqtt_server_ip)
    FORM_SAVE_INT(config.mqtt_server_port)
    FORM_SAVE_STRING(config.mqtt_server_username)
    FORM_SAVE_STRING(config.mqtt_server_password)
    FORM_SAVE_FLOAT(config.loadcell_scale)
    FORM_SAVE_INT(config.loadcell_offset)
    FORM_SAVE_FLOAT(config.loadcell_temp_coeff)

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

    FORM_ASK_FLOAT(config.loadcell_scale, "Load Cell Scale", 6)
    FORM_ASK_VALUE(config.loadcell_offset, "Load Cell Offset")
    FORM_ASK_FLOAT(config.loadcell_temp_coeff, "Temp Coeff (g/C)", 6)
    FORM_END("SALVAR")

    s += "<br><a href='/command?tare=1' class='button-link'><button "
         "type='button'>AUTO TARE</button></a><br><br>";

    // update
    s += "<br>\n<form action='/update?name=firmware' "
         "enctype=multipart/form-data "
         "method=POST>Firmware:<br><input type=file accept=.bin,.bin.gz "
         "name=firmware> <input type=submit value='Update "
         "Firmware'></form><br>";

    // buttons
    s += __add_buttons();

    // send config page
    request->send(200, "text/html",
                  String(FPSTR(html_header)) + s + String(FPSTR(html_footer)));
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
  } else if (request->hasParam("tare")) {
    tare_loadcell();
  } else {  // default
  }

  request->send(200, "text/html",
                "<meta http-equiv='refresh' content='0; url=/' />");
}

void __handle_reboot(AsyncWebServerRequest* request) {
  LOG_MSG(".");

  request->send(200, "text/html",
                "<meta http-equiv='refresh' content='5; url=/' />");
  delay(1 * 1000);
  ESP.restart();
  delay(2 * 1000);
}

void __handle_reset(AsyncWebServerRequest* request) {
  LOG_MSG(".");

  // erase config
  default_config();
  save_config();
  // reset wifi
  wm.resetSettings();
  __handle_reboot(request);
}

void __handle_404(AsyncWebServerRequest* request) {
  LOG_MSG(".");

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
  LOG_MSG(".");

  if (request->hasParam("n")) {
    // download
    String fileName = request->getParam("n")->value();
    LOG_MSG("Download: %s", fileName.c_str());
    request->send(LittleFS, fileName, "application/octet-stream");
  } else if (request->hasParam("x")) {
    // delete
    String fileName = request->getParam("x")->value();
    LOG_MSG("Delete: %s", fileName.c_str());
    LittleFS.remove(fileName);
    request->redirect("/files");
  } else {
    // dir
    LOG_MSG("Dir:");
    String s;
    s += "<div style='border: 1px solid black'>\n<div style='border: 1px "
         "solid "
         "black'>\n";
    // scan files
    Dir dir = LittleFS.openDir("");
    while (dir.next()) {
      if (dir.isFile()) {
        LOG_MSG("\t%s", dir.fileName().c_str());
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
    request->send(200, "text/html",
                  String(FPSTR(html_header)) + s + String(FPSTR(html_footer)));
  }
}

void __handle_upload(AsyncWebServerRequest* request, String filename,
                     size_t index, uint8_t* data, size_t len, bool final) {
  if (!index) {
    LOG_MSG("UploadStart: %s", filename.c_str());
    request->_tempFile = LittleFS.open("/" + filename, "w");
  }
  LOG_MSG("UploadMiddle: %u bytes", len);
  request->_tempFile.write(data, len);
  if (final) {
    LOG_MSG("UploadEnd: %s size: %u", filename.c_str(), index + len);
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
  LOG_MSG("WIFI OK. Got IP: %s", WiFi.localIP().toString().c_str());

  // install www handlers
  server.onNotFound(__handle_404);
  server.on("/", HTTP_ANY, __handle_root);
  server.on("/config", HTTP_ANY, __handle_config);
  server.on("/info", HTTP_ANY, __handle_info);
  server.on("/reboot", HTTP_ANY, __handle_reboot);
  server.on("/reset", HTTP_ANY, __handle_reset);
  server.on("/command", HTTP_ANY, __handle_command);
  server.on("/weight", HTTP_ANY, [](AsyncWebServerRequest* request) {
    char buf[16];
    snprintf(buf, sizeof(buf), "%.2f", get_weight());
    request->send(200, "text/plain", buf);
  });
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
