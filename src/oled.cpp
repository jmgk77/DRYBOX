
#include "main.h"

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