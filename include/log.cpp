#pragma once

#include <Arduino.h>
#include <time.h>

#ifdef DEBUG

// Helper to get basename from __FILE__
const char* __basename(const char* path) {
  const char* last_slash = strrchr(path, '/');
  if (last_slash) {
    return last_slash + 1;
  }
  return path;
}

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
}
#endif