#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <MPU6050.h>

// Create separate I2C bus for MPU6050
TwoWire I2C_MPU = TwoWire(1);

// LCD on default I2C (GPIO 21/22)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// MPU6050 on second I2C bus (GPIO 25/26)
MPU6050 mpu6050(0x68, &I2C_MPU);

// DHT11
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Ultrasonic
#define TRIG_PIN 5
#define ECHO_PIN 18

// Soil Moisture
#define MOISTURE_PIN 34

// ============ HEALTH SCORE CALCULATION ============

float calculateHealthScore(float temp, float hum, float moisturePercent, float vibration) {
    // Define optimal ranges for healthy plant
    const float OPTIMAL_TEMP = 25.0;       // Optimal temperature in °C
    const float OPTIMAL_HUM = 60.0;        // Optimal humidity in %
    const float OPTIMAL_MOISTURE = 60.0;   // Optimal soil moisture in %
    
    // Weight factors (sum = 1.0)
    const float TEMP_WEIGHT = 0.25;
    const float HUM_WEIGHT = 0.25;
    const float MOISTURE_WEIGHT = 0.35;
    const float VIBRATION_WEIGHT = 0.15;
    
    // Calculate deviations from optimal values
    float tempDeviation = abs(temp - OPTIMAL_TEMP);
    float humDeviation = abs(hum - OPTIMAL_HUM);
    float moistureDeviation = abs(moisturePercent - OPTIMAL_MOISTURE);
    float vibrationScore = vibration * 100; // Convert to percentage scale
    
    // Normalize deviations to 0-1 scale with appropriate ranges
    float tempFactor = min(tempDeviation / 15.0, 1.0);         // 15°C deviation = 100% penalty
    float humFactor = min(humDeviation / 30.0, 1.0);           // 30% deviation = 100% penalty
    float moistureFactor = min(moistureDeviation / 40.0, 1.0); // 40% deviation = 100% penalty
    float vibrationFactor = min(vibrationScore / 50.0, 1.0);  // 50% vibration = 100% penalty
    
    // Calculate weighted health score (0-100)
    float healthScore = 100.0 - (
        (tempFactor * TEMP_WEIGHT + 
         humFactor * HUM_WEIGHT + 
         moistureFactor * MOISTURE_WEIGHT + 
         vibrationFactor * VIBRATION_WEIGHT) * 100.0
    );
    
    // Ensure score stays within bounds
    return constrain(healthScore, 0.0, 100.0);
}

// Get health status message
String getHealthStatus(float score) {
    if (score >= 90) return "Excellent";
    else if (score >= 70) return "Good";
    else if (score >= 50) return "Fair";
    else if (score >= 30) return "Poor";
    else return "Critical";
}

// =================================================

void setup() {
    Serial.begin(115200);
    Serial.println("Starting Plant Health Monitoring System...");

    // Initialize I2C for LCD (GPIO 21/22)
    Wire.begin(21, 22);
    delay(100);

    // Initialize LCD
    lcd.begin(16, 2);
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Plant Health");
    lcd.setCursor(0, 1);
    lcd.print("Monitoring System");
    Serial.println("LCD initialized");

    delay(1500);

    // Initialize I2C for MPU6050 (GPIO 25/26)
    I2C_MPU.begin(25, 26);
    delay(100);

    // Initialize DHT11
    dht.begin();
    Serial.println("DHT11 initialized");

    // Ultrasonic pins
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    // Soil moisture pin
    pinMode(MOISTURE_PIN, INPUT);

    // Initialize MPU6050
    Serial.println("Initializing MPU6050...");
    mpu6050.initialize();
    mpu6050.setFullScaleAccelRange(MPU6050_ACCEL_FS_2); // Set range to ±2g

    if (mpu6050.testConnection()) {
        Serial.println("MPU6050 OK");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("All Sensors OK");
    } else {
        Serial.println("MPU6050 FAIL");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("MPU6050 FAIL");
    }

    delay(1500);
    lcd.clear();
}

float readUltrasonicCM() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    long duration = pulseIn(ECHO_PIN, HIGH, 30000);
    float distance = duration * 0.034 / 2;

    // If no echo, return 0
    if (duration == 0) {
        return 0;
    }
    // Cap at reasonable distance
    if (distance > 400 || distance < 0) {
        return 0;
    }
    return distance;
}

float readVibration() {
    int16_t ax, ay, az;
    mpu6050.getAcceleration(&ax, &ay, &az);

    // Convert to g (16384 LSB/g at ±2g range)
    float ax_g = ax / 16384.0;
    float ay_g = ay / 16384.0;
    float az_g = az / 16384.0;

    // Calculate total acceleration magnitude
    float total = sqrt(ax_g * ax_g + ay_g * ay_g + az_g * az_g);

    // Vibration is deviation from 1g (gravity)
    float vibration = abs(total - 1.0);

    return vibration;
}

void loop() {
    // ========== Read All Sensors ==========
    
    // Read DHT11
    float temp = dht.readTemperature();
    float hum = dht.readHumidity();
    if (isnan(temp)) temp = 0;
    if (isnan(hum)) hum = 0;

    // Read Ultrasonic
    float distance = readUltrasonicCM();

    // Read Soil Moisture
    int moistureRaw = analogRead(MOISTURE_PIN);
    float moisturePercent = map(moistureRaw, 0, 4095, 100, 0);
    moisturePercent = constrain(moisturePercent, 0, 100);

    // Read Vibration
    float vibration = readVibration();

    // Calculate Health Score
    float healthScore = calculateHealthScore(temp, hum, moisturePercent, vibration);
    String healthStatus = getHealthStatus(healthScore);

    // ========== Serial Output ==========
    Serial.println("========================================");
    Serial.println("        CONSTRUCTION HEALTH MONITORING        ");
    Serial.println("========================================");
    Serial.print("Temperature: "); Serial.print(temp, 1); Serial.println(" C");
    Serial.print("Humidity:    "); Serial.print(hum, 0); Serial.println(" %");
    Serial.print("Distance:    "); Serial.print(distance, 1); Serial.println(" cm");
    Serial.print("Soil Raw:    "); Serial.print(moistureRaw);
    Serial.print(" | Soil %:   "); Serial.println(moisturePercent, 0);
    Serial.print("Vibration:  "); Serial.print(vibration, 4); Serial.println(" g");
    Serial.println("----------------------------------------");
    Serial.print("HEALTH SCORE: "); Serial.print(healthScore, 1);
    Serial.print(" / 100 - Status: ");
    Serial.println(healthStatus);
    Serial.println("========================================");

    // ========== LCD Display - Screen 1: Health Score (Priority) ==========
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Health Score:");
    lcd.setCursor(0, 1);
    lcd.print(healthScore, 0);
    lcd.print(" % - ");
    lcd.print(healthStatus);
    delay(3000);

    // ========== LCD Display - Screen 2: Temperature & Humidity ==========
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(temp, 1);
    lcd.print((char)223);
    lcd.print("C");
    lcd.setCursor(0, 1);
    lcd.print("Hum: ");
    lcd.print(hum, 0);
    lcd.print(" %");
    delay(2000);

    // ========== LCD Display - Screen 3: Soil Moisture ==========
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Soil Moisture:");
    lcd.setCursor(0, 1);
    lcd.print(moisturePercent, 0);
    lcd.print(" %");
    delay(2000);

    // ========== LCD Display - Screen 4: Distance ==========
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Distance:");
    lcd.setCursor(0, 1);
    lcd.print(distance, 1);
    lcd.print(" cm");
    delay(2000);

    // ========== LCD Display - Screen 5: Vibration ==========
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Vibration:");
    lcd.setCursor(0, 1);
    lcd.print(vibration, 4);
    lcd.print(" g");
    delay(2000);

    // ========== LCD Display - Screen 6: All Combined ==========
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("T:"); lcd.print(temp, 0); lcd.print((char)223);
    lcd.print(" H:"); lcd.print(hum, 0); lcd.print("%");
    lcd.setCursor(0, 1);
    lcd.print("S:"); lcd.print(moisturePercent, 0); lcd.print("%");
    lcd.print(" V:"); lcd.print(vibration * 100, 0);
    delay(2000);
}