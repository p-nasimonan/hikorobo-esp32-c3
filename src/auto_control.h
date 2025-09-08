#ifndef AUTO_CONTROL_H
#define AUTO_CONTROL_H

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
    
    // 現在の角度（積分により計算）
    float currentPitch;
    float currentRoll;
    float currentYaw;
    
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
    void setTargets(float pitch, float roll, float yaw);
    void setTargetPitch(float pitch) { targetPitch = pitch; }
    void setTargetRoll(float roll) { targetRoll = roll; }
    void setTargetYaw(float yaw) { targetYaw = yaw; }
    
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
    
    // 現在の角度取得
    float getCurrentPitch() const { return currentPitch; }
    float getCurrentRoll() const { return currentRoll; }
    float getCurrentYaw() const { return currentYaw; }
    
    // デバッグ情報
    void printDebugInfo();
    
    // リセット
    void reset();
};

#endif
