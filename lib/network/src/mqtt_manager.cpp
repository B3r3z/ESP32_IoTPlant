#include <Arduino.h>
#include <ArduinoJson.h>        // dodaj do platformio.ini / lib_deps: arduinojson
#include "mqtt_manager.h"
#include "mqtt_config.h"
#include "controller.h"

extern Controller controller;  // global z main.cpp

void MqttManager::begin(const char* ssid,
                        const char* password,
                        const char* broker,
                        uint16_t port) {
    _ssid     = ssid;
    _password = password;
    _broker   = broker;
    _port     = port;

    Serial.println("[MqttManager] begin");

    // ustawiamy callback odbioru wiadomości
    _mqttClient.onMessage(
      [this](char* topic, char* payload, AsyncMqttClientMessageProperties props,
             size_t len, size_t index, size_t total) {
        String t(topic);
        String p;
        for(size_t i=0;i<len;i++) p += (char)payload[i];
        onMqttMessage(t, p);
      }
    );

    connectToWifi();
}

void MqttManager::connectToWifi() {
    Serial.print("[MqttManager] Connecting to Wi-Fi ");
    Serial.println(_ssid);
    WiFi.begin(_ssid, _password);
}

// polling-based update to handle Wi-Fi and MQTT reconnects
void MqttManager::update() {
    if (WiFi.status() == WL_CONNECTED) {
        if (!_wifiConnected) {
            _wifiConnected = true;
            Serial.print("[MqttManager] Wi-Fi connected, IP: ");
            Serial.println(WiFi.localIP());
            connectToMqtt();
        }
    } else {
        if (_wifiConnected) {
            _wifiConnected = false;
            Serial.println("[MqttManager] Wi-Fi disconnected, reconnecting...");
            WiFi.reconnect();
        }
    }
    if (_wifiConnected && !_mqttClient.connected()) {
        Serial.println("[MqttManager] MQTT not connected, reconnecting...");
        connectToMqtt();
    }
}

void MqttManager::connectToMqtt() {
    Serial.print("[MqttManager] Connecting to MQTT broker ");
    Serial.print(_broker);
    Serial.print(":");
    Serial.println(_port);

    _mqttClient.setServer(_broker, _port);
    _mqttClient.onConnect(
      [this](bool sessionPresent){ onMqttConnect(sessionPresent); }
    );
    _mqttClient.onDisconnect(
      [this](AsyncMqttClientDisconnectReason reason){
          Serial.print("[MqttManager] MQTT disconnected, reason: ");
          Serial.println((int)reason);
          connectToMqtt();
      }
    );
    _mqttClient.connect();
}

void MqttManager::onMqttConnect(bool sessionPresent) {
    Serial.println("[MqttManager] MQTT connected");

    // subskrybujemy komendę podlewania
    Serial.print("[MqttManager] Subscribing to ");
    Serial.println(CMD_TOPIC);
    _mqttClient.subscribe(CMD_TOPIC, 1);
}

// handler wszystkich przychodzących wiadomości
void MqttManager::onMqttMessage(const String& topic, const String& payload) {
    Serial.print("[MqttManager] Message arrived [");
    Serial.print(topic);
    Serial.print("] -> ");
    Serial.println(payload);

    if (topic == String(CMD_TOPIC)) {
        // parsujemy JSON: {"duration_ms":5000}
        StaticJsonDocument<128> doc;
        if (deserializeJson(doc, payload) == DeserializationError::Ok) {
            uint32_t d = doc["duration_ms"] | 0;
            Serial.printf("[MqttManager] Water for %lu ms\n", d);
            controller.handleWaterCmd(d);
        } else {
            Serial.println("[MqttManager] JSON parse error");
        }
    }
}

void MqttManager::publish(const char* topic, const char* payload) {
    if (_mqttClient.connected()) {
        Serial.print("[MqttManager] Publishing to ");
        Serial.print(topic);
        Serial.print(": ");
        Serial.println(payload);
        _mqttClient.publish(topic, 0, false, payload);
    } else {
        Serial.println("[MqttManager] Publish failed: MQTT not connected");
    }
}
