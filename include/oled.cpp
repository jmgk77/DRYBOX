#include "main.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define CLEARSCREEN_DELAY 5

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void init_oled() {
  // init screen
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    LOG_MSG("OLED NOK");
  } else {
    LOG_MSG("OLED OK");
  }
  // draw logo
  display.clearDisplay();
  display.drawXBitmap((display.width() - logo_width) / 2,
                      (display.height() - logo_height) / 2, logo_bits,
                      logo_width, logo_height, WHITE);
  display.display();
  delay(1000);

  // setup screen
  display.setTextSize(1);
  display.setTextColor(WHITE);
}

void oled_clear() {
  display.clearDisplay();
  display.display();
}

// Forward declarations for functions used from other files
const char* get_current_profile_name();
String get_dry_cycle_state_str();
String get_remaining_time_str();
DryCycleState get_dry_cycle_state();

void oled_th_status() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  // --- Profile Name (Top, centered) ---
  const char* profile_name = get_current_profile_name();
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(profile_name, 0, 0, &x1, &y1, &w, &h);
  display.setCursor((SCREEN_WIDTH - w) / 2, 0);
  display.println(profile_name);

  // --- Cycle Status (Middle, large font, centered) ---
  String status_str = get_dry_cycle_state_str();
  display.setTextSize(2);
  display.getTextBounds(status_str.c_str(), 0, 0, &x1, &y1, &w, &h);
  display.setCursor((SCREEN_WIDTH - w) / 2, 16);
  display.println(status_str);
  display.setTextSize(1);

  // --- Remaining Time (Below status, centered) ---
  String time_str = get_remaining_time_str();
  display.getTextBounds(time_str.c_str(), 0, 0, &x1, &y1, &w, &h);
  display.setCursor((SCREEN_WIDTH - w) / 2, 35);
  display.println(time_str);

  // --- Temperature and Humidity (Bottom) ---
  char th_buf[20];

  // Temperature (Bottom-left)
  snprintf(th_buf, sizeof(th_buf), "T: %.1f C", get_temperature());
  display.setCursor(0, SCREEN_HEIGHT - 8);
  display.print(th_buf);

  // Humidity (Bottom-right)
  snprintf(th_buf, sizeof(th_buf), "H: %.1f %%", get_humidity());
  display.getTextBounds(th_buf, 0, 0, &x1, &y1, &w, &h);
  display.setCursor(SCREEN_WIDTH - w, SCREEN_HEIGHT - 8);
  display.print(th_buf);

  display.display();
}

bool oled_status = false;

long clear_screen = 0;

void handle_oled() {
  if (oled_status) {
    oled_status = false;
    oled_th_status();
    clear_screen = millis();
  }

  // pending screen off?
  if ((clear_screen != 0) &&
      (millis() - clear_screen > CLEARSCREEN_DELAY * 1000)) {
    clear_screen = 0;
    oled_clear();
  }
}