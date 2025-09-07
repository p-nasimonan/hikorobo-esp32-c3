#ifndef MPU6050_WRAPPER_H
#define MPU6050_WRAPPER_H

#include <Arduino.h>
#include <Wire.h>
#include "imu_sensor.h"

// tockn MPU6050ライブラリのインクルード
#include <MPU6050_tockn.h>

class MPU6050Wrapper : public IMUSensor {
private:
    MPU6050 mpu;
    bool initialized;
    
public:
    MPU6050Wrapper();
    
    // IMUSensorインターフェース実装
    bool begin() override;
    bool isInitialized() override;
    void readSensorData() override;
    
    float getGyroX() override;
    float getGyroY() override;
    float getGyroZ() override;
    
    float getAccelX() override;
    float getAccelY() override;
    float getAccelZ() override;
    
    const char* getSensorName() override;
    
    // 追加メソッド
    float getTemperature();
};

#endif
