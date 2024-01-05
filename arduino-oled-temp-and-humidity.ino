#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include "ClosedCube_HDC1080.h"

#define LCDWidth u8g2.getDisplayWidth()
#define ALIGN_CENTER(t) ((LCDWidth - (u8g2.getUTF8Width(t))) / 2)

#define SDA 14
#define SCL 12

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /*clock=*/ 12, /*data=*/ 14, U8X8_PIN_NONE);
ClosedCube_HDC1080 hdc1080;

char temp[10];
char humidity[10];

void setup(void) {
  u8g2.begin();
  Wire.begin(SDA, SCL);
  hdc1080.begin(0x40);
}

void loop(void) {
  // Read temperature and humidity
  float tempValue = hdc1080.readTemperature();
  float humidityValue = hdc1080.readHumidity();

  // Format and round to 1 decimal place
  dtostrf(tempValue, 3, 1, temp);
  dtostrf(humidityValue, 3, 1, humidity);

  // Append units
  strcat(temp, "°C");
  strcat(humidity, "%");

  // Display temperature
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_unifont_te);
  u8g2.drawUTF8(0, 14, "Temperatuur:");

  u8g2.setFont(u8g2_font_logisoso38_tf);
  u8g2.drawUTF8(ALIGN_CENTER(temp), 60, temp);
  u8g2.sendBuffer();

  delay(2000);

  // Display humidity
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_unifont_te);
  u8g2.drawUTF8(0, 16, "Õhuniiskus:");

  u8g2.setFont(u8g2_font_logisoso38_tf);
  u8g2.drawUTF8(ALIGN_CENTER(humidity), 60, humidity);
  u8g2.sendBuffer();

  delay(2000);
}
