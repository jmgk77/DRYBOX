#include "main.h"

#define BUTTON_PIN 0  // D3, GPIO0

OneButton button;

// Forward declarations
void next_profile();

void __single_click() {
  LOG_MSG(".");
  oled_status = true;
}

void __double_click() {
  LOG_MSG(".");
  next_profile();
  oled_status = true;
}

void __long_press() {
  LOG_MSG(".");
  if (current_dry_state == DryCycleState::IDLE ||
      current_dry_state == DryCycleState::DONE) {
    start_dry_cycle();
  } else {
    stop_dry_cycle();
  }
  oled_status = true;
}

void init_button() {
  // Initialize button on GPIO0, with internal pull-up, and active low.
  button.setup(BUTTON_PIN, INPUT_PULLUP, true);
  button.attachClick(__single_click);
  button.attachDoubleClick(__double_click);
  button.attachLongPressStart(__long_press);
  LOG_MSG("BUTTON OK");
}

void handle_button() { button.tick(); }