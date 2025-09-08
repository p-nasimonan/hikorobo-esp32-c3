#include "display_controller.h"

// グローバルディスプレイオブジェクト（OLED: SDA=5, SCL=6）
U8G2_SSD1306_72X40_ER_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ 6, /* data=*/ 5, /* reset=*/ U8X8_PIN_NONE);

DisplayController::DisplayController(int sda_pin, int scl_pin) {
    sdaPin = sda_pin;
    sclPin = scl_pin;
    display = &u8g2;
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
    
    // ライン2: モード表示
    modeLine = "Mode: MPU6050";
    display->drawStr(0, 18, modeLine.c_str());
    
    // ライン3-4: 固定表示
    display->drawStr(0, 28, "RC System");
    display->drawStr(0, 38, "Running...");
    
    display->sendBuffer();
}

void DisplayController::updateDisplayWithGyroData(float gx, float gy, float gz, float temp) {
    display->clearBuffer();
    display->setFont(u8g2_font_4x6_tr);
    
    // ライン1: システム状態
    display->drawStr(0, 8, "ESP32-C3 Ready");
    
    // ライン2: モード表示
    display->drawStr(0, 18, "Mode: MPU6050");
    
    // ライン3-4: ジャイロデータ
    String gyroX = "X:" + String(gx, 1);
    String gyroY = "Y:" + String(gy, 1);
    String tempStr = String(temp, 0) + "C";
    
    display->drawStr(0, 28, gyroX.c_str());
    display->drawStr(24, 28, gyroY.c_str());
    display->drawStr(48, 28, tempStr.c_str());
    display->drawStr(0, 38, ("Z:" + String(gz, 1)).c_str());
    
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
