#pragma once

#include <Arduino.h>
#include <time.h>

#define LOG_TO_FILE
#define LOG_FILENAME "/debug.log"
#define MAX_LOG_FILES 5

#ifdef DEBUG

// Helper to get basename from __FILE__
// =================================================================================================
// __basename
// =================================================================================================
const char* __basename(const char* path) {
  const char* last_slash = strrchr(path, '/');
  if (last_slash) {
    return last_slash + 1;
  }
  return path;
}

// =================================================================================================
// init_file_logging
// =================================================================================================
#ifdef LOG_TO_FILE
void init_file_logging() {
  // Remove oldest
  String oldest = String(LOG_FILENAME) + "." + String(MAX_LOG_FILES - 1);
  if (LittleFS.exists(oldest)) LittleFS.remove(oldest);

  // Rotate
  for (int i = MAX_LOG_FILES - 2; i >= 0; i--) {
    String src = String(LOG_FILENAME) + (i == 0 ? "" : "." + String(i));
    String dst = String(LOG_FILENAME) + "." + String(i + 1);
    if (LittleFS.exists(src)) {
      LittleFS.rename(src, dst);
    }
  }
}
#endif

// =================================================================================================
// log_message
// =================================================================================================
void log_message(const char* file, int line, const char* func, const char* fmt,
                 ...) {
  char timestamp[20];
  time_t t = time(nullptr);
  strftime(timestamp, sizeof(timestamp), "%H:%M:%S", localtime(&t));

  Serial.printf("%s [%s:%d %s()] ", timestamp, __basename(file), line, func);

  char buf[1024];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buf, sizeof(buf), fmt, args);
  va_end(args);

  Serial.println(buf);

#ifdef LOG_TO_FILE
  File f = LittleFS.open(LOG_FILENAME, "a");
  if (f) {
    f.printf("%s [%s:%d %s()] %s\n", timestamp, __basename(file), line, func,
             buf);
    f.close();
  }
#endif
}
#endif