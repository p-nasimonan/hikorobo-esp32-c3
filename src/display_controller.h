#ifndef DISPLAY_CONTROLLER_H
#define DISPLAY_CONTROLLER_H

#include <Arduino.h>
#include <U8g2lib.h>

class DisplayController {
private:
    U8G2_SSD1306_72X40_ER_F_SW_I2C* display;
    int sdaPin;
    int sclPin;
    bool initialized;
    
    // 表示更新間隔
    static constexpr unsigned long DISPLAY_UPDATE_INTERVAL = 500;  // 500ms
    unsigned long lastUpdate;
    
    // 表示用バッファ
    String statusLine;
    String gyroLine;
    String modeLine;
    
public:
    DisplayController(int sda_pin, int scl_pin);
    void begin();
    void update();
    bool isInitialized();
    
    // 表示内容更新
    void updateDisplay();
    void updateDisplayWithGyroData(float gx, float gy, float gz, float temp);
    void showStartupMessage();
    void showError(String message);
};

#endif
