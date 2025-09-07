#include <Arduino.h>
#include "led_controller.h"
#include "servo_controller.h"
#include "gyro_controller.h"

// ピンの定義
const int SDA_PIN = 5;         // I2C SDA (ジャイロセンサー用)
const int SCL_PIN = 6;         // I2C SCL (ジャイロセンサー用)

const int ELEVATOR_INPUT_PIN = 1;   // エレベーター受信ピン
const int ELEVATOR_SERVO_PIN = 2;   // エレベーターサーボピン
const int RUDDER_INPUT_PIN = 21;    // ラダー受信ピン
const int RUDDER_SERVO_PIN = 20;    // ラダーサーボピン
const int LED_INPUT_PIN = 4;        // LED制御信号受信ピン
const int LED_OUTPUT_PIN = 0;       // LED出力ピン

// コントローラーオブジェクト
LedController ledController(LED_INPUT_PIN, LED_OUTPUT_PIN);
ServoController elevatorController(ELEVATOR_INPUT_PIN, ELEVATOR_SERVO_PIN, "エレベーター");
ServoController rudderController(RUDDER_INPUT_PIN, RUDDER_SERVO_PIN, "ラダー");
GyroController gyroController(SDA_PIN, SCL_PIN);

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32-C3 ラジコン信号受信とサーボ制御を開始します");
  
  // 各コントローラーの初期化
  gyroController.begin();
  ledController.begin();
  elevatorController.begin();
  rudderController.begin();
  
  Serial.println("全コントローラー初期化完了");
}

void loop() {
  // ジャイロセンサーの更新
  gyroController.update();
  
  // 各コントローラーの更新
  elevatorController.update();
  rudderController.update();
  ledController.update();
  
  delay(20);  // 50Hz更新レート（サーボ制御に適した頻度）
}
