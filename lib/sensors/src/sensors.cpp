#include <Arduino.h>
#include "sensors.h"

//void hello(){
    
//}

// simulate sensor data
float getMoisture() {
    return random(200, 450) / 10.0;
}

float getTemperature() {
    return random(200, 250) / 10.0;
}
