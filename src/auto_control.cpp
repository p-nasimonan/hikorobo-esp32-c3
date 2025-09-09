#include "auto_control.h"
#include <Arduino.h>

AutoControl::AutoControl() 
    : pitchPID(1.0, 0.1, 0.05),    // PIDパラメータ（調整が必要）
      rollPID(2.0, 0.1, 0.05),
      yawPID(1.0, 0.1, 0.05),
      targetPitch(0), targetRoll(0), targetYaw(0),
#ifdef USE_ANGLE_CONTROL
      currentPitch(0), currentRoll(0), currentYaw(0),
#endif
#ifdef USE_ACCEL_CONTROL
      targetAccelX(0), targetAccelY(0), targetAccelZ(-1.0),  // Z軸は重力分
      currentAccelX(0), currentAccelY(0), currentAccelZ(-1.0),
#endif
      lastUpdateTime(0),
      enablePitchControl(true), enableRollControl(false), enableYawControl(true),
      pitchFilter(0), rollFilter(0) {
}

void AutoControl::begin() {
#ifdef USE_ANGLE_CONTROL
    // 角度制御用PID設定
    pitchPID.setOutputLimits(-90, 90);  // エレベーター出力制限
    rollPID.setOutputLimits(-90, 90);   // エルロン出力制限
    yawPID.setOutputLimits(-90, 90);    // ラダー出力制限
    Serial.println("AutoControl initialized - ANGLE CONTROL MODE");
#endif

#ifdef USE_ACCEL_CONTROL
    // 加速度制御用PID設定（より小さな出力制限）
    pitchPID.setOutputLimits(-50, 50);  // エレベーター出力制限
    rollPID.setOutputLimits(-50, 50);   // エルロン出力制限
    yawPID.setOutputLimits(-30, 30);    // ラダー出力制限
    Serial.println("AutoControl initialized - ACCELERATION CONTROL MODE");
#endif
    
    lastUpdateTime = millis();
}

#ifdef USE_ANGLE_CONTROL
void AutoControl::setTargets(float pitch, float roll, float yaw) {
    targetPitch = pitch;
    targetRoll = roll;
    targetYaw = yaw;
}
#endif

#ifdef USE_ACCEL_CONTROL
void AutoControl::setAccelTargets(float accelX, float accelY, float accelZ) {
    targetAccelX = accelX;
    targetAccelY = accelY;
    targetAccelZ = accelZ;
}
#endif

void AutoControl::update(MPU6050& mpu) {
    unsigned long currentTime = millis();
    float deltaTime = (currentTime - lastUpdateTime) / 1000.0;
    
    if (deltaTime < 0.001) return; // 更新頻度制限
    
#ifdef USE_ANGLE_CONTROL
    // 角度制御モード
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
#endif

#ifdef USE_ACCEL_CONTROL
    // 加速度制御モード
    // 加速度データ取得
    float accX = mpu.getAccX();
    float accY = mpu.getAccY();
    float accZ = mpu.getAccZ();
    
    // ローパスフィルターで加速度をスムージング
    float alpha = 0.8; // フィルターの重み
    currentAccelX = alpha * currentAccelX + (1 - alpha) * accX;
    currentAccelY = alpha * currentAccelY + (1 - alpha) * accY;
    currentAccelZ = alpha * currentAccelZ + (1 - alpha) * accZ;
    
    // ピッチとロール用のフィルターも加速度ベースで更新
    pitchFilter = currentAccelX;  // X軸加速度をピッチ制御に使用
    rollFilter = currentAccelY;   // Y軸加速度をロール制御に使用
#endif
    
    lastUpdateTime = currentTime;
}

float AutoControl::getElevatorOutput() {
    if (!enablePitchControl) return 0;
#ifdef USE_ANGLE_CONTROL
    return pitchPID.calculate(targetPitch, pitchFilter);
#endif
#ifdef USE_ACCEL_CONTROL
    return pitchPID.calculate(targetAccelX, currentAccelX);
#endif
}

float AutoControl::getRudderOutput() {
    if (!enableYawControl) return 0;
#ifdef USE_ANGLE_CONTROL
    return yawPID.calculate(targetYaw, currentYaw);
#endif
#ifdef USE_ACCEL_CONTROL
    return yawPID.calculate(targetAccelZ, currentAccelZ);
#endif
}

float AutoControl::getAileronOutput() {
    if (!enableRollControl) return 0;
#ifdef USE_ANGLE_CONTROL
    return rollPID.calculate(targetRoll, rollFilter);
#endif
#ifdef USE_ACCEL_CONTROL
    return rollPID.calculate(targetAccelY, currentAccelY);
#endif
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
#ifdef USE_ANGLE_CONTROL
    Serial.print("Angle - Pitch: ");
    Serial.print(currentPitch, 2);
    Serial.print(" -> ");
    Serial.print(getElevatorOutput(), 2);
    Serial.print(", Roll: ");
    Serial.print(currentRoll, 2);
    Serial.print(", Yaw: ");
    Serial.print(currentYaw, 2);
    Serial.print(" -> ");
    Serial.println(getRudderOutput(), 2);
#endif
#ifdef USE_ACCEL_CONTROL
    Serial.print("Accel - X: ");
    Serial.print(currentAccelX, 3);
    Serial.print(" -> ");
    Serial.print(getElevatorOutput(), 2);
    Serial.print(", Y: ");
    Serial.print(currentAccelY, 3);
    Serial.print(", Z: ");
    Serial.print(currentAccelZ, 3);
    Serial.print(" -> ");
    Serial.println(getRudderOutput(), 2);
#endif
}

void AutoControl::reset() {
    pitchPID.reset();
    rollPID.reset();
    yawPID.reset();
#ifdef USE_ANGLE_CONTROL
    currentPitch = 0;
    currentRoll = 0;
    currentYaw = 0;
#endif
#ifdef USE_ACCEL_CONTROL
    currentAccelX = 0;
    currentAccelY = 0;
    currentAccelZ = -1.0;  // 重力分
#endif
    pitchFilter = 0;
    rollFilter = 0;
    lastUpdateTime = 0;
}
