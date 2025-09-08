#ifndef SERVO_OUTPUT_H
#define SERVO_OUTPUT_H

#include <Arduino.h>
#include <ESP32Servo.h>

class ServoOutput {
private:
    int outputPin;
    Servo servo;
    String name;
    
    static const int SERVO_MIN = 45;     // サーボ最小角度
    static const int SERVO_MAX = 135;    // サーボ最大角度
    static const int SERVO_CENTER = 90;  // サーボ中央角度
    
public:
    ServoOutput(int output_pin, String servo_name);
    void begin();
    
    // -100 から +100 の値をサーボ角度に変換して出力
    void writeValue(float value);
    
    // 角度を直接指定
    void writeAngle(int angle);
    
    // センター位置に設定
    void center();
};

#endif
