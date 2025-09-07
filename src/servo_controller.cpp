#include "servo_controller.h"
#include "gyro_controller.h"

ServoController::ServoController(int input_pin, int output_pin, String servo_name, String axis_name) {
    inputPin = input_pin;
    outputPin = output_pin;
    name = servo_name;
    axis = axis_name;
    gyroController = nullptr;  // 初期化時はnullptr
}

void ServoController::begin() {
    pinMode(inputPin, INPUT);
    servo.attach(outputPin);
    centerServo();  // 中央位置に設定
    Serial.print(name);
    Serial.println(" 初期化完了");
}

void ServoController::update() {
    // サーボ制御信号読み取り
    unsigned long servoPulse = pulseIn(inputPin, HIGH, 25000);  // タイムアウト25ms
    
    if (servoPulse > 0) {
        int angle = processSignal(servoPulse);
        setServoAngle(angle);
    } else {
        // 信号なしの場合は中央位置
        centerServo();
    }
}

void ServoController::setGyroController(GyroController* gyro) {
    gyroController = gyro;
}

int ServoController::processSignal(unsigned long pulseWidth) {
    int angle;
    if (pulseWidth < RC_MIN) {
        angle = SERVO_MIN;
    } else if (pulseWidth > RC_MAX) {
        angle = SERVO_MAX;
    } else {
        // 線形変換: RC_MIN-RC_MAX → SERVO_MIN-SERVO_MAX
        angle = map(pulseWidth, RC_MIN, RC_MAX, SERVO_MIN, SERVO_MAX);
    }
    
    // ジャイロ補正を適用（有効な場合のみ）
    if (gyroController != nullptr && gyroController->isInitialized() && gyroController->isGyroEnabled()) {
        angle = gyroController->applyGyroCorrection(angle, axis);
    }
    
    return angle;
}

void ServoController::setServoAngle(int angle) {
    servo.write(angle);
}

void ServoController::centerServo() {
    servo.write(SERVO_CENTER);
}
