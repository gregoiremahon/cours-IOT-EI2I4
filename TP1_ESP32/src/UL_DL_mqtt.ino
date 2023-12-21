/**
 * UL_DL_mqtt.ino
 * Authors: Grégoire MAHON & Armand LELONG [EI2I 4 II - Groupe B]
 * 
 * This sketch establishes a connection to a WiFi network and an MQTT broker
 * to send temperature and humidity data read from a DHT11 sensor, and receive
 * commands to control an LED.
 * It publishes sensor data to specific MQTT topics and subscribes to a control
 * topic for the LED. On receiving a command, it adjusts the LED state accordingly.
 */

#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// Network credentials and MQTT server address
// Replace with actual WiFi credentials
const char* ssid = "SSID";
const char* password = "PASSWORD";
const char* mqtt_server = "172.20.10.3";

// MQTT topics for DHT11 sensor data and LED control
const char* temperature_topic = "dht11/temperature";
const char* humidity_topic = "dht11/humidity";
const char* led_control_topic = "led/control";

#define DHTPIN 5
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

WiFiClient espClient;
PubSubClient client(espClient);

// Callback function for incoming MQTT messages
void callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0';
  String message = String((char*)payload);
  
  // Control LED based on received MQTT message
  if (String(topic) == led_control_topic) {
    digitalWrite(LED_BUILTIN, message == "true" ? HIGH : LOW);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  WiFi.begin(ssid, password);
  
  // Attempt to connect to WiFi network
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Setup MQTT connection
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
  // Attempt to connect to MQTT broker and subscribe to LED control topic
  while (!client.connected()) {
    if (client.connect("ESP32Client")) {
      client.subscribe(led_control_topic);
      Serial.println("Connected to MQTT Broker");
    } else {
      Serial.println("Failed to connect to MQTT. Retry in 5 seconds");
      delay(5000);
    }
  }

  // Initialize DHT sensor
  dht.begin();
}

void loop() {
  // Handle MQTT loop
  client.loop();
  
  // Read and publish DHT11 sensor data
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Check if any reads failed and exit early
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Convert sensor readings to strings
  char tempStr[8];
  char humStr[8];
  dtostrf(temperature, 1, 2, tempStr);
  dtostrf(humidity, 1, 2, humStr);

  // Publish sensor data to MQTT topics
  client.publish(temperature_topic, tempStr);
  client.publish(humidity_topic, humStr);

  // Short delay before next loop iteration
  delay(10);
}
