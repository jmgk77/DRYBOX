#include "main.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define CLEARSCREEN_DELAY 5

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void init_oled() {
  // init screen
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
#ifdef DEBUG
    Serial.println("* OLED NOK");
#endif
  }
#ifdef DEBUG
  Serial.println("* OLED OK");
#endif

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

void oled_th_status() {
  display.clearDisplay();

  // Print Temperature
  display.setCursor(0, 10);
  display.print(get_temperature(), 1);  // Print float with 1 decimal place
  display.print(" C");

  // Print Humidity
  display.setCursor(0, 30);
  display.print(get_humidity(), 1);  // Print float with 1 decimal place
  display.print(" %");

  //***show cycle status
  // show cycle profile

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