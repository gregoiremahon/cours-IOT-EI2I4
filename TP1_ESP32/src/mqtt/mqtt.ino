#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

const char* ssid = "gregsiphone";
const char* password = "gregwifi";

const char* mqtt_server = "172.20.10.3";
const char* temperature_topic = "dht11/temperature";
const char* humidity_topic = "dht11/humidity";
const char* led_control_topic = "led/control";  // Add topic for LED control

#define DHTPIN 5
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0';
  String message = String((char*)payload);
  
  if (String(topic) == led_control_topic) {
    if (message == "true") {
      digitalWrite(LED_BUILTIN, HIGH);
    } else {
      digitalWrite(LED_BUILTIN, LOW);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
  while (!client.connected()) {
    if (client.connect("ESP32Client")) {
      client.subscribe(led_control_topic);
      Serial.println("Connected to MQTT Broker");
    } else {
      Serial.println("Failed to connect to MQTT. Retry in 5 seconds");
      delay(5000);
    }
  }

  dht.begin();
}

void loop() {
  client.loop();

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  char tempStr[8];
  char humStr[8];
  dtostrf(temperature, 1, 2, tempStr);
  dtostrf(humidity, 1, 2, humStr);

  client.publish(temperature_topic, tempStr);
  client.publish(humidity_topic, humStr);

  delay(10);
}
