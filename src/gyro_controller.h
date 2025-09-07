#ifndef GYRO_CONTROLLER_H
#define GYRO_CONTROLLER_H

#include <Arduino.h>
#include <Wire.h>

class GyroController {
private:
    int sdaPin;
    int sclPin;
    bool initialized;
    
    // ジャイロセンサーの値（将来実装）
    float gyroX, gyroY, gyroZ;
    float accelX, accelY, accelZ;
    
public:
    GyroController(int sda_pin, int scl_pin);
    void begin();
    void update();
    bool isInitialized();
    
    // 将来のジャイロデータ取得関数
    float getGyroX();
    float getGyroY();
    float getGyroZ();
    float getAccelX();
    float getAccelY();
    float getAccelZ();
    
    // サーボ信号にジャイロ補正を適用する関数（将来実装）
    int applyGyroCorrection(int originalSignal, String axis);
};

#endif
