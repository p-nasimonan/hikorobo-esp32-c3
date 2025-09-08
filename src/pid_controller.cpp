#include "pid_controller.h"
#include <Arduino.h>

PIDController::PIDController(float kp, float ki, float kd) 
    : kp(kp), ki(ki), kd(kd), previousError(0), integral(0), 
      lastTime(0), outputMin(-1000), outputMax(1000) {
}

float PIDController::calculate(float setpoint, float input) {
    unsigned long currentTime = millis();
    float deltaTime = (currentTime - lastTime) / 1000.0; // 秒に変換
    
    if (lastTime == 0) {
        deltaTime = 0.01; // 初回は10msと仮定
    }
    
    // 誤差計算
    float error = setpoint - input;
    
    // 積分項（積分ワインドアップ対策付き）
    integral += error * deltaTime;
    if (integral > outputMax / ki) integral = outputMax / ki;
    if (integral < outputMin / ki) integral = outputMin / ki;
    
    // 微分項
    float derivative = 0;
    if (deltaTime > 0) {
        derivative = (error - previousError) / deltaTime;
    }
    
    // PID出力計算
    float output = kp * error + ki * integral + kd * derivative;
    
    // 出力制限
    if (output > outputMax) output = outputMax;
    if (output < outputMin) output = outputMin;
    
    // 次回用に保存
    previousError = error;
    lastTime = currentTime;
    
    return output;
}

void PIDController::setGains(float new_kp, float new_ki, float new_kd) {
    kp = new_kp;
    ki = new_ki;
    kd = new_kd;
}

void PIDController::setOutputLimits(float min, float max) {
    outputMin = min;
    outputMax = max;
}

void PIDController::reset() {
    previousError = 0;
    integral = 0;
    lastTime = 0;
}
