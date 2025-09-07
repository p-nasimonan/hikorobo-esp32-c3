#ifndef GYRO_CONTROLLER_H
#define GYRO_CONTROLLER_H

#include <Arduino.h>
#include <Wire.h>
#include <Arduino_LSM6DS3.h>

class GyroController {
private:
    int sdaPin;
    int sclPin;
    int switchPin;  // パススルー/補正切り替えピン
    bool initialized;
    bool gyroEnabled;  // ジャイロ補正のON/OFF状態
    
    // センサー値のフィルタリング用
    static const int FILTER_SIZE = 5;
    float gyroXBuffer[FILTER_SIZE];
    float gyroYBuffer[FILTER_SIZE];
    float gyroZBuffer[FILTER_SIZE];
    int bufferIndex;
    
    // ジャイロセンサーの値
    float gyroX, gyroY, gyroZ;
    float accelX, accelY, accelZ;
    
    // 補正パラメータ
    static constexpr float GYRO_THRESHOLD = 2.0;  // ノイズ閾値（度/秒）
    static constexpr float CORRECTION_FACTOR = 0.1;  // 補正係数
    static constexpr int RC_CENTER = 1500;  // スイッチ判定用中央値
    
    // フィルタリング関数
    float applyMovingAverage(float* buffer, float newValue);
    void updateSwitchState();  // スイッチ状態更新
    
public:
    GyroController(int sda_pin, int scl_pin, int switch_pin);
    void begin();
    void update();
    bool isInitialized();
    bool isGyroEnabled();  // ジャイロ補正が有効かどうか
    
    // センサーデータ取得関数
    float getGyroX();
    float getGyroY();
    float getGyroZ();
    float getAccelX();
    float getAccelY();
    float getAccelZ();
    
    // サーボ信号にジャイロ補正を適用する関数
    int applyGyroCorrection(int originalAngle, String axis);
    
    // キャリブレーション関数
    void calibrate(int samples = 100);
};

#endif
