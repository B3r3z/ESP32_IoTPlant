#pragma once

// identyfikator roślinki
#define PLANT_ID   "3"

// telemetry (publikacja)
#define TOPIC_TELEM "plant/" PLANT_ID "/telemetry"

// cmd/water (subskrypcja)
#define CMD_TOPIC   "plant/" PLANT_ID "/cmd/water"

// status topic (publikacja)
#define STATUS_TOPIC "plant/" PLANT_ID "/status"

// Wersja firmware'u
#define FIRMWARE_VERSION "1.0.0"

// Identyfikator klienta MQTT
#define CLIENT_ID "ESP32_Plant_" PLANT_ID