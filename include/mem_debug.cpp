#include "main.h"

#define MEMDEBUG_TIME 60

uint32_t md_initial_heap;
bool __mem_debug = false;
Ticker md;

void __md_callback() { __mem_debug = true; }

void init_memdebug() {
  md.attach_scheduled(MEMDEBUG_TIME, __md_callback);

  LOG_MSG("MEMDEBUG OK");
}

void handle_memdebug() {
  if (__mem_debug) {
    LOG_MSG("[total: %u]\t[free: %u]\t[frag: %u%%]\t[max: %u]", md_initial_heap,
            ESP.getFreeHeap(), ESP.getHeapFragmentation(),
            ESP.getMaxFreeBlockSize());
    __mem_debug = false;
  }
}
