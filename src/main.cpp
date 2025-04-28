#include <Arduino.h>
#include <mqtt_manager.h>
#include "sensors.h"
#include "mqtt_config.h"
#include "controller.h"

#define SSID     "test"
#define PASSWORD "test3213"
#define BROKER   "192.168.218.12"
#define PORT     1883

#define LED_BUILTIN 2

MqttManager mqtt;
Controller   controller;

void setup(){
    Serial.begin(115200);
    while(!Serial);

    // inicjujemy moduł sterowania diodą
    controller.begin(LED_BUILTIN);

    // podłączamy MQTT
    mqtt.begin(SSID, PASSWORD, BROKER, PORT);
}

void loop() {
    mqtt.update();
    // … telemetryka …
    delay(5000);
}