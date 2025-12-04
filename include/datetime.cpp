#include "main.h"

Ticker get_time;
char boot_time[32];

// Forward declaration
void init_logger();
void init_memdebug();

void __get_time_callback() {
  // year is past 2021?
  if (time(nullptr) > 1609459200) {
    // get boot time
    time_t t = time(NULL);
    strncpy(boot_time, ctime(&t), sizeof(boot_time));
#ifdef DEBUG
    Serial.print("* NTTP OK\n  Current date: ");
    Serial.print(boot_time);
#endif
    // Time is synced, now we can initialize the loggers with a correct
    // timestamp.
    init_logger();
#ifdef DEBUG_MEM
    init_memdebug();
#endif
    // Stop polling
    get_time.detach();  // Stop polling
  }
}

void init_time() {
  // get internet time (GMT-3)
  configTime("<-03>3", "pool.ntp.org");

  memset(boot_time, 0, sizeof(boot_time));

  get_time.attach_ms(1000, __get_time_callback);
}
