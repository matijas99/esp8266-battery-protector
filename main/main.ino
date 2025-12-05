#include "batteryProtector.h"
#include "basicHardware.h"
#include <Wire.h>

BatteryProtector* batteryProtector;
Display* display;

// Voltage cutoff configuration
#define VOLTAGE_CUTOFF_THRESHOLD 10.5f  // Cutoff threshold in Volts (battery voltage below this will trigger cutoff)

void setup() {
  Serial.begin(115200);
  delay(100); // Wait for Serial to initialize
  
  Wire.begin(); // Initialize I²C bus for LCD
  delay(100); // Wait for I²C bus to stabilize
  Wire.setClock(100000); // Set I²C clock speed to 100kHz (slower for reliability)
  
  // Initialize LCD display (I²C address 0x27)
  display = new Display(0x27, 16, 2);
  
  // Initialize Battery Protector with voltage threshold and display
  batteryProtector = new BatteryProtector(VOLTAGE_CUTOFF_THRESHOLD, display);
}

void loop() {
  // Update battery protector (handles voltage monitoring, state management, and display updates)
  batteryProtector->update();
  
  delay(500); // 500ms delay to prevent tight loop
}
