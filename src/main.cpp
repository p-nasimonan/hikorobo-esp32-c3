#include <Arduino.h>
#include <Wire.h>
#include <MPU6050_tockn.h>
#include "rc_receiver.h"
#include "servo_output.h"
#include "led_output.h"
#include "display_controller.h"
#include "auto_control.h"

// ピン定義
const int SDA_PIN = 5;         // I2C SDA
const int SCL_PIN = 6;         // I2C SCL
const int ELEVATOR_INPUT_PIN = 21;   // エレベーター受信ピン
const int ELEVATOR_SERVO_PIN = 20;   // エレベーターサーボピン
const int RUDDER_INPUT_PIN = 1;    // ラダー受信ピン
const int RUDDER_SERVO_PIN = 2;    // ラダーサーボピン
const int LED_INPUT_PIN = 4;        // LED制御信号受信ピン
const int LED_OUTPUT_PIN = 0;       // LED出力ピン

// オブジェクト
MPU6050 mpu6050(Wire);
RCReceiver rcReceiver(ELEVATOR_INPUT_PIN, RUDDER_INPUT_PIN, LED_INPUT_PIN);
ServoOutput elevatorServo(ELEVATOR_SERVO_PIN, "エレベーター");
ServoOutput rudderServo(RUDDER_SERVO_PIN, "ラダー");
LedOutput ledOutput(LED_OUTPUT_PIN);
DisplayController displayController(SDA_PIN, SCL_PIN);
AutoControl autoControl;

// 制御モード管理
bool mpu6050Available = false;
bool previousPassthroughMode = true;  // 前回のパススルーモード状態

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
    mpu6050Available = true;
    Serial.println("MPU6050 OK");
    
    // 自動制御システム初期化
    autoControl.begin();
    autoControl.setTargets(0, 0, 0);  // 水平飛行目標
  } else {
    Serial.println("MPU6050 Error - Passthrough only");
    mpu6050Available = false;
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
  
  // デバッグ出力用
  static unsigned long lastDebugTime = 0;
  static unsigned long lastControlTime = 0;
  
  // 制御モード切り替わりを検出
  bool modeChanged = (isPassthrough != previousPassthroughMode);
  
  // 制御モード（姿勢制御）
  if (!isPassthrough && mpu6050Available) {
    // MPU6050データ更新
    mpu6050.update();
    
    // 自動制御システム更新
    autoControl.update(mpu6050);
    
    // パススルーから制御モードに切り替わった瞬間
    if (modeChanged && previousPassthroughMode) {
#ifdef USE_ANGLE_CONTROL
      // 現在の姿勢を目標値として設定
      float currentPitch = autoControl.getCurrentPitch();
      float currentRoll = autoControl.getCurrentRoll();
      float currentYaw = autoControl.getCurrentYaw();
      
      autoControl.setTargets(currentPitch, currentRoll, currentYaw);
      Serial.println("Auto Control ON - Holding current attitude:");
      Serial.print("Target Pitch: "); Serial.println(currentPitch, 2);
      Serial.print("Target Roll: "); Serial.println(currentRoll, 2);
      Serial.print("Target Yaw: "); Serial.println(currentYaw, 2);
#endif
#ifdef USE_ACCEL_CONTROL
      // 現在の加速度を目標値として設定
      float currentAccelX = autoControl.getCurrentAccelX();
      float currentAccelY = autoControl.getCurrentAccelY();
      float currentAccelZ = autoControl.getCurrentAccelZ();
      
      autoControl.setAccelTargets(currentAccelX, currentAccelY, currentAccelZ);
      Serial.println("Auto Control ON - Holding current acceleration:");
      Serial.print("Target AccelX: "); Serial.println(currentAccelX, 3);
      Serial.print("Target AccelY: "); Serial.println(currentAccelY, 3);
      Serial.print("Target AccelZ: "); Serial.println(currentAccelZ, 3);
#endif
    }
    
    // RC受信機からの目標値を取得（微調整用）
    float elevatorInput = rcReceiver.getElevatorValue();
    float rudderInput = rcReceiver.getRudderValue();
    
#ifdef USE_ANGLE_CONTROL
    // RC入力による目標角度の微調整（現在の目標値からのオフセット）
    static float basePitchTarget = 0;
    static float baseYawTarget = 0;
    
    // モード切り替わり時にベース目標値を更新
    if (modeChanged && previousPassthroughMode) {
      basePitchTarget = autoControl.getCurrentPitch();
      baseYawTarget = autoControl.getCurrentYaw();
    }
    
    float pitchTarget = basePitchTarget + (elevatorInput * 0.05);  // ±5度程度の微調整
    float yawTarget = baseYawTarget + (rudderInput * 0.05);       // ±5度程度の微調整
    autoControl.setTargets(pitchTarget, 0, yawTarget);
#endif

#ifdef USE_ACCEL_CONTROL
    // RC入力による目標加速度の微調整
    static float baseAccelXTarget = 0;
    static float baseAccelZTarget = -1.0;  // 重力分
    
    // モード切り替わり時にベース目標値を更新
    if (modeChanged && previousPassthroughMode) {
      baseAccelXTarget = autoControl.getCurrentAccelX();
      baseAccelZTarget = autoControl.getCurrentAccelZ();
    }
    
    float accelXTarget = baseAccelXTarget + (elevatorInput * 0.01);  // 微調整
    float accelZTarget = baseAccelZTarget + (rudderInput * 0.01);    // 微調整
    autoControl.setAccelTargets(accelXTarget, 0, accelZTarget);
#endif
    
    // PID制御出力を取得
    float elevatorControl = autoControl.getElevatorOutput();
    float rudderControl = autoControl.getRudderOutput();
    
    // RC入力と制御出力を混合
    float elevatorOutput = elevatorInput + elevatorControl;
    float rudderOutput = rudderInput + rudderControl;
    
    // 出力制限
    elevatorOutput = constrain(elevatorOutput, -100, 100);
    rudderOutput = constrain(rudderOutput, -100, 100);
    
    // サーボに出力
    elevatorServo.writeValue(elevatorOutput);
    rudderServo.writeValue(rudderOutput);
    
    // デバッグ出力（1秒毎）
    if (millis() - lastDebugTime > 1000) {
      autoControl.printDebugInfo();
      Serial.print("Control: E=");
      Serial.print(elevatorOutput, 1);
      Serial.print(" R=");
      Serial.println(rudderOutput, 1);
      lastDebugTime = millis();
    }
    delay(10);  // 100Hz制御ループ
  } else {
    // パススルーモード
    
    // RC受信機からの入力をそのまま出力
    float elevatorInput = rcReceiver.getElevatorValue();
    float rudderInput = rcReceiver.getRudderValue();
    
    elevatorServo.writeValue(elevatorInput);
    rudderServo.writeValue(rudderInput);
    
    // 制御システムをリセット
    if (mpu6050Available) {
      autoControl.reset();
    }
  }
  
  // 前回のモード状態を更新
  previousPassthroughMode = isPassthrough;
}
