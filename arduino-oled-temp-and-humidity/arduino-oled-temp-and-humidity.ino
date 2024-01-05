#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /*clock=*/ 12, /*data=*/ 14, U8X8_PIN_NONE);

void setup(void) {
  u8g2.begin();
}

void loop(void) {

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_unifont_te);
  u8g2.drawUTF8(0, 14, "Temperatuur:");

  u8g2.setFont(u8g2_font_logisoso38_tf);
  u8g2.drawUTF8(22, 60, "18°C");
  u8g2.sendBuffer();

  delay(2000);

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_unifont_te);
  u8g2.drawUTF8(0, 16, "Õhuniiskus:");

  u8g2.setFont(u8g2_font_logisoso38_tf);
  u8g2.drawUTF8(30, 60, "46%");
  u8g2.sendBuffer();

  delay(2000);
}