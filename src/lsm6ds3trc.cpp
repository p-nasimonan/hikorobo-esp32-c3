#include "lsm6ds3trc.h"

LSM6DS3TRC::LSM6DS3TRC(uint8_t address) : i2cAddress(address), initialized(false) {
    rawGyroX = rawGyroY = rawGyroZ = 0;
    rawAccelX = rawAccelY = rawAccelZ = 0;
}

bool LSM6DS3TRC::begin() {
    // 複数のアドレスを試行
    uint8_t addresses[] = {LSM6DS3TRC_ADDRESS_PRIMARY, LSM6DS3TRC_ADDRESS_SECONDARY};
    bool deviceFound = false;
    
    for (int addr_idx = 0; addr_idx < 2; addr_idx++) {
        i2cAddress = addresses[addr_idx];
        Serial.print("LSM6DS3TRC アドレス 0x");
        Serial.print(i2cAddress, HEX);
        Serial.print(" を試行中...");
        
        // I2C接続テスト
        Wire.beginTransmission(i2cAddress);
        if (Wire.endTransmission() != 0) {
            Serial.println(" 接続失敗");
            continue;
        }
        
        // WHO_AM_I レジスタを確認
        uint8_t whoAmI = readRegister(LSM6DS3TRC_WHO_AM_I);
        Serial.print(" WHO_AM_I: 0x");
        Serial.println(whoAmI, HEX);
        
        if (whoAmI == LSM6DS3TRC_WHO_AM_I_RESPONSE || whoAmI == 0x69) {  // 0x69はLSM6DS3用
            Serial.println("LSM6DS3TRC/LSM6DS3 検出成功");
            deviceFound = true;
            break;
        }
    }
    
    if (!deviceFound) {
        Serial.println("LSM6DS3TRC 検出失敗");
        return false;
    }
    
    // ソフトリセット
    writeRegister(LSM6DS3TRC_CTRL3_C, LSM6DS3TRC_CTRL3_C_SW_RESET);
    delay(50);  // リセット待機時間を延長
    
    // SlimeVRの設定パラメータを適用
    writeRegister(LSM6DS3TRC_CTRL1_XL, LSM6DS3TRC_CTRL1_XL_VALUE);  // 加速度計: 8g, 416Hz
    delay(10);
    writeRegister(LSM6DS3TRC_CTRL2_G, LSM6DS3TRC_CTRL2_G_VALUE);    // ジャイロ: 1000dps, 416Hz
    delay(10);
    writeRegister(LSM6DS3TRC_CTRL3_C, LSM6DS3TRC_CTRL3_C_VALUE);    // BDU=1, IF_INC=1
    delay(10);
    
    Serial.println("LSM6DS3TRC 初期化完了");
    Serial.println("  加速度計: 8g, 416Hz");
    Serial.println("  ジャイロ: 1000dps, 416Hz");
    
    initialized = true;
    return true;
}

bool LSM6DS3TRC::isInitialized() {
    return initialized;
}

void LSM6DS3TRC::readSensorData() {
    if (!initialized) return;
    
    uint8_t buffer[12];
    
    // ジャイロと加速度データを連続読み込み (0x22-0x2D)
    readMultipleRegisters(LSM6DS3TRC_OUTX_L_G, buffer, 12);
    
    // ジャイロデータ (0x22-0x27)
    rawGyroX = (int16_t)(buffer[1] << 8 | buffer[0]);
    rawGyroY = (int16_t)(buffer[3] << 8 | buffer[2]);
    rawGyroZ = (int16_t)(buffer[5] << 8 | buffer[4]);
    
    // 加速度データ (0x28-0x2D)
    rawAccelX = (int16_t)(buffer[7] << 8 | buffer[6]);
    rawAccelY = (int16_t)(buffer[9] << 8 | buffer[8]);
    rawAccelZ = (int16_t)(buffer[11] << 8 | buffer[10]);
}

float LSM6DS3TRC::getGyroX() {
    return rawGyroX / LSM6DS3TRC_GYRO_SENSITIVITY;
}

float LSM6DS3TRC::getGyroY() {
    return rawGyroY / LSM6DS3TRC_GYRO_SENSITIVITY;
}

float LSM6DS3TRC::getGyroZ() {
    return rawGyroZ / LSM6DS3TRC_GYRO_SENSITIVITY;
}

float LSM6DS3TRC::getAccelX() {
    return rawAccelX / LSM6DS3TRC_ACCEL_SENSITIVITY * 9.81f; // m/s²に変換
}

float LSM6DS3TRC::getAccelY() {
    return rawAccelY / LSM6DS3TRC_ACCEL_SENSITIVITY * 9.81f;
}

float LSM6DS3TRC::getAccelZ() {
    return rawAccelZ / LSM6DS3TRC_ACCEL_SENSITIVITY * 9.81f;
}

void LSM6DS3TRC::writeRegister(uint8_t reg, uint8_t value) {
    Wire.beginTransmission(i2cAddress);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
}

uint8_t LSM6DS3TRC::readRegister(uint8_t reg) {
    Wire.beginTransmission(i2cAddress);
    Wire.write(reg);
    byte error = Wire.endTransmission(false);
    
    if (error != 0) {
        Serial.print("I2C書き込みエラー: ");
        Serial.println(error);
        return 0;
    }
    
    uint8_t bytesReceived = Wire.requestFrom(i2cAddress, (uint8_t)1);
    if (bytesReceived != 1) {
        Serial.print("I2C読み取りエラー: 期待1バイト、受信");
        Serial.print(bytesReceived);
        Serial.println("バイト");
        return 0;
    }
    
    if (Wire.available()) {
        return Wire.read();
    }
    return 0;
}

void LSM6DS3TRC::readMultipleRegisters(uint8_t reg, uint8_t* buffer, uint8_t length) {
    Wire.beginTransmission(i2cAddress);
    Wire.write(reg);
    Wire.endTransmission(false);
    
    Wire.requestFrom(i2cAddress, length);
    
    for (uint8_t i = 0; i < length; i++) {
        if (Wire.available()) {
            buffer[i] = Wire.read();
        } else {
            buffer[i] = 0;
        }
    }
}
