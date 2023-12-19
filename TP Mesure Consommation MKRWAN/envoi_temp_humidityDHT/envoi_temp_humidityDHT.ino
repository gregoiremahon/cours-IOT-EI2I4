#include <MKRWAN.h>
#include <DHT.h>
#include <ArduinoLowPower.h>

LoRaModem modem;

String appEui = "1322144234234235";
String appKey = "A01DC8F9E363C86A883E41A6817427A5";

int DHT_PIN = 4;
int DHT_POWER = 5; // Broche pour alimenter le DHT avec GPIO
int DHT_TYPE = DHT11;
DHT dht(DHT_PIN, DHT_TYPE);

bool connected = false;
int err_count = 0;
short con = 0;

void setup() {
    initSerial();
    initSensors();
    initModem();
    pinMode(DHT_POWER, OUTPUT); // Configurez la broche d'alimentation comme sortie
    digitalWrite(DHT_POWER, LOW); // Éteignez le capteur pour commencer
}

void loop() {
    handleLoRaConnection();
    sendData();
    LowPower.sleep(20000); // Remplacer par la durée appropriée si nécessaire
}

void initSerial() {
    Serial.begin(115200);
    Serial.println("Carte MKRWAN n°11 connectée !\nTentative de connexion à l'application TTN...");
}

void initSensors() {
    dht.begin();
}

void initModem() {
    modem.begin(EU868);
    delay(1000);
}



void handleLoRaConnection() {
    if (!connected) {
        int ret = modem.joinOTAA(appEui, appKey);
        if (ret) {
            connected = true;
            modem.minPollInterval(60);
            modem.dataRate(5);
            delay(2000);
        }
    }
    if (connected) {
          LowPower.sleep(20000);
    }
}

int sendLoRaPacket() { // envoi paquet lora
    dht.begin();
    short dhtTemp = short(dht.readTemperature());
    short dhtHumidity = short(dht.readHumidity());
    modem.dataRate(1); // SF11 : (DataRate : 1 / Modulation : LoRa  / SF : 11 / BW : 125 /  bit/s : 440) 
    modem.beginPacket();
    modem.write(dhtTemp);
    modem.write(dhtHumidity);
    return modem.endPacket();
}


void sendData() {
    if (connected) {
        digitalWrite(DHT_POWER, HIGH); // Allumez le capteur
        delay(1000); // Donnez du temps au capteur pour se stabiliser
        int err = sendLoRaPacket();
        digitalWrite(DHT_POWER, LOW); // Éteignez le capteur après la mesure
    }
}
