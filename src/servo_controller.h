#ifndef SERVO_CONTROLLER_H
#define SERVO_CONTROLLER_H

#include <Arduino.h>
#include <ESP32Servo.h>

class ServoController {
private:
    int inputPin;
    int outputPin;
    Servo servo;
    String name;
    
    static const int RC_MIN = 1000;      // ラジコン信号の最小値（マイクロ秒）
    static const int RC_MAX = 2000;      // ラジコン信号の最大値（マイクロ秒）
    static const int SERVO_MIN = 45;     // サーボ最小角度
    static const int SERVO_MAX = 135;    // サーボ最大角度
    static const int SERVO_CENTER = 90;  // サーボ中央角度
    
public:
    ServoController(int input_pin, int output_pin, String servo_name);
    void begin();
    void update();
    int processSignal(unsigned long pulseWidth);
    void setServoAngle(int angle);
    void centerServo();
};

#endif
