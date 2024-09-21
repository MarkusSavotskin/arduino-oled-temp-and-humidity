#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <U8g2lib.h>
#include "ClosedCube_HDC1080.h"

// OLED and I2C configuration
#define LCDWidth u8g2.getDisplayWidth()
#define ALIGN_CENTER(t) ((LCDWidth - (u8g2.getUTF8Width(t))) / 2)
#define SDA 14
#define SCL 12

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /*clock=*/SCL, /*data=*/SDA, U8X8_PIN_NONE);
ClosedCube_HDC1080 hdc1080;

// WiFi configuration
const char* ssid = "ssid";
const char* password = "password";

// MQTT configuration
const char* mqtt_server = "mqtt_server";
const char* mqtt_user = "mqtt_user";
const char* mqtt_password = "mqtt_password";
const char* temp_topic = "home/esp8266/temperature";
const char* humidity_topic = "home/esp8266/humidity";

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long previousMillis = 0;

char temp[6];
char humidity[7];

float tempValue;
float humidityValue;

// Non-blocking WiFi initialization
void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("Attempting to connect to WiFi...");
}

// Reconnect WiFi if disconnected
void ensureWiFiConnected() {
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    Serial.println("Reconnecting to WiFi...");
  }
}

// MQTT callback function (not used in this case)
void callback(char* topic, byte* payload, unsigned int length) {
  // Handle incoming MQTT messages if needed
}

// Reconnect MQTT if disconnected
void reconnectMQTT() {
  if (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again later");
    }
  }
}

void setup(void) {
  Serial.begin(115200);
  
  initWiFi();

  // Initialize MQTT client
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  // Initialize OLED and sensor
  u8g2.begin();
  Wire.begin(SDA, SCL);
  hdc1080.begin(0x40);
}

// Publish temperature and humidity to MQTT (raw values)
void publishToMQTT() {
  // Publish temperature and humidity (raw values, no units)
  client.publish(temp_topic, temp);
  client.publish(humidity_topic, humidity);
}

// Update OLED display with temperature and humidity
void updateDisplay() {
  // Display temperature on OLED
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_unifont_te);
  u8g2.drawUTF8(0, 14, "Temperatuur:");
  u8g2.setFont(u8g2_font_logisoso38_tf);
  char tempDisplay[10];
  snprintf(tempDisplay, sizeof(tempDisplay), "%s°C", temp);
  u8g2.drawUTF8(ALIGN_CENTER(tempDisplay), 60, tempDisplay);
  u8g2.sendBuffer();
  
  // Hold temperature display for 5 seconds
  delay(5000);
  
  // Display humidity on OLED
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_unifont_te);
  u8g2.drawUTF8(0, 16, "Õhuniiskus:");
  u8g2.setFont(u8g2_font_logisoso38_tf);
  char humidityDisplay[10];
  snprintf(humidityDisplay, sizeof(humidityDisplay), "%s%%", humidity);
  u8g2.drawUTF8(ALIGN_CENTER(humidityDisplay), 60, humidityDisplay);
  u8g2.sendBuffer();
  
  // Hold humidity display for 5 seconds
  delay(5000);
}

void loop(void) {
  // Read temperature and humidity
  tempValue = hdc1080.readTemperature();
  humidityValue = hdc1080.readHumidity();

  // Format to 1 decimal place (for MQTT)
  dtostrf(tempValue, 3, 1, temp);       // No "°C"
  dtostrf(humidityValue, 3, 1, humidity); // No "%"

  // Update OLED display
  updateDisplay();

  // Try WiFi reconnection (non-blocking)
  ensureWiFiConnected();

  // Try MQTT reconnection
  reconnectMQTT();

  // Publish to MQTT if connected
  if (client.connected()) {
    publishToMQTT();
  }
}