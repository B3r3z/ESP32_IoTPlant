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


private:
    void connectToWifi();
    void connectToMqtt();

    // event‐handlery ESP32
    void onWifiEvent(WiFiEvent_t event);
    void onMqttConnect(bool sessionPresent);

    const char* _ssid;
    const char* _password;
    const char* _broker;
    uint16_t    _port;

    AsyncMqttClient _mqttClient;
    WiFiEventId_t  _wifiEventId;
};