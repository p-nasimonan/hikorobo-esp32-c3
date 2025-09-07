#include "led_controller.h"

LedController::LedController(int input_pin, int output_pin) {
    inputPin = input_pin;
    outputPin = output_pin;
}

void LedController::begin() {
    pinMode(inputPin, INPUT);
    pinMode(outputPin, OUTPUT);
    digitalWrite(outputPin, LOW);  // 初期状態はOFF
    Serial.println("LED Controller 初期化完了");
}

void LedController::update() {
    // LED制御信号読み取り
    unsigned long ledPulse = pulseIn(inputPin, HIGH, 25000);  // タイムアウト25ms
    
    if (ledPulse > 0) {
        bool ledState = processSignal(ledPulse);
        setLedState(ledState);
    } else {
        // 信号なしの場合はLED OFF
        setLedState(false);
    }
}

bool LedController::processSignal(unsigned long pulseWidth) {
    // PWM信号の中央値（1500μs）を基準にON/OFF判定
    // 1500μs以上でON、未満でOFF
    return (pulseWidth >= RC_CENTER);
}

void LedController::setLedState(bool state) {
    digitalWrite(outputPin, state ? HIGH : LOW);
}
