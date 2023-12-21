/**
 * UL_DL_http.ino
 * Authors: Grégoire MAHON & Armand LELONG [EI2I 4 II - Groupe B]
 *
 * This code connects an ESP32 to a WiFi network and sends temperature and humidity data
 * from a DHT11 sensor to a Node-RED server using HTTP POST. It also checks the state of an LED
 * via an HTTP GET request and controls the LED based on the server's response.
 */

#include "DHT.h"
#include <ArduinoJson.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define DHTPIN 5
#define DHTTYPE DHT11
// Replace with actual WiFi credentials
const char* ssid = "SSID";
const char* password = "PASSWORD";

DHT dht(DHTPIN, DHTTYPE);

// Connects to the WiFi network
void connectToWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

// Reads data from DHT sensor and sends it to the server
void readSensorAndSendData() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  if (isnan(humidity) || isnan(temperature)) {
    return;
  }
  if (WiFi.status() == WL_CONNECTED) {
    StaticJsonDocument<200> jsonDoc;
    jsonDoc["temperature"] = temperature;
    jsonDoc["humidity"] = humidity;
    char jsonBuffer[512];
    serializeJson(jsonDoc, jsonBuffer);
    HTTPClient http;
    http.begin("http://172.20.10.3:1880/dht11");  // Server URL
    http.addHeader("Content-Type", "application/json");
    http.POST(jsonBuffer);
    http.end();
  }
}

// Checks the LED state from the server and controls the LED
void getLedState() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("http://172.20.10.3:1880/ledState");  // URL for LED state
    if (http.GET() == 200) {
      String response = http.getString();
      digitalWrite(LED_BUILTIN, response == "true" ? HIGH : LOW);
    }
    http.end();
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  dht.begin();
  connectToWiFi();
}

void loop() {
  readSensorAndSendData();
  getLedState();
  delay(10);
}
