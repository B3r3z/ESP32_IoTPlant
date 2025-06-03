#include "controller.h"

void Controller::begin(uint8_t ledPin) {
    _ledPin = ledPin;
    pinMode(_ledPin, OUTPUT);
    digitalWrite(_ledPin, LOW);
}

void Controller::handleWaterCmd(uint32_t duration_ms) {
    // zapal diodę i zaplanuj jej wyłączenie
    digitalWrite(_ledPin, HIGH);
    _waterEndTime = millis() + duration_ms;
    Serial.print("[Controller] Watering started, LED ON, duration: ");
    Serial.print(duration_ms);
    Serial.println(" ms");
}

void Controller::update() {
    // Sprawdź czy trzeba wyłączyć diodę po upływie czasu podlewania
    if (_waterEndTime > 0 && millis() >= _waterEndTime) {
        digitalWrite(_ledPin, LOW);
        _waterEndTime = 0;
        Serial.println("[Controller] Watering finished, LED OFF");
    }
}