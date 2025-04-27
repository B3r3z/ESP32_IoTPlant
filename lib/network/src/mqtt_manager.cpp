#include <Arduino.h>
#include "mqtt_manager.h"

void MqttManager::begin(const char* ssid,
                        const char* password,
                        const char* broker,
                        uint16_t port) {
    _ssid     = ssid;
    _password = password;
    _broker   = broker;
    _port     = port;

    Serial.println("[MqttManager] begin");

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
    // np. automatycznie subskrybuj temat lub potwierdź połączenie
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
