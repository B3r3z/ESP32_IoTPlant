#include <Arduino.h>
#include <mqtt_manager.h>
#include "sensors.h"


#define SSID "your_ssid"
#define PASSWORD "your_password"
#define BROKER "192.168.100.1"
#define PORT 1883


#define PLANT_ID "2"
#define TOPIC "plant/" PLANT_ID "/telemetry"


MqttManager mqtt;

void setup(){
    mqtt.begin(SSID, PASSWORD, BROKER, PORT);
    // initial setup complete
}

// simulate and publish sensor telemetry periodically
void loop() {
    float moisture    = getMoisture();
    float temperature = getTemperature();
    long  ts          = millis() / 1000;

    String payload = String("{\"timestamp\":") + ts +
                     ",\"moisture\":" + moisture +
                     ",\"temperature\":" + temperature + "}";

    mqtt.publish(TOPIC, payload.c_str());
    delay(5000);
}