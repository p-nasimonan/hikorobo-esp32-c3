#ifndef IMU_SENSOR_H
#define IMU_SENSOR_H

#include <Arduino.h>

// センサータイプ定義
enum SensorType {
    SENSOR_LSM6DS3TRC,
    SENSOR_MPU6050,
    SENSOR_AUTO_DETECT
};

// 共通インターフェース
class IMUSensor {
public:
    virtual ~IMUSensor() = default;
    virtual bool begin() = 0;
    virtual bool isInitialized() = 0;
    virtual void readSensorData() = 0;
    
    virtual float getGyroX() = 0;
    virtual float getGyroY() = 0;
    virtual float getGyroZ() = 0;
    
    virtual float getAccelX() = 0;
    virtual float getAccelY() = 0;
    virtual float getAccelZ() = 0;
    
    virtual const char* getSensorName() = 0;
};

#endif
