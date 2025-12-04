#include "main.h"

#define MEMDEBUG_TIME 60

char memdebug_log_name[64];
uint32_t md_initial_heap;
bool __mem_debug = false;
Ticker md;

void __md_callback() { __mem_debug = true; }

void init_memdebug() {
  __mem_debug = false;

  // create mem log name
  struct tm now;
  time_t t = time(NULL);
  localtime_r(&t, &now);
  strftime(memdebug_log_name, sizeof(memdebug_log_name), "/%Y%m%d_%H%M.log",
           &now);

  // Start the periodic logging
  md.attach_scheduled(MEMDEBUG_TIME, __md_callback);

#ifdef DEBUG
  Serial.println("* MEMDEBUG OK (" + String(memdebug_log_name) + ")");
#endif
}

void handle_memdebug() {
  if (__mem_debug) {
    __mem_debug = false;

    struct tm now;
    time_t t = time(NULL);
    localtime_r(&t, &now);

    File f = LittleFS.open(memdebug_log_name, "a");
    if (f) {
      f.printf("[%d:%d:%d]\t[total: %u]\t[free: %u]\t[frag: %u%%]\t[max: %u]\n",
               now.tm_hour, now.tm_min, now.tm_sec, md_initial_heap,
               ESP.getFreeHeap(), ESP.getHeapFragmentation(),
               ESP.getMaxFreeBlockSize());
      f.close();
    }
  }
}