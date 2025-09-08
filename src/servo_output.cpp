#include "servo_output.h"

ServoOutput::ServoOutput(int output_pin, String servo_name) {
    outputPin = output_pin;
    name = servo_name;
}

void ServoOutput::begin() {
    servo.attach(outputPin);
    center();  // 初期位置は中央
}

void ServoOutput::writeValue(float value) {
    // -100 から +100 を 45-135度にマップ
    int angle = map(constrain(value, -100, 100), -100, 100, SERVO_MIN, SERVO_MAX);
    servo.write(angle);
}

void ServoOutput::writeAngle(int angle) {
    angle = constrain(angle, SERVO_MIN, SERVO_MAX);
    servo.write(angle);
}

void ServoOutput::center() {
    servo.write(SERVO_CENTER);
}
