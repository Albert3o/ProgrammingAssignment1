# ProgramingAssignment1

### Description:

**Part 1 (60 points)**

- Connect the *RGB Light Sensor* on ESP32
- Read sensor values once every second
- Display the values on the IDE console window
- Turn the onboard LED ON if the average value in a window of X (e.g. 2) seconds is higher than a threshold of Y (e.g. 150 - or pick another value you can justify/document)

**Part 2 (20 points)**

- Connect 3 *external LEDs* to ESP32
- Display the intensity of light from the RGB Sensor on 3 external LEDs using various patterns (e.g. low -> green LED on, medium -> yellow LED on, high -> red LED on)
  - Document your value thresholds and justify

**Part 3 (20 points)**

- Flash (instead of just ON) *onboard LED* if the average value in a sliding window of X (e.g. 2) seconds is higher than a threshold of Y (e.g. 150 or the value you picked earlier)
- The higher the readings, the faster it flashes
