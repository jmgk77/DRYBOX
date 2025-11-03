#include "main.h"

#define BUTTON_PIN 0  // D3, GPIO0

OneButton button;

void __single_click() { oled_status = true; }

void __double_click() {
  //***change current profile
  oled_status = true;
}

void __long_press() {
  if (current_dry_state == DryCycleState::IDLE ||
      current_dry_state == DryCycleState::DONE) {
    start_dry_cycle();
  } else {
    stop_dry_cycle();
  }
}

void init_button() {
  // Initialize button on GPIO0, with internal pull-up, and active low.
  button.setup(BUTTON_PIN, INPUT_PULLUP, true);
  button.attachClick(__single_click);
  button.attachDoubleClick(__double_click);
  button.attachLongPressStart(__long_press);
}

void handle_button() { button.tick(); }