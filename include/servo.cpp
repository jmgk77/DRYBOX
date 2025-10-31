#include "main.h"

#define SERVO_PIN 14  // D5

#define SERVO_ON_POS 0
#define SERVO_OFF_POS 20

Servo vent;

void servo_on() {
  vent.write(SERVO_ON_POS);
#ifdef DEBUG
  Serial.println("! Servo ON");
#endif
}

void servo_off() {
  vent.write(SERVO_OFF_POS);
#ifdef DEBUG
  Serial.println("! Servo OFF");
#endif
}

bool get_servo_status() { return vent.read() == SERVO_ON_POS; }

void init_servo() {
  vent.attach(SERVO_PIN);

  servo_off();

#ifdef DEBUG
  Serial.println("* SERVO OK");
#endif
}

