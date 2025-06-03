#include <Arduino.h>
#include "sensors.h"

// simulate sensor data
float getMoisture() {
    return random(300, 450) / 10.0;
}

float getTemperature() {
    return random(200, 250) / 10.0;
}
