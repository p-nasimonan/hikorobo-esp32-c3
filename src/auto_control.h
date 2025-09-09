#ifndef AUTO_CONTROL_H
#define AUTO_CONTROL_H

// 制御モード選択（どちらか一つをコメントアウト）
#define USE_ANGLE_CONTROL     // 角度制御モード
// #define USE_ACCEL_CONTROL     // 加速度制御モード

#include "pid_controller.h"
#include <MPU6050_tockn.h>

class AutoControl {
private:
    PIDController pitchPID;     // ピッチ制御用PID
    PIDController rollPID;      // ロール制御用PID
    PIDController yawPID;       // ヨー制御用PID
    
    // 目標値
    float targetPitch;
    float targetRoll;
    float targetYaw;
    
#ifdef USE_ANGLE_CONTROL
    // 現在の角度（積分により計算）
    float currentPitch;
    float currentRoll;
    float currentYaw;
#endif

#ifdef USE_ACCEL_CONTROL
    // 目標加速度
    float targetAccelX;
    float targetAccelY;
    float targetAccelZ;
    
    // 現在の加速度
    float currentAccelX;
    float currentAccelY;
    float currentAccelZ;
#endif
    
    // 角度積分用
    unsigned long lastUpdateTime;
    
    // 制御有効フラグ
    bool enablePitchControl;
    bool enableRollControl;
    bool enableYawControl;
    
    // フィルター用
    float pitchFilter;
    float rollFilter;
    
public:
    AutoControl();
    
    // 初期化
    void begin();
    
    // 目標値設定
#ifdef USE_ANGLE_CONTROL
    void setTargets(float pitch, float roll, float yaw);
    void setTargetPitch(float pitch) { targetPitch = pitch; }
    void setTargetRoll(float roll) { targetRoll = roll; }
    void setTargetYaw(float yaw) { targetYaw = yaw; }
#endif

#ifdef USE_ACCEL_CONTROL
    void setAccelTargets(float accelX, float accelY, float accelZ);
    void setTargetAccelX(float accelX) { targetAccelX = accelX; }
    void setTargetAccelY(float accelY) { targetAccelY = accelY; }
    void setTargetAccelZ(float accelZ) { targetAccelZ = accelZ; }
#endif
    
    // 制御実行
    void update(MPU6050& mpu);
    
    // 制御出力取得
    float getElevatorOutput();  // ピッチ制御出力
    float getRudderOutput();    // ヨー制御出力
    float getAileronOutput();   // ロール制御出力（将来用）
    
    // PIDパラメータ設定
    void setPitchPID(float kp, float ki, float kd);
    void setRollPID(float kp, float ki, float kd);
    void setYawPID(float kp, float ki, float kd);
    
    // 制御有効/無効
    void enableControl(bool pitch, bool roll, bool yaw);
    
#ifdef USE_ANGLE_CONTROL
    // 現在の角度取得
    float getCurrentPitch() const { return currentPitch; }
    float getCurrentRoll() const { return currentRoll; }
    float getCurrentYaw() const { return currentYaw; }
#endif

#ifdef USE_ACCEL_CONTROL
    // 現在の加速度取得
    float getCurrentAccelX() const { return currentAccelX; }
    float getCurrentAccelY() const { return currentAccelY; }
    float getCurrentAccelZ() const { return currentAccelZ; }
#endif
    
    // デバッグ情報
    void printDebugInfo();
    
    // リセット
    void reset();
};

#endif
