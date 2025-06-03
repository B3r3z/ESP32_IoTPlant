#pragma once

#include <WiFi.h>
#include <AsyncMqttClient.h>

class MqttManager {
public:
    // ssid, hasło do Wi‑Fi, adres i port brokera
    void begin(const char* ssid,
               const char* password,
               const char* broker,
               uint16_t port);

    // publikacja wiadomości
    void publish(const char* topic, const char* payload);

    // poll Wi-Fi connection status and trigger MQTT connect
    void update();

    // sprawdza czy mamy połączenie z MQTT
    bool isConnected() const { return _mqttClient.connected(); }

    // sprawdza siłę sygnału WiFi
    int getWifiRssi() const { return WiFi.RSSI(); }

private:
    void connectToWifi();
    void connectToMqtt();
    void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);

    // handles successful MQTT connection
    void onMqttConnect(bool sessionPresent);
    // handles incoming MQTT messages
    void onMqttMessage(const String& topic, const String& payload);

    const char* _ssid;
    const char* _password;
    const char* _broker;
    uint16_t    _port;

    AsyncMqttClient _mqttClient;
    // Wi-Fi connection state for polling
    bool _wifiConnected = false;
    // Czas ostatniej próby połączenia z MQTT
    unsigned long _lastMqttConnectAttempt = 0;
    // Czas pomiędzy próbami ponownego połączenia z MQTT (ms)
    unsigned long _mqttReconnectInterval = 5000;
};