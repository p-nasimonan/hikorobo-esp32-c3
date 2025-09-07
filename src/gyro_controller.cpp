#include "gyro_controller.h"

GyroController::GyroController(int sda_pin, int scl_pin) {
    sdaPin = sda_pin;
    sclPin = scl_pin;
    initialized = false;
    
    // 初期値設定
    gyroX = gyroY = gyroZ = 0.0;
    accelX = accelY = accelZ = 0.0;
}

void GyroController::begin() {
    // I2C初期化
    Wire.begin(sdaPin, sclPin);
    Serial.println("Gyro Controller I2C初期化完了 (SDA: Pin " + String(sdaPin) + ", SCL: Pin " + String(sclPin) + ")");
    
    // TODO: ジャイロセンサー（MPU6050など）の初期化処理を追加
    // 現在はプレースホルダー
    Serial.println("注意: ジャイロセンサーの実装は未完成です（将来実装予定）");
    
    initialized = true;
}

void GyroController::update() {
    if (!initialized) return;
    
    // TODO: ジャイロセンサーからデータを読み取る処理を追加
    // 現在はプレースホルダー
    
    // デバッグ用（実際のセンサー実装時は削除）
    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate > 1000) {  // 1秒に1回
        Serial.println("Gyro Controller: センサーデータ更新中...");
        lastUpdate = millis();
    }
}

bool GyroController::isInitialized() {
    return initialized;
}

float GyroController::getGyroX() {
    return gyroX;
}

float GyroController::getGyroY() {
    return gyroY;
}

float GyroController::getGyroZ() {
    return gyroZ;
}

float GyroController::getAccelX() {
    return accelX;
}

float GyroController::getAccelY() {
    return accelY;
}

float GyroController::getAccelZ() {
    return accelZ;
}

int GyroController::applyGyroCorrection(int originalSignal, String axis) {
    // TODO: ジャイロデータを使用した補正処理を実装
    // 現在はオリジナル信号をそのまま返す（パススルー）
    
    if (!initialized) {
        return originalSignal;
    }
    
    // 将来の実装例:
    // if (axis == "elevator") {
    //     // エレベーター軸のジャイロ補正
    //     float correction = gyroX * CORRECTION_FACTOR;
    //     return constrain(originalSignal + correction, SERVO_MIN, SERVO_MAX);
    // } else if (axis == "rudder") {
    //     // ラダー軸のジャイロ補正
    //     float correction = gyroZ * CORRECTION_FACTOR;
    //     return constrain(originalSignal + correction, SERVO_MIN, SERVO_MAX);
    // }
    
    return originalSignal;  // 現在はパススルー
}
