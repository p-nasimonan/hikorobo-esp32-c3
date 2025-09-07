#include <Arduino.h>
#include <Wire.h>
#include "led_controller.h"
#include "servo_controller.h"
#include "gyro_controller.h"
#include "display_controller.h"

// センサー選択 (以下の中から選択)
// SENSOR_AUTO_DETECT: 自動検出 (推奨)
// SENSOR_MPU6050: MPU6050を強制使用
// SENSOR_LSM6DS3TRC: LSM6DS3TRCを強制使用
#define SENSOR_TYPE SENSOR_AUTO_DETECT

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
ServoController elevatorController(ELEVATOR_INPUT_PIN, ELEVATOR_SERVO_PIN, "エレベーター", "elevator");
ServoController rudderController(RUDDER_INPUT_PIN, RUDDER_SERVO_PIN, "ラダー", "rudder");
GyroController gyroController(SDA_PIN, SCL_PIN, LED_INPUT_PIN, SENSOR_TYPE);  // センサータイプ指定
DisplayController displayController(SDA_PIN, SCL_PIN);

void setup() {
  Serial.begin(115200);
  delay(1000);  // シリアル初期化待機
  Serial.println("ESP32-C3 ラジコン制御システム開始");
  
  // I2C初期化（ジャイロとディスプレイ共用）
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(400000);  // 最初から400kHzで開始
  Serial.println("I2C初期化完了 (400kHz)");
  delay(500);  // I2C安定化待機
  
  // I2Cデバイススキャン
  Serial.println("I2Cデバイススキャン開始...");
  int deviceCount = 0;
  for (byte address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();
    
    if (error == 0) {
      Serial.print("I2Cデバイス発見: 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
      deviceCount++;
      
      if (address == 0x6A) {
        Serial.println("  -> LSM6DS3TR-C (Primary)");
      } else if (address == 0x6B) {
        Serial.println("  -> LSM6DS3TR-C (Secondary)");
      } else if (address == 0x3C || address == 0x3D) {
        Serial.println("  -> SSD1306 ディスプレイ");
      }
    }
  }
  
  if (deviceCount == 0) {
    Serial.println("I2Cデバイスが見つかりません");
  } else {
    Serial.print("合計 ");
    Serial.print(deviceCount);
    Serial.println(" 個のデバイスが見つかりました");
  }
  
  // 各コントローラーの初期化（順序重要）
  Serial.println("ディスプレイ初期化中...");
  displayController.begin();
  delay(100);  // 短縮
  
  Serial.println("ジャイロセンサー初期化中...");
  gyroController.begin();  // 400kHz固定で初期化
  delay(100);  // 短縮
  
  ledController.begin();
  elevatorController.begin();
  rudderController.begin();
  
  // ジャイロコントローラーを各コントローラーに設定
  elevatorController.setGyroController(&gyroController);
  rudderController.setGyroController(&gyroController);
  displayController.setGyroController(&gyroController);
  
  Serial.println("初期化完了");
  Serial.println("LEDスイッチでジャイロ補正ON/OFF切り替え");
}

void loop() {
  // ジャイロセンサーの更新
  gyroController.update();
  
  // 各コントローラーの更新
  elevatorController.update();
  rudderController.update();
  ledController.update();
  
  // ディスプレイ更新
  displayController.update();
  
  delay(20);  // 50Hz更新レート（サーボ制御に適した頻度）
}
