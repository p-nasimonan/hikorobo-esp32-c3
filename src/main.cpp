#include <Arduino.h>

// ピンの定義
const int RC_INPUT_PIN = 0;    // ラジコン受信機の信号線
const int LED_PIN = 1;         // LED接続ピン（ESP32の内蔵LED）

// PWM設定
const int PWM_CHANNEL = 0;
const int PWM_FREQUENCY = 5000;  // 5kHz
const int PWM_RESOLUTION = 8;    // 8ビット解像度（0-255）

// 信号の範囲設定
const int RC_MIN = 1000;  // ラジコン信号の最小値（マイクロ秒）
const int RC_MAX = 2000;  // ラジコン信号の最大値（マイクロ秒）

void setup() {
  Serial.begin(115200);
  Serial.println("ラジコン信号読み取りとLED制御を開始します");
  
  // 受信機信号ピンを入力に設定
  pinMode(RC_INPUT_PIN, INPUT);
  
  // LEDピンをPWM出力に設定
  ledcSetup(PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcAttachPin(LED_PIN, PWM_CHANNEL);
  
  Serial.println("セットアップ完了");
}

void loop() {
  // ラジコン受信機からのPWM信号を読み取り（マイクロ秒）
  unsigned long pulseWidth = pulseIn(RC_INPUT_PIN, HIGH, 25000);  // タイムアウト25ms
  
  if (pulseWidth > 0) {
    // 信号が有効な場合
    Serial.print("受信信号: ");
    Serial.print(pulseWidth);
    Serial.println(" μs");
    
    // PWM信号をLEDの明るさに変換（0-255）
    int ledBrightness;
    if (pulseWidth < RC_MIN) {
      ledBrightness = 0;
    } else if (pulseWidth > RC_MAX) {
      ledBrightness = 255;
    } else {
      // 線形変換: RC_MIN-RC_MAX → 0-255
      ledBrightness = map(pulseWidth, RC_MIN, RC_MAX, 0, 255);
    }
    
    // LEDの明るさを設定
    ledcWrite(PWM_CHANNEL, ledBrightness);
    
    Serial.print("LED明るさ: ");
    Serial.println(ledBrightness);
  } else {
    // 信号が読み取れない場合はLEDを消灯
    ledcWrite(PWM_CHANNEL, 0);
    Serial.println("信号なし - LED消灯");
  }
  
  delay(50);  // 50ms待機
}
