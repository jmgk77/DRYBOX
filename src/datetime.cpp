#include "main.h"

Ticker get_time;
char boot_time[32];

void __get_time_callback() {
  // year is past 2021?
  if (time(nullptr) > 1609459200) {
    // get boot time
    time_t t = time(NULL);
    strncpy(boot_time, ctime(&t), sizeof(boot_time));
#ifdef DEBUG
    Serial.print("Current date: ");
    Serial.print(boot_time);
#endif

  } else {
    // reschedule
    get_time.once_ms_scheduled(200, __get_time_callback);
  }
}

void init_time() {
  // get internet time (GMT-3)
  configTime("<-03>3", "pool.ntp.org");

  memset(boot_time, 0, sizeof(boot_time));

  get_time.once_ms_scheduled(1000, __get_time_callback);
}
