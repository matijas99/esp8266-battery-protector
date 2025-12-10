#include "batteryProtector.h"
#include "basicHardware.h"
#include <Wire.h>

BatteryProtector* batteryProtector;
Display* display;

// Voltage configuration
#define VOLTAGE_CUTOFF_THRESHOLD 11.0f  // Cutoff threshold in Volts (battery voltage below this will trigger cutoff)
#define VOLTAGE_REARM_THRESHOLD 12.8f  // Rearm threshold in Volts (battery voltage must rise above this to trigger rearm)

// Timing configuration
#define REARM_DELAY_SECONDS 60  // Delay in seconds before rearming after voltage exceeds rearm threshold

void setup() {
  Serial.begin(115200);
  delay(100); // Wait for Serial to initialize
  
  Wire.begin(); // Initialize I²C bus for LCD
  delay(100); // Wait for I²C bus to stabilize
  Wire.setClock(100000); // Set I²C clock speed to 100kHz (slower for reliability)
  
  // Initialize LCD display (I²C address 0x27)
  display = new Display(0x27, 16, 2);
  
  // Initialize Battery Protector with voltage thresholds, rearm delay, and display
  batteryProtector = new BatteryProtector(
    VOLTAGE_CUTOFF_THRESHOLD,
    VOLTAGE_REARM_THRESHOLD,
    REARM_DELAY_SECONDS * 1000UL,  // Convert seconds to milliseconds
    display
  );
}

void loop() {
  // Update battery protector (handles voltage monitoring, state management, and display updates)
  batteryProtector->update();
  
  delay(500); // 500ms delay to prevent tight loop
}
