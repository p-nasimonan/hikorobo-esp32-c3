#include "servo_controller.h"

ServoController::ServoController(int input_pin, int output_pin, String servo_name) {
    inputPin = input_pin;
    outputPin = output_pin;
    name = servo_name;
}

void ServoController::begin() {
    pinMode(inputPin, INPUT);
    servo.attach(outputPin);
    centerServo();  // 中央位置に設定
    Serial.print(name);
    Serial.println(" Controller 初期化完了 - 中央位置に設定");
}

void ServoController::update() {
    // サーボ制御信号読み取り
    unsigned long servoPulse = pulseIn(inputPin, HIGH, 25000);  // タイムアウト25ms
    
    if (servoPulse > 0) {
        int angle = processSignal(servoPulse);
        setServoAngle(angle);
        Serial.print(name);
        Serial.print(" - 信号: ");
        Serial.print(servoPulse);
        Serial.print(" μs, 角度: ");
        Serial.println(angle);
    } else {
        // 信号なしの場合は中央位置
        centerServo();
        Serial.print(name);
        Serial.println(" 信号なし - 中央位置");
    }
}

int ServoController::processSignal(unsigned long pulseWidth) {
    // 将来のジャイロ補正処理をここに追加予定
    // 現在はパススルー処理
    
    int angle;
    if (pulseWidth < RC_MIN) {
        angle = SERVO_MIN;
    } else if (pulseWidth > RC_MAX) {
        angle = SERVO_MAX;
    } else {
        // 線形変換: RC_MIN-RC_MAX → SERVO_MIN-SERVO_MAX
        angle = map(pulseWidth, RC_MIN, RC_MAX, SERVO_MIN, SERVO_MAX);
    }
    
    return angle;
}

void ServoController::setServoAngle(int angle) {
    servo.write(angle);
}

void ServoController::centerServo() {
    servo.write(SERVO_CENTER);
}
