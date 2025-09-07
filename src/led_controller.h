#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include <Arduino.h>

class LedController {
private:
    int inputPin;
    int outputPin;
    static const int RC_CENTER = 1500;  // ラジコン信号の中央値（マイクロ秒）
    
public:
    LedController(int input_pin, int output_pin);
    void begin();
    void update();
    bool processSignal(unsigned long pulseWidth);
    void setLedState(bool state);
};

#endif
