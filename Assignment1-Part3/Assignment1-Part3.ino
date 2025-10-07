#include <Wire.h>
#include <Adafruit_TCS34725.h>

// Initialize RGB sensor with shorter integration time
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_154MS, TCS34725_GAIN_4X);

// LED pin definition
const int ONBOARD_LED = 2;  // ESP32 onboard LED

// Flash threshold
const float FLASH_THRESHOLD = 150.0;  // Threshold for onboard LED flashing

// Timing variables
const int SAMPLE_INTERVAL = 1000;     // Read sensor data every 1 second
const int SLIDING_WINDOW_SIZE = 3;    // 3 seconds sliding window
unsigned long lastSampleTime = 0;

// Sliding window data storage
float lightIntensityWindow[SLIDING_WINDOW_SIZE];
int windowIndex = 0;
bool windowFilled = false;

// Flashing variables
unsigned long lastFlashTime = 0;
bool onboardLedState = false;
int flashInterval = 1000;  // Default flash interval (ms)

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  Serial.println("ESP32 RGB Light Sensor - Onboard LED Flashing System");
  Serial.println("====================================================");
  
  // Initialize onboard LED pin
  pinMode(ONBOARD_LED, OUTPUT);
  digitalWrite(ONBOARD_LED, LOW);  // Turn off LED initially
  
  // Initialize RGB sensor
  if (tcs.begin()) {
    Serial.println("RGB sensor found and initialized successfully!");
  } else {
    Serial.println("ERROR: RGB sensor not found. Please check wiring!");
    while(1);  // Stop execution if sensor not found
  }
  
  // Initialize sliding window array
  for(int i = 0; i < SLIDING_WINDOW_SIZE; i++) {
    lightIntensityWindow[i] = 0.0;
  }
  
  // Display threshold settings
  Serial.print("Flash threshold for onboard LED: ");
  Serial.println(FLASH_THRESHOLD);
  Serial.println();
  
  Serial.println("Starting light intensity detection...");
  Serial.println("Format: R: xxx, G: xxx, B: xxx, Clear: xxx, Intensity: xxx.xx, Sliding Avg: xxx.xx, Flash: xxx ms");
  Serial.println();
}

void loop() {
  unsigned long currentTime = millis();
  
  // Read sensor data every 1 second
  if (currentTime - lastSampleTime >= SAMPLE_INTERVAL) {
    readSensorAndUpdateSystem();
    lastSampleTime = currentTime;
  }
  
  // Handle onboard LED flashing
  handleOnboardLEDFlashing(currentTime);
}

void readSensorAndUpdateSystem() {
  uint16_t r, g, b, c;
  
  // Read raw RGB and Clear channel data from sensor
  tcs.getRawData(&r, &g, &b, &c);
  
  // Calculate light intensity - using RGB average
  float lightIntensity = (float)(r + g + b) / 3.0;
  
  // Store in sliding window
  lightIntensityWindow[windowIndex] = lightIntensity;
  windowIndex = (windowIndex + 1) % SLIDING_WINDOW_SIZE;
  
  // Check if window is filled at least once
  if (windowIndex == 0) {
    windowFilled = true;
  }
  
  // Calculate sliding window average
  float slidingAverage = calculateSlidingAverage();
  
  // Update flash interval based on sliding window average
  updateFlashInterval(slidingAverage);
  
  // Display sensor data and system status
  Serial.print("R: ");
  Serial.print(r);
  Serial.print(", G: ");
  Serial.print(g);
  Serial.print(", B: ");
  Serial.print(b);
  Serial.print(", Clear: ");
  Serial.print(c);
  Serial.print(", Intensity: ");
  Serial.print(lightIntensity, 2);
  Serial.print(", Sliding Avg: ");
  Serial.print(slidingAverage, 2);
  Serial.print(", Flash: ");
  if (slidingAverage > FLASH_THRESHOLD) {
    Serial.print(flashInterval);
    Serial.println(" ms");
  } else {
    Serial.println("OFF");
  }
}

float calculateSlidingAverage() {
  float sum = 0.0;
  int count = windowFilled ? SLIDING_WINDOW_SIZE : windowIndex;
  
  if (count == 0) return 0.0;
  
  for (int i = 0; i < count; i++) {
    sum += lightIntensityWindow[i];
  }
  
  return sum / count;
}

void updateFlashInterval(float slidingAverage) {
  if (slidingAverage <= FLASH_THRESHOLD) {
    // Below threshold - no flashing
    flashInterval = 0;
    digitalWrite(ONBOARD_LED, LOW);
    onboardLedState = false;
  } else {
    // Above threshold - calculate flash speed based on intensity
    // Higher intensity = faster flashing
    // Map intensity range to flash interval (50ms to 1000ms)
    float intensityRatio = (slidingAverage - FLASH_THRESHOLD) / 500.0; // Normalize
    intensityRatio = constrain(intensityRatio, 0.0, 1.0); // Limit to 0-1
    
    // Invert ratio so higher intensity = lower interval (faster flash)
    flashInterval = 1000 - (int)(intensityRatio * 950); // Range: 50ms to 1000ms
    flashInterval = constrain(flashInterval, 50, 1000); // Ensure reasonable limits
  }
}

void handleOnboardLEDFlashing(unsigned long currentTime) {
  if (flashInterval > 0) {
    // Flash the onboard LED
    if (currentTime - lastFlashTime >= flashInterval) {
      onboardLedState = !onboardLedState;
      digitalWrite(ONBOARD_LED, onboardLedState ? HIGH : LOW);
      lastFlashTime = currentTime;
    }
  }
}