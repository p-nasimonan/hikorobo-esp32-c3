#include "auto_control.h"
#include <Arduino.h>

AutoControl::AutoControl() 
    : pitchPID(2.0, 0.1, 0.5),    // PIDパラメータ（調整が必要）
      rollPID(2.0, 0.1, 0.5),
      yawPID(2.0, 0.1, 0.05),
      targetPitch(0), targetRoll(0), targetYaw(0),
      currentPitch(0), currentRoll(0), currentYaw(0),
      lastUpdateTime(0),
      enablePitchControl(true), enableRollControl(false), enableYawControl(true),
      pitchFilter(0), rollFilter(0) {
}

void AutoControl::begin() {
    // PID出力制限設定（サーボの可動範囲に合わせる）
    pitchPID.setOutputLimits(-90, 90);  // エレベーター出力制限
    rollPID.setOutputLimits(-90, 90);   // エルロン出力制限
    yawPID.setOutputLimits(-90, 90);    // ラダー出力制限
    
    lastUpdateTime = millis();
    
    Serial.println("AutoControl initialized");
}

void AutoControl::setTargets(float pitch, float roll, float yaw) {
    targetPitch = pitch;
    targetRoll = roll;
    targetYaw = yaw;
}

void AutoControl::update(MPU6050& mpu) {
    unsigned long currentTime = millis();
    float deltaTime = (currentTime - lastUpdateTime) / 1000.0;
    
    if (deltaTime < 0.001) return; // 更新頻度制限
    
    // ジャイロデータ取得
    float gyroX = mpu.getGyroX();  // ロール軸
    float gyroY = mpu.getGyroY();  // ピッチ軸
    float gyroZ = mpu.getGyroZ();  // ヨー軸
    
    // 加速度データ取得（水平基準用）
    float accX = mpu.getAccX();
    float accY = mpu.getAccY();
    float accZ = mpu.getAccZ();
    
    // 加速度から水平基準角度計算
    float accPitch = atan2(-accX, sqrt(accY * accY + accZ * accZ)) * 180.0 / PI;
    float accRoll = atan2(accY, accZ) * 180.0 / PI;
    
    // 相補フィルター（ジャイロ + 加速度）
    float alpha = 0.96; // ジャイロの重み
    
    if (lastUpdateTime == 0) {
        // 初回は加速度ベース
        currentPitch = accPitch;
        currentRoll = accRoll;
        currentYaw = 0;
    } else {
        // 相補フィルター適用
        currentPitch = alpha * (currentPitch + gyroY * deltaTime) + (1 - alpha) * accPitch;
        currentRoll = alpha * (currentRoll + gyroX * deltaTime) + (1 - alpha) * accRoll;
        currentYaw += gyroZ * deltaTime; // ヨーは積分のみ
    }
    
    // ローパスフィルター
    pitchFilter = pitchFilter * 0.8 + currentPitch * 0.2;
    rollFilter = rollFilter * 0.8 + currentRoll * 0.2;
    
    lastUpdateTime = currentTime;
}

float AutoControl::getElevatorOutput() {
    if (!enablePitchControl) return 0;
    return pitchPID.calculate(targetPitch, pitchFilter);
}

float AutoControl::getRudderOutput() {
    if (!enableYawControl) return 0;
    return yawPID.calculate(targetYaw, currentYaw);
}

float AutoControl::getAileronOutput() {
    if (!enableRollControl) return 0;
    return rollPID.calculate(targetRoll, rollFilter);
}

void AutoControl::setPitchPID(float kp, float ki, float kd) {
    pitchPID.setGains(kp, ki, kd);
}

void AutoControl::setRollPID(float kp, float ki, float kd) {
    rollPID.setGains(kp, ki, kd);
}

void AutoControl::setYawPID(float kp, float ki, float kd) {
    yawPID.setGains(kp, ki, kd);
}

void AutoControl::enableControl(bool pitch, bool roll, bool yaw) {
    enablePitchControl = pitch;
    enableRollControl = roll;
    enableYawControl = yaw;
}

void AutoControl::printDebugInfo() {
    Serial.print("Pitch: ");
    Serial.print(currentPitch, 2);
    Serial.print(" -> ");
    Serial.print(getElevatorOutput(), 2);
    Serial.print(", Roll: ");
    Serial.print(currentRoll, 2);
    Serial.print(", Yaw: ");
    Serial.print(currentYaw, 2);
    Serial.print(" -> ");
    Serial.println(getRudderOutput(), 2);
}

void AutoControl::reset() {
    pitchPID.reset();
    rollPID.reset();
    yawPID.reset();
    currentPitch = 0;
    currentRoll = 0;
    currentYaw = 0;
    pitchFilter = 0;
    rollFilter = 0;
    lastUpdateTime = 0;
}
