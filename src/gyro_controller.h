#ifndef GYRO_CONTROLLER_H
#define GYRO_CONTROLLER_H

#include <Arduino.h>
#include <Wire.h>
#include "imu_sensor.h"
#include "lsm6ds3trc.h"
#include "mpu6050_wrapper.h"

class GyroController {
private:
    int sdaPin;
    int sclPin;
    int switchPin;  // パススルー/補正切り替えピン
    uint8_t i2cAddress;  // LSM6DS3のI2Cアドレス
    bool initialized;
    bool gyroEnabled;  // ジャイロ補正のON/OFF状態
    IMUSensor* imuSensor;  // 共通インターフェース
    SensorType sensorType; // 使用するセンサータイプ
    
    // センサーインスタンス
    LSM6DS3TRC lsm6ds3trc;
    MPU6050Wrapper mpu6050;
    
    // I2C設定
    static constexpr uint32_t I2C_CLOCK = 400000;  // 400kHz固定
    
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
    bool detectAndInitializeSensor();  // センサー自動検出・初期化
    
public:
    GyroController(int sda_pin, int scl_pin, int switch_pin, SensorType sensor_type = SENSOR_AUTO_DETECT);
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
    
    // センサー情報取得
    const char* getSensorName();
    SensorType getSensorType();
};

#endif
