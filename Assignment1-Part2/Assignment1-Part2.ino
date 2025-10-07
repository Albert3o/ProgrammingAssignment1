#include <Wire.h>
#include <Adafruit_TCS34725.h>

// Initialize RGB sensor with shorter integration time
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_154MS, TCS34725_GAIN_4X);

// LED pin definitions
const int GREEN_LED = 12;   // Green LED - Low light intensity
const int YELLOW_LED = 13;  // Yellow LED - Medium light intensity
const int RED_LED = 14;     // Red LED - High light intensity

// Light intensity threshold definitions
const int LOW_THRESHOLD = 200;    // Low light intensity threshold
const int HIGH_THRESHOLD = 500;   // High light intensity threshold

// Timing variables
const int SAMPLE_INTERVAL = 1000;  // Read sensor data every 1 second
unsigned long lastSampleTime = 0;

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  Serial.println("ESP32 RGB Light Sensor - Three Color LED Indicator");
  Serial.println("==================================================");
  
  // Initialize LED pins
  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  
  // Turn off all LEDs initially
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED, LOW);
  
  // Initialize RGB sensor
  if (tcs.begin()) {
    Serial.println("RGB sensor found and initialized successfully!");
  } else {
    Serial.println("ERROR: RGB sensor not found. Please check wiring!");
    while(1);  // Stop execution if sensor not found
  }
  
  // Display threshold settings
  Serial.println("Light intensity threshold settings:");
  Serial.print("Low intensity (Green LED): 0 - ");
  Serial.println(LOW_THRESHOLD);
  Serial.print("Medium intensity (Yellow LED): ");
  Serial.print(LOW_THRESHOLD + 1);
  Serial.print(" - ");
  Serial.println(HIGH_THRESHOLD);
  Serial.print("High intensity (Red LED): ");
  Serial.print(HIGH_THRESHOLD + 1);
  Serial.println(" and above");
  Serial.println();
  
  Serial.println("Starting light intensity detection...");
  Serial.println("Format: R: xxx, G: xxx, B: xxx, Clear: xxx, Intensity: xxx.xx, LED Status: Color");
  Serial.println();
}

void loop() {
  unsigned long currentTime = millis();
  // Read sensor data and update LEDs every 1 second
  if (currentTime - lastSampleTime >= SAMPLE_INTERVAL) {
    readSensorAndUpdateLEDs();
    lastSampleTime = currentTime;
  }
}

void readSensorAndUpdateLEDs() {
  uint16_t r, g, b, c;
  tcs.getRawData(&r, &g, &b, &c);  // Read raw RGB and Clear channel data from sensor
  float lightIntensity = (float)(r + g + b) / 3.0;  // Calculate light intensity - using RGB average
  String ledStatus = updateLEDDisplay(lightIntensity);  // Update LED display based on light intensity
  // Display sensor data and LED status
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
  Serial.print(", LED Status: ");
  Serial.println(ledStatus);
}

String updateLEDDisplay(float intensity) {
  // Turn off all LEDs first
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED, LOW);

  String status = "";
  // Determine and light up corresponding LED based on intensity threshold
  if (intensity <= LOW_THRESHOLD) {
    // Low light intensity - Turn on Green LED
    digitalWrite(GREEN_LED, HIGH);
    status = "Green (Low intensity)";
  } 
  else if (intensity <= HIGH_THRESHOLD) {
    // Medium light intensity - Turn on Yellow LED
    digitalWrite(YELLOW_LED, HIGH);
    status = "Yellow (Medium intensity)";
  } 
  else {
    // High light intensity - Turn on Red LED
    digitalWrite(RED_LED, HIGH);
    status = "Red (High intensity)";
  }
  return status;
}