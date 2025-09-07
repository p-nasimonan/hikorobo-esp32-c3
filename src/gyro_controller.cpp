#include "gyro_controller.h"

GyroController::GyroController(int sda_pin, int scl_pin, int switch_pin, SensorType sensor_type) : lsm6ds3trc(), mpu6050() {
    sdaPin = sda_pin;
    sclPin = scl_pin;
    switchPin = switch_pin;
    sensorType = sensor_type;
    imuSensor = nullptr;
    initialized = false;
    gyroEnabled = false;  // 初期状態はパススルー
    bufferIndex = 0;
    
    // 初期値設定
    gyroX = gyroY = gyroZ = 0.0;
    accelX = accelY = accelZ = 0.0;
    
    // バッファ初期化
    for (int i = 0; i < FILTER_SIZE; i++) {
        gyroXBuffer[i] = 0.0;
        gyroYBuffer[i] = 0.0;
        gyroZBuffer[i] = 0.0;
    }
}

void GyroController::begin() {
    Serial.println("IMUセンサー初期化中...");
    
    // スイッチピン初期化
    pinMode(switchPin, INPUT);
    Serial.println("切り替えスイッチ初期化完了");
    
    // センサー自動検出・初期化
    if (!detectAndInitializeSensor()) {
        Serial.println("IMUセンサー初期化失敗 - パススルーモード");
        initialized = false;
        return;
    }
    
    Serial.print("使用センサー: ");
    Serial.println(imuSensor->getSensorName());
    
    // I2C速度を400kHzに設定
    Wire.setClock(I2C_CLOCK);
    Serial.print("I2C速度: ");
    Serial.print(I2C_CLOCK);
    Serial.println("Hz");
    
    // キャリブレーション実行
    Serial.println("キャリブレーション開始...");
    calibrate(30);  // 30サンプルで高速化
    Serial.println("キャリブレーション完了");
    
    initialized = true;
}

void GyroController::update() {
    // スイッチ状態を更新
    updateSwitchState();
    
    if (!initialized || !gyroEnabled) {
        return;  // センサー未初期化またはパススルーモード
    }
    
    // センサーデータ読み取り
    imuSensor->readSensorData();
    gyroX = applyMovingAverage(gyroXBuffer, imuSensor->getGyroX());
    gyroY = applyMovingAverage(gyroYBuffer, imuSensor->getGyroY());
    gyroZ = applyMovingAverage(gyroZBuffer, imuSensor->getGyroZ());
    
    accelX = imuSensor->getAccelX();
    accelY = imuSensor->getAccelY();
    accelZ = imuSensor->getAccelZ();
    
    // ノイズ閾値以下の値はゼロに
    if (abs(gyroX) < GYRO_THRESHOLD) gyroX = 0.0;
    if (abs(gyroY) < GYRO_THRESHOLD) gyroY = 0.0;
    if (abs(gyroZ) < GYRO_THRESHOLD) gyroZ = 0.0;
    
    // デバッグ出力（10秒に1回、簡素化）
    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate > 10000) {
        Serial.print("モード: ");
        Serial.println(gyroEnabled ? "ジャイロ補正" : "パススルー");
        lastUpdate = millis();
    }
}

void GyroController::updateSwitchState() {
    // スイッチ信号読み取り（LEDと同じピン）
    unsigned long switchPulse = pulseIn(switchPin, HIGH, 25000);  // タイムアウト25ms
    
    static bool lastState = false;
    bool currentState = false;
    
    if (switchPulse > 0) {
        // PWM信号の中央値（1500μs）を基準にON/OFF判定
        currentState = (switchPulse >= RC_CENTER);
        
        // 状態が変化した場合のみ更新
        if (currentState != lastState) {
            gyroEnabled = currentState;
            Serial.print("切り替え: ");
            Serial.println(gyroEnabled ? "補正ON" : "パススルー");
            lastState = currentState;
        }
    } else {
        // 信号なしの場合はパススルーモード
        if (gyroEnabled) {
            gyroEnabled = false;
            Serial.println("信号なし - パススルー");
        }
        lastState = false;
    }
}

bool GyroController::isInitialized() {
    return initialized;
}

bool GyroController::isGyroEnabled() {
    return gyroEnabled;
}

float GyroController::getGyroX() {
    return gyroX;
}

float GyroController::getGyroY() {
    return gyroY;
}

float GyroController::getGyroZ() {
    return gyroZ;
}

float GyroController::getAccelX() {
    return accelX;
}

float GyroController::getAccelY() {
    return accelY;
}

float GyroController::getAccelZ() {
    return accelZ;
}

int GyroController::applyGyroCorrection(int originalAngle, String axis) {
    if (!initialized || !gyroEnabled) {
        return originalAngle;  // パススルー
    }
    
    float correction = 0.0;
    
    if (axis == "elevator") {
        // エレベーター軸のジャイロ補正（Y軸ピッチ）
        correction = -gyroY * CORRECTION_FACTOR;
    } else if (axis == "rudder") {
        // ラダー軸のジャイロ補正（Z軸ヨー）
        correction = -gyroZ * CORRECTION_FACTOR;
    }
    
    // 補正値を適用
    int correctedAngle = originalAngle + (int)correction;
    
    // 角度範囲制限（45°-135°）
    correctedAngle = constrain(correctedAngle, 45, 135);
    
    return correctedAngle;
}

void GyroController::calibrate(int samples) {
    if (!initialized) return;
    
    Serial.println("キャリブレーション中...");
    
    for (int i = 0; i < samples; i++) {
        // IMUセンサーからデータ読み取り
        imuSensor->readSensorData();
        float gx = imuSensor->getGyroX();
        float gy = imuSensor->getGyroY(); 
        float gz = imuSensor->getGyroZ();
        (void)gx; (void)gy; (void)gz;  // 未使用警告を抑制
        delay(10);
        
        if (i % 25 == 0) {
            Serial.print(".");
        }
    }
    
    Serial.println(" 完了");
}

float GyroController::applyMovingAverage(float* buffer, float newValue) {
    // 新しい値をバッファに追加
    buffer[bufferIndex] = newValue;
    bufferIndex = (bufferIndex + 1) % FILTER_SIZE;
    
    // 移動平均を計算
    float sum = 0.0;
    for (int i = 0; i < FILTER_SIZE; i++) {
        sum += buffer[i];
    }
    
    return sum / FILTER_SIZE;
}

bool GyroController::detectAndInitializeSensor() {
    // 指定されたセンサータイプを試す
    if (sensorType == SENSOR_LSM6DS3TRC) {
        Serial.println("LSM6DS3TRC指定 - 初期化中...");
        if (lsm6ds3trc.begin()) {
            imuSensor = &lsm6ds3trc;
            return true;
        }
        return false;
    }
    
    if (sensorType == SENSOR_MPU6050) {
        Serial.println("MPU6050指定 - 初期化中...");
        // MPU6050用にI2C速度を調整
        Wire.setClock(100000);  // 100kHz
        delay(10);
        if (mpu6050.begin()) {
            imuSensor = &mpu6050;
            return true;
        }
        return false;
    }
    
    // 自動検出モード
    Serial.println("センサー自動検出中...");
    
    // 最初にMPU6050を試す（より一般的）
    Serial.println("MPU6050を試行中...");
    // MPU6050用にI2C速度を調整
    Wire.setClock(100000);  // 100kHz
    delay(10);
    if (mpu6050.begin()) {
        imuSensor = &mpu6050;
        sensorType = SENSOR_MPU6050;
        return true;
    }
    
    // 次にLSM6DS3TRCを試す
    Serial.println("LSM6DS3TRCを試行中...");
    if (lsm6ds3trc.begin()) {
        imuSensor = &lsm6ds3trc;
        sensorType = SENSOR_LSM6DS3TRC;
        return true;
    }
    
    Serial.println("対応するセンサーが見つかりません");
    return false;
}

const char* GyroController::getSensorName() {
    if (imuSensor) {
        return imuSensor->getSensorName();
    }
    return "Unknown";
}

SensorType GyroController::getSensorType() {
    return sensorType;
}
