#include <Arduino.h>
#include <Wire.h>
#include <MPU6050_tockn.h>
#include "rc_receiver.h"
#include "servo_output.h"
#include "led_output.h"
#include "display_controller.h"

// ピン定義
const int SDA_PIN = 5;         // I2C SDA
const int SCL_PIN = 6;         // I2C SCL
const int ELEVATOR_INPUT_PIN = 1;   // エレベーター受信ピン
const int ELEVATOR_SERVO_PIN = 2;   // エレベーターサーボピン
const int RUDDER_INPUT_PIN = 21;    // ラダー受信ピン
const int RUDDER_SERVO_PIN = 20;    // ラダーサーボピン
const int LED_INPUT_PIN = 4;        // LED制御信号受信ピン
const int LED_OUTPUT_PIN = 0;       // LED出力ピン

// オブジェクト
MPU6050 mpu6050(Wire);
RCReceiver rcReceiver(ELEVATOR_INPUT_PIN, RUDDER_INPUT_PIN, LED_INPUT_PIN);
ServoOutput elevatorServo(ELEVATOR_SERVO_PIN, "エレベーター");
ServoOutput rudderServo(RUDDER_SERVO_PIN, "ラダー");
LedOutput ledOutput(LED_OUTPUT_PIN);
DisplayController displayController(SDA_PIN, SCL_PIN);

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("ESP32-C3 RC System Start");
  
  // I2C初期化（ジャイロとディスプレイ共用）
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(400000);  // 400kHz
  delay(700);
  
  // MPU6050初期化（エラーハンドリング付き）
  bool mpu6050Found = false;
  
  // 0x68をチェック
  Wire.beginTransmission(0x68);
  if (Wire.endTransmission() == 0) {
    mpu6050Found = true;
    Serial.println("MPU6050 found at 0x68");
  }
  
  // 0x68で見つからなかった場合、0x69をチェック
  if (!mpu6050Found) {
    Wire.beginTransmission(0x69);
    if (Wire.endTransmission() == 0) {
      mpu6050Found = true;
      Serial.println("MPU6050 found at 0x69");
    }
  }
  
  if (mpu6050Found) {
    mpu6050.begin();
    mpu6050.calcGyroOffsets(true);
    Serial.println("MPU6050 OK");
  } else {
    Serial.println("MPU6050 Error - Passthrough only");
  }
  
  // 各コントローラーの初期化
  // displayController.begin();
  ledOutput.begin();
  rcReceiver.begin();
  elevatorServo.begin();
  rudderServo.begin();
  
  Serial.println("System Ready");
}

void loop() {
  // RC受信機の状態を確認（最初に判定）
  bool isPassthrough = rcReceiver.isPassthroughMode();
  
  // LED制御処理（パススルーモードの時オン、姿勢制御の時オフ）
  ledOutput.setState(isPassthrough);
  

  // // ディスプレイ更新（パススルーモード時は頻度を下げる）
  // static unsigned long lastDisplayUpdate = 0;
  // unsigned long displayInterval = isPassthrough ? 2000 : 500;  // パススルー時は2000ms、姿勢制御時は500ms
  
  
  // 姿勢制御モードの場合のみジャイロ処理を実行
  if (!isPassthrough) {
    // データ更新
    mpu6050.update();
    
    // データ取得
    float gyroX = mpu6050.getGyroX();
    float gyroY = mpu6050.getGyroY();
    float gyroZ = mpu6050.getGyroZ();
    
    float accX = mpu6050.getAccX();
    float accY = mpu6050.getAccY();
    float accZ = mpu6050.getAccZ();
    
    float temp = mpu6050.getTemp();


  //   if (millis() - lastDisplayUpdate > displayInterval) {
  //     displayController.updateDisplayWithGyroData(
  //         gyroX, gyroY, 
  //         gyroZ, temp);
  //     lastDisplayUpdate = millis();
  //     }
  }else{

  // RC受信機からの入力を取得
  float elevatorInput = rcReceiver.getElevatorValue();
  float rudderInput = rcReceiver.getRudderValue();
  
  float elevatorOutput = elevatorInput;
  float rudderOutput = rudderInput;
    
  // サーボに出力
  elevatorServo.writeValue(elevatorOutput);
  rudderServo.writeValue(rudderOutput);
  

  // if (millis() - lastDisplayUpdate > displayInterval) {
  //   displayController.updateDisplay();
  //   lastDisplayUpdate = millis();
  // }
  }
}
