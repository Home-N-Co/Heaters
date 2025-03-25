//
// Created by nicol on 25/03/2025.
//

#include <WiFi.h>
#include <PubSubClient.h>

// Wi-Fi Credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// MQTT Broker Settings
const char* mqtt_server = "YOUR_MQTT_BROKER_IP";
constexpr int mqtt_port = 1883;
const char* mqtt_user = "YOUR_MQTT_USER";
const char* mqtt_password = "YOUR_MQTT_PASSWORD";

// MQTT Topics Pub
const char* consumption_topic = "heater/consumption";

// MQTT Topics Sub
const char* command_topic = "heater/command";
const char* temp_topic = "heater/temperature";


// Initialize WiFi & MQTT Client
WiFiClient espClient;
PubSubClient client(espClient);


void setup_wifi() {
    delay(10);
    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, password);
    while (WiFiClass::status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi Connected!");
}

void reconnect_mqtt() {
    while (!client.connected()) {
        Serial.println("Connecting to MQTT...");
        if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
            Serial.println("Connected to MQTT Broker!");
            client.subscribe(command_topic);
        } else {
            Serial.print("MQTT failed, retrying in 5s...");
            delay(5000);
        }
    }
}

void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message received [");
    Serial.print(topic);
    Serial.print("] ");

    String message = "";
    for (int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    Serial.println(message);

    if (String(topic) == command_topic) {
        if (message == "ON") {
            Serial.println("Turning heater ON");
        } else if (message == "OFF") {
            Serial.println("Turning heater OFF");
        }
    }
}

void setup() {
    Serial.begin(115200);
    setup_wifi();

    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);
}

void loop() {
    if (!client.connected()) {
        reconnect_mqtt();
    }
    client.loop();

    // Sub Temperature sensor readings
    temperature += (random(-10, 10) / 100.0);

    powerConsumption += (random(-5, 5) / 100.0);

    // Publish temperature & consumption data
    char tempStr[8], powerStr[8];
    dtostrf(temperature, 6, 2, tempStr);
    dtostrf(powerConsumption, 6, 2, powerStr);

    client.publish(temp_topic, tempStr);
    client.publish(consumption_topic, powerStr);

    delay(5000); // Publish every 5 seconds
}
