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

// MQTT Topics
const char* command_topic = "heater/command";
const char* status_topic = "heater/status";
const char* conso_topic = "heater/conso";

WiFiClient espClient;
PubSubClient client(espClient);

bool heaterStatus = false;  // Heater status (OFF by default)
float consoElec = 0.0;  // Off by default

void setup_wifi() {
    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
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
            client.subscribe(command_topic);  // Subscribe to heater command
            client.subscribe(conso_topic); // for exemple as we don't have captor yet
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
            heaterStatus = true;
        } else if (message == "OFF") {
            Serial.println("Turning heater OFF");
            heaterStatus = false;
        }
        client.publish(status_topic, heaterStatus ? "ON" : "OFF");
    }
    if (String(topic) == conso_topic) {
        if (message == consoElec) {}
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

    // Publish Heater Status every 5 seconds
    client.publish(status_topic, heaterStatus ? "ON" : "OFF");
    client.publish(conso_topic, );

    delay(5000);
}
