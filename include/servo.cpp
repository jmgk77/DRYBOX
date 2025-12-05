#include "main.h"

#define SERVO_PIN 14  // D5

#define SERVO_ON_POS 0
#define SERVO_OFF_POS 45

Servo vent;

void servo_on() {
  vent.write(SERVO_ON_POS);
  LOG_MSG("VENT OPEN");
}

void servo_off() {
  vent.write(SERVO_OFF_POS);
  LOG_MSG("VENT CLOSED");
}

bool get_servo_status() { return vent.read() == SERVO_ON_POS; }

void init_servo() {
  vent.attach(SERVO_PIN);

  servo_off();

  LOG_MSG("SERVO OK");
}
