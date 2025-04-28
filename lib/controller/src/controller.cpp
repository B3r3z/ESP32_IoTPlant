#include "controller.h"

void Controller::begin(uint8_t ledPin) {
    _ledPin = ledPin;
    pinMode(_ledPin, OUTPUT);
    digitalWrite(_ledPin, LOW);
}

void Controller::handleWaterCmd(uint32_t duration_ms) {
    // zapal diodę
    digitalWrite(_ledPin, HIGH);
    delay(duration_ms);
    digitalWrite(_ledPin, LOW);
}