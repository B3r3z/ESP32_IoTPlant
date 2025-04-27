#include "mqtt_manager.h"

void MqttManager::begin(const char* ssid,
                        const char* password,
                        const char* broker,
                        uint16_t port) {
    _ssid     = ssid;
    _password = password;
    _broker   = broker;
    _port     = port;

    // zarejestruj handler Wi‑Fi
    _wifiEventId = WiFi.onEvent(
      [this](WiFiEvent_t event){ onWifiEvent(event); }
    );

    connectToWifi();
}

void MqttManager::connectToWifi() {
    WiFi.begin(_ssid, _password);
}

void MqttManager::onWifiEvent(WiFiEvent_t event) {
    if (event == SYSTEM_EVENT_STA_GOT_IP) {
        // po uzyskaniu IP – łączymy MQTT
        connectToMqtt();
    } else if (event == SYSTEM_EVENT_STA_DISCONNECTED) {
        // w razie rozłączenia – próbujemy ponownie Wi‑Fi
        WiFi.reconnect();
    }
}

void MqttManager::connectToMqtt() {
    _mqttClient.setServer(_broker, _port);
    _mqttClient.onConnect(
      [this](bool sessionPresent){ onMqttConnect(sessionPresent); }
    );
    _mqttClient.onDisconnect(
      [](AsyncMqttClientDisconnectReason reason){
          // tutaj ew. logika po disconnect
      }
    );
    _mqttClient.connect();
}

void MqttManager::onMqttConnect(bool sessionPresent) {
    // np. automatycznie subskrybuj temat lub potwierdź połączenie
    // Serial.println("MQTT connected!");
}

void MqttManager::publish(const char* topic, const char* payload) {
    if (_mqttClient.connected()) {
        _mqttClient.publish(topic, 0, false, payload);
    }
}

void MqttManager::loop() {
    // AsyncMqttClient nie wymaga loop(), ale jeśli używasz innej biblioteki:
    _mqttClient.loop();
}