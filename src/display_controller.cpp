#include "display_controller.h"
#include "gyro_controller.h"

// グローバルディスプレイオブジェクト
U8G2_SSD1306_72X40_ER_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ 6, /* data=*/ 5, /* reset=*/ U8X8_PIN_NONE);

DisplayController::DisplayController(int sda_pin, int scl_pin) {
    sdaPin = sda_pin;
    sclPin = scl_pin;
    display = &u8g2;
    gyroController = nullptr;
    initialized = false;
    lastUpdate = 0;
}

void DisplayController::begin() {
    // ディスプレイ初期化
    display->begin();
    display->clearBuffer();
    display->setFont(u8g2_font_4x6_tr);  // 小さなフォント
    
    showStartupMessage();
    display->sendBuffer();
    
    initialized = true;
    Serial.println("Display 初期化完了");
}

void DisplayController::setGyroController(GyroController* gyro) {
    gyroController = gyro;
}

bool DisplayController::isInitialized() {
    return initialized;
}

void DisplayController::update() {
    if (!initialized) return;
    
    // 定期的に表示更新
    if (millis() - lastUpdate >= DISPLAY_UPDATE_INTERVAL) {
        updateDisplay();
        lastUpdate = millis();
    }
}

void DisplayController::updateDisplay() {
    display->clearBuffer();
    display->setFont(u8g2_font_4x6_tr);
    
    // ライン1: システム状態
    statusLine = "ESP32-C3 Ready";
    display->drawStr(0, 8, statusLine.c_str());
    
    if (gyroController != nullptr) {
        // ライン2: モード表示
        modeLine = gyroController->isGyroEnabled() ? "Mode: GYRO" : "Mode: PASS";
        display->drawStr(0, 18, modeLine.c_str());
        
        if (gyroController->isInitialized() && gyroController->isGyroEnabled()) {
            // ライン3-4: ジャイロデータ
            float gx = gyroController->getGyroX();
            float gy = gyroController->getGyroY();
            float gz = gyroController->getGyroZ();
            
            String gyroX = "X:" + String(gx, 1);
            String gyroY = "Y:" + String(gy, 1);
            String gyroZ = "Z:" + String(gz, 1);
            
            display->drawStr(0, 28, gyroX.c_str());
            display->drawStr(24, 28, gyroY.c_str());
            display->drawStr(0, 38, gyroZ.c_str());
            display->drawStr(24, 38, "dps");
        } else if (!gyroController->isInitialized()) {
            display->drawStr(0, 28, "Gyro: ERROR");
        } else {
            display->drawStr(0, 28, "Gyro: OFF");
        }
    } else {
        display->drawStr(0, 18, "No Gyro");
    }
    
    display->sendBuffer();
}

void DisplayController::showStartupMessage() {
    display->clearBuffer();
    display->setFont(u8g2_font_4x6_tr);
    display->drawStr(0, 8, "ESP32-C3");
    display->drawStr(0, 18, "RC System");
    display->drawStr(0, 28, "I2C:400kHz");
    display->drawStr(0, 38, "Starting...");
    display->sendBuffer();
    delay(800);  // 短縮
}

void DisplayController::showError(String message) {
    display->clearBuffer();
    display->setFont(u8g2_font_4x6_tr);
    display->drawStr(0, 8, "ERROR:");
    display->drawStr(0, 18, message.c_str());
    display->sendBuffer();
}
