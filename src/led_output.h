#ifndef LED_OUTPUT_H
#define LED_OUTPUT_H

#include <Arduino.h>

class LedOutput {
private:
    int ledPin;
    bool currentState;
    
public:
    LedOutput(int pin);
    void begin();
    void turnOn();
    void turnOff();
    void setState(bool state);
    bool getState();
};

#endif
