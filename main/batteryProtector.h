#ifndef batteryProtector_h
#define batteryProtector_h

#include "Arduino.h"
#include "basicHardware.h"

//////////////////////////////////////////////////////////
// BATTERY PROTECTOR
//////////////////////////////////////////////////////////
class BatteryProtector {
  public:
    BatteryProtector(
      float voltageCutoffThreshold = 10.5f,  // Voltage threshold in Volts (cutoff when battery voltage drops below this)
      Display* display = nullptr  // Optional LCD display for status output
    );
    
    void update(); // Call in loop()
    void rearm();  // Manually rearm the circuit (close relay and resume monitoring)
    void printStatus(); // Print current status to Serial
    void updateDisplay(); // Update LCD display with current status
    
    enum State {
      STATE_ARMED,    // Relay closed, voltage above threshold
      STATE_CUTOFF    // Relay opened, voltage below threshold
    };
    
    State getState();
    float getBatteryVoltage();
    float getVoltageCutoffThreshold();
    
  private:
    VoltageSensor* _voltageSensor;
    Relay* _loadRelay;
    LED* _greenLED;
    LED* _redLED;
    Switch* _testButton;
    Display* _display;
    
    // Pin definitions
    static const uint8_t PIN_VOLTAGE_SENSOR = A0;  // A0 analog pin for voltage divider
    static const uint8_t PIN_RELAY_CONTROL = 12;    // D6/GPIO12
    static const uint8_t PIN_GREEN_LED = 2;         // D4/GPIO2
    static const uint8_t PIN_RED_LED = 14;         // D5/GPIO14
    static const uint8_t PIN_TEST_BUTTON = 0;      // D3/GPIO0
    
    float _voltageCutoffThreshold;
    
    State _state;
    float _lastVoltage;
    unsigned long _lastRearmAttemptMs;
    unsigned long _rearmCountdownStartMs; // When the rearm countdown started
    bool _isWaitingForRearm; // True when voltage is good but waiting for rearm interval
    
    const unsigned long _rearmIntervalMs = 60000; // Auto-rearm interval: 1 minute (60000 ms)
    const unsigned long _updateIntervalMs = 1000; // Update interval: 1 second (1000 ms)
    unsigned long _lastUpdateTimeMs;
    unsigned long _lastLEDUpdateMs; // For LED blinking during countdown
    unsigned long _lastDisplayUpdateMs; // For display updates during countdown
    
    void _handleTestButton();
    void _updateState();
    void _updateLEDs();
    bool _shouldCutoff();
    void _performCutoff();
    void _attemptRearm();
};
//////////////////////////////////////////////////////////

#endif