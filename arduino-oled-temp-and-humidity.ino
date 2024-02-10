#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Wire.h>
#include <U8g2lib.h>
#include "ClosedCube_HDC1080.h"

#define LCDWidth u8g2.getDisplayWidth()
#define ALIGN_CENTER(t) ((LCDWidth - (u8g2.getUTF8Width(t))) / 2)

#define SDA 14
#define SCL 12

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /*clock=*/12, /*data=*/14, U8X8_PIN_NONE);
ClosedCube_HDC1080 hdc1080;

const char* ssid = "SSID";
const char* password = "Password";

unsigned long previousMillis = 0;
unsigned long interval = 30000;

char temp[6];
char humidity[7];

float tempValue;
float humidityValue;

const char* wl_status_to_string(wl_status_t status) {
  switch (status) {
    case WL_NO_SHIELD: return "WL_NO_SHIELD";
    case WL_IDLE_STATUS: return "WL_IDLE_STATUS";
    case WL_NO_SSID_AVAIL: return "WL_NO_SSID_AVAIL";
    case WL_SCAN_COMPLETED: return "WL_SCAN_COMPLETED";
    case WL_CONNECTED: return "WL_CONNECTED";
    case WL_CONNECT_FAILED: return "WL_CONNECT_FAILED";
    case WL_CONNECTION_LOST: return "WL_CONNECTION_LOST";
    case WL_DISCONNECTED: return "WL_DISCONNECTED";
    default: return "UNKNOWN_STATUS";
  }
}

ESP8266WebServer server(80);

// Define routing
void restServerRouting() {
  server.on("/", HTTP_GET, []() {
    String response = "{\"temp\": \"" + String(tempValue) + "\", \"humidity\": \"" + String(humidityValue) + "\"}";
    server.enableCORS(true);
    server.send(200, "application/json", response);
});
}

// Manage not found URL
void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
}

void setup(void) {
  Serial.begin(115200);

  initWiFi();

  // Set server routing
  restServerRouting();
  // Set not found response
  server.onNotFound(handleNotFound);
  // Start server
  server.begin();
  Serial.println("HTTP server started");

  u8g2.begin();
  Wire.begin(SDA, SCL);
  hdc1080.begin(0x40);
}

void loop(void) {
  //print the Wi-Fi status every 30 seconds
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    Serial.print("Connection status: ");
    Serial.println(wl_status_to_string(WiFi.status()));
    previousMillis = currentMillis;
  }
  server.handleClient();

  // Read temperature and humidity
  tempValue = hdc1080.readTemperature();
  humidityValue = hdc1080.readHumidity();

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

  delay(5000);

  // Display humidity
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_unifont_te);
  u8g2.drawUTF8(0, 16, "Õhuniiskus:");

  u8g2.setFont(u8g2_font_logisoso38_tf);
  u8g2.drawUTF8(ALIGN_CENTER(humidity), 60, humidity);
  u8g2.sendBuffer();

  delay(5000);
}
