#ifndef RC_RECEIVER_H
#define RC_RECEIVER_H

#include <Arduino.h>

class RCReceiver {
private:
    int elevatorPin;
    int rudderPin;
    int ledPin;  // LED制御信号ピン追加
    
    // 最新の値を保存
    volatile unsigned long elevatorPulseStart = 0;
    volatile unsigned long elevatorPulseWidth = 1500;
    volatile unsigned long rudderPulseStart = 0;
    volatile unsigned long rudderPulseWidth = 1500;
    volatile unsigned long ledPulseStart = 0;
    volatile unsigned long ledPulseWidth = 1500;
    
    // 割り込み処理用の静的変数
    static RCReceiver* instance;
    
    // 割り込みハンドラー
    static void IRAM_ATTR elevatorISR();
    static void IRAM_ATTR rudderISR();
    static void IRAM_ATTR ledISR();
    
public:
    RCReceiver(int elevator_pin, int rudder_pin, int led_pin);
    void begin();
    
    // パルス幅を取得（マイクロ秒）
    unsigned long getElevatorPulseWidth();
    unsigned long getRudderPulseWidth();
    unsigned long getLedPulseWidth();
    
    // -100 から +100 の値に変換
    float getElevatorValue();
    float getRudderValue();
    float getLedValue();
    
    // 信号が有効かチェック
    bool isElevatorValid();
    bool isRudderValid();
    bool isLedValid();
    
    // LED制御モード判定（パススルーモード = true）
    bool isPassthroughMode();
};

#endif
