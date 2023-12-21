#include <WiFi.h>

const char *ssid = "ESP32-Access-Point"; // Nom du point d'accès
const char *password = "123456789";      // Mot de passe du point d'accès

void setup() {
  Serial.begin(115200);
  WiFi.softAP(ssid, password);
  
  Serial.println();
  Serial.print("Point d'accès lancé. IP : ");
  Serial.println(WiFi.softAPIP());
}

void loop() {
}
