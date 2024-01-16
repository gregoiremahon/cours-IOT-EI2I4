/*
  Fichier : get_wifis_ESP32.ino
  Description : Scanne les réseaux Wi-Fi et envoie les données au serveur Node-RED via une requête HTTP POST.
  Auteurs : Grégoire MAHON, Armand LELONG
*/

#include <WiFi.h>
#include <HTTPClient.h>

// Identifiants du réseau WiFi
const char* ssid = "VOTRE SSID";
const char* password = "MOT DE PASSE";

// Adresse du serveur Node-RED
const char* serverName = "http://172.20.10.3:1880/wifi-data";

void setup() {
  Serial.begin(115200);          // Initialisation du port série
  WiFi.begin(ssid, password);    // Connexion au réseau WiFi

  // Attente de la connexion
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  int n = WiFi.scanNetworks();   // Scanne les réseaux Wi-Fi disponibles
  if (n == 0) {
    Serial.println("No networks found");
  } else {
    String jsonData = "[";       // Préparation du JSON pour envoyer les données (pourrait être fait avec une librairie JSON beaucoup plus facilement...)
    for (int i = 0; i < n; ++i) {
      if (i > 0) {
        jsonData += ",";
      }
      jsonData += "{\"ssid\":\"" + WiFi.SSID(i) + "\", \"rssi\":" + WiFi.RSSI(i) + ", \"bssid\":\"" + WiFi.BSSIDstr(i) + "\"}";
    }
    jsonData += "]";
    
    // Envoi des données au serveur Node-RED
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
  delay(10000); // Attente de 10 secondes avant le prochain scan
}
