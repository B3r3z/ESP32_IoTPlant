#include <Arduino.h>
#include <mqtt_manager.h>
#include "sensors.h"


#define SSID "test"
#define PASSWORD "test3213"
#define BROKER "192.168.218.12"
#define PORT 1883


#define PLANT_ID "2"
//#define TOPIC "plant/" PLANT_ID "/telemetry"
#define TOPIC "plant/2/telemetry"

MqttManager mqtt;

void setup(){
    Serial.begin(115200);
    while(!Serial);
    mqtt.begin(SSID, PASSWORD, BROKER, PORT);
    Serial.println(WiFi.localIP());
    // initial setup complete
}

// simulate and publish sensor telemetry periodically
void loop() {
    mqtt.update();
    float moisture    = getMoisture();
    float temperature = getTemperature();
    long  ts          = millis() / 1000;

    String payload = String("{\"timestamp\":") + ts +
                     ",\"moisture\":" + moisture +
                     ",\"temperature\":" + temperature + "}";

    mqtt.publish(TOPIC, payload.c_str());
    delay(5000);
}