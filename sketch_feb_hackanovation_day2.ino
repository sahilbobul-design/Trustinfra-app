#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// PIN DEFINITIONS (ESP32-WROVER Safe)
#define TRIG_PIN    5    // GPIO5 - Ultrasonic Trigger
#define ECHO_PIN    18   // GPIO18 - Ultrasonic Echo  
#define MOISTURE_PIN 36  // GPIO36 (VP) - ADC1_CH0, soil moisture analog
#define SDA_PIN     21   // I2C SDA
#define SCL_PIN     22   // I2C SCL

// OLED Display
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// MEASUREMENT PARAMETERS - CALIBRATE THESE FOR YOUR SAMPLES!
const float PATH_LENGTH_CM = 20.0;        // Concrete test path length (measure exactly!)
const float GOOD_VELOCITY_MIN = 3500;     // m/s threshold (good concrete)
const float MOISTURE_MAX_GOOD = 25.0;     // % max moisture for good mix
const int NUM_READINGS = 5;               // Buffer size for averaging

// Buffers for averaging
float velocityBuffer[NUM_READINGS];
float moistureBuffer[NUM_READINGS];
int bufferIndex = 0;
unsigned long lastMeasurement = 0;

// EMA smoothing variables
float lastVelocity = 3500.0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // Pin initialization
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  // I2C pins
  Wire.begin(SDA_PIN, SCL_PIN);
  
  // OLED initialization
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED init failed!");
    while (1);
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Concrete Quality");
  display.println("Monitor v2.0");
  display.printf("Path: %.0fcm\n", PATH_LENGTH_CM);
  display.printf("Calibrating...\n");
  display.display();
  delay(2000);
  
  Serial.println("=== Concrete Quality Monitor v2.0 Started ===");
  Serial.printf("Path Length: %.1f cm\n", PATH_LENGTH_CM);
  Serial.printf("Good Velocity: >%.0f m/s\n", GOOD_VELOCITY_MIN);
  Serial.printf("Max Moisture: <%.1f%%\n", MOISTURE_MAX_GOOD);
  Serial.println("Enhanced noise filtering: Median + EMA\n");
}

void loop() {
  if (millis() - lastMeasurement > 2000) {  // Measure every 2 seconds
    takeMeasurement();
    lastMeasurement = millis();
  }
  
  updateDisplay();
  delay(100);
}

void takeMeasurement() {
  // Ultrasonic Pulse Velocity with advanced noise filtering
  float velocity = readUltrasonicVelocity();
  velocityBuffer[bufferIndex] = velocity;
  
  // Moisture content
  float moisture = readMoisture();
  moistureBuffer[bufferIndex] = moisture;
  
  bufferIndex = (bufferIndex + 1) % NUM_READINGS;
  
  // Calculate averages
  float avgVelocity = calculateAverage(velocityBuffer, NUM_READINGS);
  float avgMoisture = calculateAverage(moistureBuffer, NUM_READINGS);
  
  // Status determination
  bool velocityGood = (avgVelocity >= GOOD_VELOCITY_MIN);
  bool moistureGood = (avgMoisture <= MOISTURE_MAX_GOOD);
  bool overallGood = velocityGood && moistureGood;
  
  // Detailed serial logging
  Serial.printf("[T+%.1fs] VEL: %.0f m/s (%.2f km/s) %s\n", 
                millis()/1000.0, avgVelocity, avgVelocity/1000.0,
                velocityGood ? "✓ GOOD" : "✗ LOW");
  Serial.printf("[T+%.1fs] MOIST: %.1f%% %s\n", 
                millis()/1000.0, avgMoisture,
                moistureGood ? "✓ OK" : "✗ HIGH");
  Serial.printf("[T+%.1fs] STATUS: %s\n\n", 
                millis()/1000.0, overallGood ? "✓ GOOD CONCRETE" : "✗ DEFECT DETECTED");
}

float readUltrasonicVelocity() {
  const int FILTER_SIZE = 5;
  long readings[FILTER_SIZE];
  
  // Take 5 rapid readings for median filtering
  for (int i = 0; i < FILTER_SIZE; i++) {
    // Clean trigger pulse
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    
    // Read echo with timeout
    long duration = pulseIn(ECHO_PIN, HIGH, 30000);
    
    // Range rejection (reject impossible readings)
    if (duration > 100 && duration < 25000) {
      readings[i] = duration;
    } else {
      readings[i] = 0;  // Mark as invalid
    }
    delay(60);  // Prevent interference
  }
  
  // Median filter - sort and take middle value
  for (int i = 0; i < FILTER_SIZE - 1; i++) {
    for (int j = 0; j < FILTER_SIZE - i - 1; j++) {
      if (readings[j] > readings[j + 1]) {
        long temp = readings[j];
        readings[j] = readings[j + 1];
        readings[j + 1] = temp;
      }
    }
  }
  
  long medianDuration = readings[FILTER_SIZE / 2];
  if (medianDuration == 0) return lastVelocity;  // Use last good reading
  
  // Calculate velocity: distance / time
  float duration_ms = medianDuration / 1000.0;
  float rawVelocity = (PATH_LENGTH_CM / 100.0) / (duration_ms / 1000.0);
  
  // Constrain to realistic range
  rawVelocity = constrain(rawVelocity, 2000, 5000);
  
  // Exponential Moving Average (EMA) for smooth trends
  float filteredVelocity = 0.7 * lastVelocity + 0.3 * rawVelocity;
  lastVelocity = filteredVelocity;
  
  return filteredVelocity;
}

float readMoisture() {
  long totalADC = 0;
  
  // 10x averaging for stable moisture reading
  for (int i = 0; i < 10; i++) {
    totalADC += analogRead(MOISTURE_PIN);
    delay(10);
  }
  
  float avgADC = totalADC / 10.0;
  float moisturePercent = 100.0 - (avgADC / 4095.0 * 100.0);
  
  // Clamp to valid range
  return constrain(moisturePercent, 0, 100);
}

float calculateAverage(float* buffer, int size) {
  float sum = 0;
  int validCount = 0;
  
  for (int i = 0; i < size; i++) {
    if (buffer[i] > 0) {
      sum += buffer[i];
      validCount++;
    }
  }
  
  return validCount > 0 ? sum / validCount : 0;
}

void updateDisplay() {
  // Current averages for display
  float avgVel = calculateAverage(velocityBuffer, NUM_READINGS);
  float avgMoist = calculateAverage(moistureBuffer, NUM_READINGS);
  
  bool velGood = avgVel >= GOOD_VELOCITY_MIN;
  bool moistGood = avgMoist <= MOISTURE_MAX_GOOD;
  
  display.clearDisplay();
  display.setCursor(0, 0);
  
  // Header
  display.setTextSize(1);
  display.println("CONCRETE MONITOR");
  
  // Big measurement numbers
  display.setTextSize(2);
  display.printf("V:%.1f\n", avgVel/1000);  // km/s
  display.printf("M:%.0f\n", avgMoist);     // %
  
  // Status indicators
  display.setTextSize(1);
  display.setCursor(0, 32);
  display.print(velGood ? "VEL:GOOD " : "VEL:POOR");
  display.setCursor(64, 32);
  display.print(moistGood ? "MOIST:OK" : "MOIST:HIGH");
  
  // Overall verdict
  display.setTextSize(2);
  display.setCursor(0, 48);
  if (velGood && moistGood) {
    display.print("GOOD");
  } else {
    display.print("ALERT");
  }
  
  display.display();
}