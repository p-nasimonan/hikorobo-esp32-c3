#include "led_output.h"

LedOutput::LedOutput(int pin) {
    ledPin = pin;
    currentState = false;
}

void LedOutput::begin() {
    pinMode(ledPin, OUTPUT);
    turnOff();  // 初期状態はオフ
}

void LedOutput::turnOn() {
    digitalWrite(ledPin, HIGH);
    currentState = true;
}

void LedOutput::turnOff() {
    digitalWrite(ledPin, LOW);
    currentState = false;
}

void LedOutput::setState(bool state) {
    if (state) {
        turnOn();
    } else {
        turnOff();
    }
}

bool LedOutput::getState() {
    return currentState;
}
