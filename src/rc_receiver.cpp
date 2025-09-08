#include "rc_receiver.h"

// 静的メンバーの初期化
RCReceiver* RCReceiver::instance = nullptr;

RCReceiver::RCReceiver(int elevator_pin, int rudder_pin, int led_pin) {
    elevatorPin = elevator_pin;
    rudderPin = rudder_pin;
    ledPin = led_pin;
    instance = this;
}

void RCReceiver::begin() {
    pinMode(elevatorPin, INPUT);
    pinMode(rudderPin, INPUT);
    pinMode(ledPin, INPUT);
    
    // 割り込み設定（立ち上がりと立ち下がりの両方で検出）
    attachInterrupt(digitalPinToInterrupt(elevatorPin), elevatorISR, CHANGE);
    attachInterrupt(digitalPinToInterrupt(rudderPin), rudderISR, CHANGE);
    attachInterrupt(digitalPinToInterrupt(ledPin), ledISR, CHANGE);
}

void IRAM_ATTR RCReceiver::elevatorISR() {
    if (instance == nullptr) return;
    
    if (digitalRead(instance->elevatorPin) == HIGH) {
        // 立ち上がり: パルス開始
        instance->elevatorPulseStart = micros();
    } else {
        // 立ち下がり: パルス終了
        unsigned long pulseEnd = micros();
        if (instance->elevatorPulseStart > 0) {
            instance->elevatorPulseWidth = pulseEnd - instance->elevatorPulseStart;
        }
    }
}

void IRAM_ATTR RCReceiver::rudderISR() {
    if (instance == nullptr) return;
    
    if (digitalRead(instance->rudderPin) == HIGH) {
        // 立ち上がり: パルス開始
        instance->rudderPulseStart = micros();
    } else {
        // 立ち下がり: パルス終了
        unsigned long pulseEnd = micros();
        if (instance->rudderPulseStart > 0) {
            instance->rudderPulseWidth = pulseEnd - instance->rudderPulseStart;
        }
    }
}

void IRAM_ATTR RCReceiver::ledISR() {
    if (instance == nullptr) return;
    
    if (digitalRead(instance->ledPin) == HIGH) {
        // 立ち上がり: パルス開始
        instance->ledPulseStart = micros();
    } else {
        // 立ち下がり: パルス終了
        unsigned long pulseEnd = micros();
        if (instance->ledPulseStart > 0) {
            instance->ledPulseWidth = pulseEnd - instance->ledPulseStart;
        }
    }
}

unsigned long RCReceiver::getElevatorPulseWidth() {
    return elevatorPulseWidth;
}

unsigned long RCReceiver::getRudderPulseWidth() {
    return rudderPulseWidth;
}

unsigned long RCReceiver::getLedPulseWidth() {
    return ledPulseWidth;
}

float RCReceiver::getElevatorValue() {
    // 1000-2000μs を -100 から +100 にマップ
    return map(elevatorPulseWidth, 1000, 2000, -100, 100);
}

float RCReceiver::getRudderValue() {
    // 1000-2000μs を -100 から +100 にマップ
    return map(rudderPulseWidth, 1000, 2000, -100, 100);
}

float RCReceiver::getLedValue() {
    // 1000-2000μs を -100 から +100 にマップ
    return map(ledPulseWidth, 1000, 2000, -100, 100);
}

bool RCReceiver::isElevatorValid() {
    // 800-2200μsの範囲内であれば有効
    return (elevatorPulseWidth >= 800 && elevatorPulseWidth <= 2200);
}

bool RCReceiver::isRudderValid() {
    // 800-2200μsの範囲内であれば有効
    return (rudderPulseWidth >= 800 && rudderPulseWidth <= 2200);
}

bool RCReceiver::isLedValid() {
    // 800-2200μsの範囲内であれば有効
    return (ledPulseWidth >= 800 && ledPulseWidth <= 2200);
}

bool RCReceiver::isPassthroughMode() {
    // LED信号がオフ（1000μs付近）の場合はパススルーモード
    // LED信号がオン（2000μs付近）の場合は姿勢制御モード
    if (!isLedValid()) {
        return true;  // 信号がない場合はパススルーモード
    }
    return ledPulseWidth < 1500;  // 1500μs未満をパススルーモードとする
}
