#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "gregsiphone";
const char* password = "gregwifi";

const char* serverName = "http://172.20.10.3:1880/wifi-data";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  // Scan for WiFi networks
  int n = WiFi.scanNetworks();
  if (n == 0) {
    Serial.println("No networks found");
  } else {
    // Création du JSON
    String jsonData = "[";
    for (int i = 0; i < n; ++i) {
      if (i > 0) {
        jsonData += ",";
      }
      jsonData += "{\"ssid\":\"" + WiFi.SSID(i) + "\", \"rssi\":" + WiFi.RSSI(i) + ", \"bssid\":\"" + WiFi.BSSIDstr(i) + "\"}";
    }
    jsonData += "]";
    // Envoi des données à Node-RED
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin(serverName);
      http.addHeader("Content-Type", "application/json");
      int httpResponseCode = http.POST(jsonData);

      if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println(httpResponseCode);
        Serial.println(response);
      }
      else {
        Serial.print("Error on sending POST: ");
        Serial.println(httpResponseCode);
      }
      http.end();
    }
  }
  delay(10000); // Attendre 10 secondes avant le prochain scan
}
