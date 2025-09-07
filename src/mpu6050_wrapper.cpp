#include "mpu6050_wrapper.h"

MPU6050Wrapper::MPU6050Wrapper() : mpu(Wire), initialized(false) {
}

bool MPU6050Wrapper::begin() {
    Serial.println("MPU6050 (tockn) 初期化中...");
    
    // MPU6050初期化
    mpu.begin();
    delay(100);
    
    // キャリブレーション実行
    Serial.println("MPU6050キャリブレーション中...");
    mpu.calcGyroOffsets(true);  // trueで詳細表示
    
    Serial.println("MPU6050 初期化・キャリブレーション完了");
    
    initialized = true;
    return true;
}

bool MPU6050Wrapper::isInitialized() {
    return initialized;
}

void MPU6050Wrapper::readSensorData() {
    if (!initialized) return;
    
    // tocknライブラリでデータ読み取り
    mpu.update();
}

float MPU6050Wrapper::getGyroX() {
    return mpu.getGyroX();  // deg/sで出力
}

float MPU6050Wrapper::getGyroY() {
    return mpu.getGyroY();
}

float MPU6050Wrapper::getGyroZ() {
    return mpu.getGyroZ();
}

float MPU6050Wrapper::getAccelX() {
    return mpu.getAccX() * 9.81f;  // gをm/s²に変換
}

float MPU6050Wrapper::getAccelY() {
    return mpu.getAccY() * 9.81f;
}

float MPU6050Wrapper::getAccelZ() {
    return mpu.getAccZ() * 9.81f;
}

const char* MPU6050Wrapper::getSensorName() {
    return "MPU6050 (tockn)";
}

float MPU6050Wrapper::getTemperature() {
    if (!initialized) return 0.0f;
    
    return mpu.getTemp();  // 摂氏温度
}
