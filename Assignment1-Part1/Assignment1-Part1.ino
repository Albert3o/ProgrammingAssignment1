#include <Wire.h>
#include <Adafruit_TCS34725.h>

// Initialize RGB sensor with shorter integration time
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_154MS, TCS34725_GAIN_4X);

// Pin definitions
const int LED_PIN = 2;  // ESP32 onboard LED pin

// Threshold and timing variables
const float THRESHOLD = 150.0;  // RGB average threshold
const int SAMPLE_INTERVAL = 1000;  // Read sensor every 1 second (1000ms)
const int CHECK_INTERVAL = 3000;   // Check threshold every 3 seconds (3000ms)

// Data storage for moving average
float rgbAverages[3];  // Store 3 seconds of RGB average data
int valueIndex = 0;    // Current index in the array
bool arrayFilled = false;  // Flag to check if array is full

// Timing variables
unsigned long lastSampleTime = 0;
unsigned long lastCheckTime = 0;

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  Serial.println("RGB Light Sensor with ESP32");
  Serial.println("============================");
  
  // Initialize LED pin
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);  // Turn off LED initially
  
  // Initialize RGB sensor
  if (tcs.begin()) {
    Serial.println("RGB sensor found and initialized successfully!");
  } else {
    Serial.println("ERROR: RGB sensor not found. Please check wiring!");
    while(1);  // Stop execution if sensor not found
  }
  
  // Initialize the RGB averages array
  for(int i = 0; i < 3; i++) {
    rgbAverages[i] = 0.0;
  }
  
  Serial.println("Starting data collection...");
  Serial.println("Format: R: xxx, G: xxx, B: xxx, Average: xxx.xx");
  Serial.println();
}

void loop() {
  unsigned long currentTime = millis();
  
  // Read sensor every 1 second
  if (currentTime - lastSampleTime >= SAMPLE_INTERVAL) {
    readAndStoreSensorData();
    lastSampleTime = currentTime;
  }
  
  // Check threshold every 3 seconds
  if (currentTime - lastCheckTime >= CHECK_INTERVAL) {
    checkThresholdAndControlLED();
    lastCheckTime = currentTime;
  }
}

void readAndStoreSensorData() {
  uint16_t r, g, b, c;
  
  // Read raw RGB data from sensor (we ignore the clear channel 'c')
  tcs.getRawData(&r, &g, &b, &c);
  
  // Calculate RGB average: (R + G + B) / 3
  float rgbAverage = (float)(r + g + b) / 3.0;
  
  // Store the RGB average in circular buffer
  rgbAverages[valueIndex] = rgbAverage;
  valueIndex = (valueIndex + 1) % 3;  // Move to next position (circular)
  
  // Check if we have filled the array at least once
  if (valueIndex == 0) {
    arrayFilled = true;
  }
  
  // Display current sensor readings (no clear channel)
  Serial.print("R: ");
  Serial.print(r);
  Serial.print(", G: ");
  Serial.print(g);
  Serial.print(", B: ");
  Serial.print(b);
  Serial.print(", Average: ");
  Serial.println(rgbAverage, 2);
}

float calculateMovingAverage() {
  float sum = 0.0;
  int count = arrayFilled ? 3 : valueIndex;  // Use available data points
  
  if (count == 0) return 0.0;
  
  for (int i = 0; i < count; i++) {
    sum += rgbAverages[i];
  }
  
  return sum / count;
}

void checkThresholdAndControlLED() {
  float movingAverage = calculateMovingAverage();
  
  Serial.println("--- THRESHOLD CHECK ---");
  Serial.print("3-second moving average of RGB: ");
  Serial.println(movingAverage, 2);
  Serial.print("Threshold value: ");
  Serial.println(THRESHOLD, 1);
  
  if (movingAverage > THRESHOLD) {
    digitalWrite(LED_PIN, HIGH);  // Turn ON LED
    Serial.println("✓ RGB Average > Threshold: LED turned ON");
  } else {
    digitalWrite(LED_PIN, LOW);   // Turn OFF LED
    Serial.println("✗ RGB Average ≤ Threshold: LED turned OFF");
  }
  
  Serial.println("=======================");
  Serial.println();
}