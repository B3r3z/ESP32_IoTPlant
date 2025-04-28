#pragma once
#include <Arduino.h>

class Controller {
public:
    // inicjalizacja – podajemy numer pinu LED
    void begin(uint8_t ledPin);
    // wywołaj, gdy przyjdzie komenda water z duration_ms
    void handleWaterCmd(uint32_t duration_ms);

private:
    uint8_t _ledPin;
};