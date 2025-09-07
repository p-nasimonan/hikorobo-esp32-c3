#ifndef LSM6DS3TRC_H
#define LSM6DS3TRC_H

#include <Arduino.h>
#include <Wire.h>
#include "imu_sensor.h"

// LSM6DS3TRC レジスタアドレス
#define LSM6DS3TRC_ADDRESS_PRIMARY    0x6A  // SA0 = GND (推奨)
#define LSM6DS3TRC_ADDRESS_SECONDARY  0x6B  // SA0 = VDD

// レジスタ定義
#define LSM6DS3TRC_WHO_AM_I           0x0F
#define LSM6DS3TRC_CTRL1_XL           0x10  // 加速度計制御レジスタ
#define LSM6DS3TRC_CTRL2_G            0x11  // ジャイロ制御レジスタ
#define LSM6DS3TRC_CTRL3_C            0x12  // 共通制御レジスタ
#define LSM6DS3TRC_FIFO_CTRL2         0x07
#define LSM6DS3TRC_FIFO_CTRL3         0x08
#define LSM6DS3TRC_FIFO_CTRL5         0x0A
#define LSM6DS3TRC_FIFO_STATUS        0x3A
#define LSM6DS3TRC_FIFO_DATA          0x3E
#define LSM6DS3TRC_OUTX_L_G           0x22
#define LSM6DS3TRC_OUTX_H_G           0x23
#define LSM6DS3TRC_OUTY_L_G           0x24
#define LSM6DS3TRC_OUTY_H_G           0x25
#define LSM6DS3TRC_OUTZ_L_G           0x26
#define LSM6DS3TRC_OUTZ_H_G           0x27
#define LSM6DS3TRC_OUTX_L_XL          0x28
#define LSM6DS3TRC_OUTX_H_XL          0x29
#define LSM6DS3TRC_OUTY_L_XL          0x2A
#define LSM6DS3TRC_OUTY_H_XL          0x2B
#define LSM6DS3TRC_OUTZ_L_XL          0x2C
#define LSM6DS3TRC_OUTZ_H_XL          0x2D

// WHO_AM_I レスポンス
#define LSM6DS3TRC_WHO_AM_I_RESPONSE  0x6A

// 設定値 (SlimeVRパラメータ参考)
#define LSM6DS3TRC_CTRL1_XL_VALUE     0x62  // 8g, 416Hz (0b11 << 2) | (0b0110 << 4)
#define LSM6DS3TRC_CTRL2_G_VALUE      0x68  // 1000dps, 416Hz (0b10 << 2) | (0b0110 << 4)  
#define LSM6DS3TRC_CTRL3_C_VALUE      0x44  // BDU = 1, IF_INC = 1 (1 << 6) | (1 << 2)
#define LSM6DS3TRC_CTRL3_C_SW_RESET   0x01  // ソフトリセット

// スケーリング因子 (SlimeVRパラメータ)
#define LSM6DS3TRC_GYRO_SENSITIVITY   28.571428571f   // 1000dps range
#define LSM6DS3TRC_ACCEL_SENSITIVITY  4098.360655738f // 8g range

class LSM6DS3TRC : public IMUSensor {
private:
    uint8_t i2cAddress;
    bool initialized;
    
    // 生データ用
    int16_t rawGyroX, rawGyroY, rawGyroZ;
    int16_t rawAccelX, rawAccelY, rawAccelZ;
    
    // レジスタ読み書き関数
    void writeRegister(uint8_t reg, uint8_t value);
    uint8_t readRegister(uint8_t reg);
    void readMultipleRegisters(uint8_t reg, uint8_t* buffer, uint8_t length);
    
public:
    LSM6DS3TRC(uint8_t address = LSM6DS3TRC_ADDRESS_PRIMARY);
    
    // 初期化・確認
    bool begin();
    bool isInitialized();
    
    // データ読み込み
    void readSensorData();
    
    // ジャイロデータ取得 (度/秒)
    float getGyroX();
    float getGyroY();
    float getGyroZ();
    
    // 加速度データ取得 (m/s²)
    float getAccelX();
    float getAccelY(); 
    float getAccelZ();
    
    // 生データ取得
    int16_t getRawGyroX() { return rawGyroX; }
    int16_t getRawGyroY() { return rawGyroY; }
    int16_t getRawGyroZ() { return rawGyroZ; }
    int16_t getRawAccelX() { return rawAccelX; }
    int16_t getRawAccelY() { return rawAccelY; }
    int16_t getRawAccelZ() { return rawAccelZ; }
    
    // センサー名取得 (IMUSensorインターフェース)
    const char* getSensorName() override { return "LSM6DS3TR-C"; }
};

#endif
