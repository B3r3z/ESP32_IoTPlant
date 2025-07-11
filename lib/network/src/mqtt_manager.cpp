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

    // Ustawienia MQTT
    _mqttClient.setKeepAlive(60);  // keepalive 60 sekund
    _mqttClient.setClientId(CLIENT_ID);
    
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
    static IPAddress lastIP;
    static unsigned long lastStatusPrint = 0;
    unsigned long currentMillis = millis();
    
    // Wydrukuj status co 30 sekund dla diagnostyki
    if (currentMillis - lastStatusPrint > 30000) {
        lastStatusPrint = currentMillis;
        Serial.print("[MqttManager] Status - WiFi: ");
        Serial.print(WiFi.status());
        Serial.print(", RSSI: ");
        Serial.print(WiFi.RSSI());
        Serial.print(" dBm, IP: ");
        Serial.print(WiFi.localIP());
        Serial.print(", MQTT connected: ");
        Serial.println(_mqttClient.connected());
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        if (!_wifiConnected) {
            _wifiConnected = true;
            Serial.print("[MqttManager] Wi-Fi connected, IP: ");
            Serial.println(WiFi.localIP());
            lastIP = WiFi.localIP();
            connectToMqtt();
        } else if (WiFi.localIP() != lastIP) {
            // IP się zmieniło, ponowne połączenie z MQTT
            lastIP = WiFi.localIP();
            Serial.print("[MqttManager] IP changed to: ");
            Serial.println(lastIP);
            _mqttClient.disconnect();
            connectToMqtt();
        }
    } else {
        if (_wifiConnected) {
            _wifiConnected = false;
            Serial.println("[MqttManager] Wi-Fi disconnected, reconnecting...");
            WiFi.reconnect();
        }
    }
    
    // Obsługa ponownego połączenia MQTT z opóźnieniem
    if (_wifiConnected && !_mqttClient.connected() && 
        (currentMillis - _lastMqttConnectAttempt > _mqttReconnectInterval)) {
        Serial.println("[MqttManager] MQTT not connected, reconnecting...");
        _lastMqttConnectAttempt = currentMillis;
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
          onMqttDisconnect(reason);
      }
    );
    _mqttClient.connect();
}

void MqttManager::onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
    Serial.print("[MqttManager] MQTT disconnected, reason: ");
    Serial.println((int)reason);
    // Nie wywołujemy connectToMqtt() tutaj - będzie obsłużone przez update()
    // z opóźnieniem, aby uniknąć szybkich wielokrotnych prób połączenia
}

void MqttManager::onMqttConnect(bool sessionPresent) {
    Serial.println("[MqttManager] MQTT connected");

    // subskrybujemy komendę podlewania
    Serial.print("[MqttManager] Subscribing to ");
    Serial.println(CMD_TOPIC);
    _mqttClient.subscribe(CMD_TOPIC, 1);
    
    // Wysyłamy status po połączeniu
    char buffer[128];
    snprintf(buffer, sizeof(buffer), 
             "{\"id\":\"%s\",\"version\":\"%s\",\"uptime\":%lu,\"rssi\":%d}", 
             PLANT_ID, FIRMWARE_VERSION, millis() / 1000, WiFi.RSSI());
    _mqttClient.publish(STATUS_TOPIC, 0, true, buffer);
    Serial.print("[MqttManager] Published status: ");
    Serial.println(buffer);
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
        _mqttClient.publish(topic, 1, false, payload);  // QoS 1 dla większej niezawodności
    } else {
        Serial.println("[MqttManager] Publish failed: MQTT not connected");
    }
}
