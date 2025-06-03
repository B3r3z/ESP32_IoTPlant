#include <Arduino.h>
#include <ArduinoJson.h>        // dodaj do platformio.ini / lib_deps: arduinojson
#include <mqtt_manager.h>
#include <time.h>               // dla funkcji czasu
#include "sensors.h"
#include "mqtt_config.h"
#include "controller.h"

#define SSID     "test"
#define PASSWORD "test3213"
#define BROKER   "192.168.111.12"
#define PORT     1883

#define LED_BUILTIN 2

// Maksymalny czas (ms) bez połączenia, po którym nastąpi restart
#define MAX_DISCONNECTED_TIME 300000  // 5 minut

MqttManager mqtt;
Controller   controller;

// Watchdog dla połączenia
unsigned long lastConnectedTime = 0;
bool wasEverConnected = false;

void setup(){
    Serial.begin(115200);
    while(!Serial);

    Serial.println("\n\n=== LeafMeAlone system starting ===");
    Serial.print("Version: ");
    Serial.println(FIRMWARE_VERSION);
    Serial.print("Plant ID: ");
    Serial.println(PLANT_ID);

    // inicjujemy moduł sterowania diodą
    controller.begin(LED_BUILTIN);

    // podłączamy MQTT
    mqtt.begin(SSID, PASSWORD, BROKER, PORT);
    
    // Konfiguracja serwera NTP
    configTime(1 * 3600, 0, "pool.ntp.org", "time.nist.gov");
    Serial.println("[SYSTEM] Waiting for NTP time sync...");
    
    // Czekamy maksymalnie 10 sekund na synchronizację czasu
    time_t now = time(nullptr);
    int retries = 0;
    while (now < 24 * 3600 && retries < 20) {
        delay(500);
        Serial.print(".");
        now = time(nullptr);
        retries++;
    }
    Serial.println();
    
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
        Serial.print("[SYSTEM] NTP time synchronized: ");
        Serial.print(timeinfo.tm_year + 1900);
        Serial.print("-");
        Serial.print(timeinfo.tm_mon + 1);
        Serial.print("-");
        Serial.print(timeinfo.tm_mday);
        Serial.print(" ");
        Serial.print(timeinfo.tm_hour);
        Serial.print(":");
        Serial.print(timeinfo.tm_min);
        Serial.print(":");
        Serial.println(timeinfo.tm_sec);
    } else {
        Serial.println("[SYSTEM] Failed to get time from NTP server");
    }
}

void loop() {
    mqtt.update();
    controller.update();  // Wywołanie controller.update() dla obsługi nieblokującego podlewania
    
    // Sprawdzanie połączenia i obsługa watchdoga
    if (mqtt.isConnected()) {
        lastConnectedTime = millis();
        wasEverConnected = true;
    } else if (wasEverConnected && millis() - lastConnectedTime > MAX_DISCONNECTED_TIME) {
        Serial.println("[SYSTEM] Connection lost for too long, restarting...");
        delay(1000);  // daj czas na wyświetlenie komunikatu
        ESP.restart();
    }
    
    static unsigned long lastTelemetryTime = 0;
    unsigned long currentTime = millis();
    
    // Wysyłanie telemetrii co 5 sekund
    if (currentTime - lastTelemetryTime >= 5000) {
        lastTelemetryTime = currentTime;
        
        // Odczyt danych z czujników
        float moisture = getMoisture();
        float temperature = getTemperature();
        
        // Publikacja danych przez MQTT
        StaticJsonDocument<128> doc;
        doc["timestamp"] = time(nullptr);  // Rzeczywisty czas Unix (sekundy od 1970)
        doc["moisture"] = moisture;
        doc["temperature"] = temperature;
        doc["rssi"] = mqtt.getWifiRssi();  // Dodanie siły sygnału WiFi
        
        char buffer[128];
        serializeJson(doc, buffer);
        mqtt.publish(TOPIC_TELEM, buffer);
        
        // Debug output
        Serial.print("Telemetria - Wilgotność: ");
        Serial.print(moisture);
        Serial.print("%, Temperatura: ");
        Serial.print(temperature);
        Serial.print("°C, RSSI: ");
        Serial.print(mqtt.getWifiRssi());
        Serial.println(" dBm");
    }
    
    // Bez delay - zastąpione częstszymi, krótszymi sleep
    for (int i = 0; i < 10; i++) {
        delay(10);  // 10 ms x 10 = 100 ms, ale z częstszym sprawdzaniem stanu
        yield();    // Daj czas na obsługę WiFi/MQTT
    }
}