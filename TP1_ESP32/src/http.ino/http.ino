#include "DHT.h"
#include <ArduinoJson.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define DHTPIN 5
#define DHTTYPE DHT11

const char* ssid = "gregsiphone";
const char* password = "gregwifi";

DHT dht(DHTPIN, DHTTYPE);

void connectToWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

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
    http.begin("http://172.20.10.3:1880/dht11");
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(jsonBuffer);
    http.end();
  }
}

void getLedState() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("http://172.20.10.3:1880/ledState");
    int httpResponseCode = http.GET();

    if (httpResponseCode == 200) {
      String response = http.getString();
      Serial.print("Response: ");
      Serial.println(response);
      digitalWrite(LED_BUILTIN, response == "true" ? HIGH : LOW);
    }
    Serial.print("Demande de l'état de la LED : ");
    Serial.println(http.getString());
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
