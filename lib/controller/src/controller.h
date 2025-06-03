#pragma once
#include <Arduino.h>

class Controller {
public:
    // inicjalizacja – podajemy numer pinu LED
    void begin(uint8_t ledPin);
    // wywołaj, gdy przyjdzie komenda water z duration_ms
    void handleWaterCmd(uint32_t duration_ms);
    // wywołuj tę metodę w każdej iteracji loop() aby obsłużyć nieblokujące podlewanie
    void update();

private:
    uint8_t _ledPin;
    unsigned long _waterEndTime = 0; // czas zakończenia podlewania
};