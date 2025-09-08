#ifndef PID_CONTROLLER_H
#define PID_CONTROLLER_H

class PIDController {
private:
    float kp, ki, kd;           // PID係数
    float previousError;        // 前回の誤差
    float integral;             // 積分項
    unsigned long lastTime;     // 前回の時間
    float outputMin, outputMax; // 出力制限

public:
    PIDController(float kp, float ki, float kd);
    
    // PID計算
    float calculate(float setpoint, float input);
    
    // パラメータ設定
    void setGains(float kp, float ki, float kd);
    void setOutputLimits(float min, float max);
    
    // リセット
    void reset();
    
    // デバッグ情報取得
    float getLastError() const { return previousError; }
    float getIntegral() const { return integral; }
};

#endif
