#include "Arduino.h"
#include "batteryProtector.h"

//////////////////////////////////////////////////////////
// BATTERY PROTECTOR
//////////////////////////////////////////////////////////
BatteryProtector :: BatteryProtector(
  float voltageCutoffThreshold,
  Display* display
) {
  Serial.println("Initializing Battery Protector...");
  
  _voltageCutoffThreshold = voltageCutoffThreshold;
  _display = display;
  
  // Initialize hardware components
  // VoltageSensor with resistor values: R1=100kΩ, R2=430kΩ (100k+330k in series)
  // Calibration factor 1.20 compensates for WeMos D1 Mini internal voltage divider (220k/100k)
  _voltageSensor = new VoltageSensor(new PinNative(PIN_VOLTAGE_SENSOR), 100000.0f, 430000.0f, 1.20f);
  _loadRelay = new Relay(new PinNative(PIN_RELAY_CONTROL));
  _greenLED = new LED(new PinNative(PIN_GREEN_LED));
  _redLED = new LED(new PinNative(PIN_RED_LED));
  _testButton = new Switch(new PinNative(PIN_TEST_BUTTON));
  
  // Initialize voltage sensor
  if (!_voltageSensor->init()) {
    Serial.println("ERROR: Failed to initialize voltage sensor!");
  } else {
    Serial.println("Voltage sensor initialized successfully.");
  }
  
  // Initialize display if provided
  if (_display) {
    _display->init();
    delay(100);
    _display->backlight();
    delay(50);
    _display->clear();
    delay(50);
  }
  
  _lastVoltage = 0.0;
  _lastRearmAttemptMs = 0;
  _lastUpdateTimeMs = millis();
  _lastLEDUpdateMs = millis();
  _lastDisplayUpdateMs = millis();
  _rearmCountdownStartMs = 0;
  _isWaitingForRearm = false;
  
  // Read initial voltage
  _lastVoltage = _voltageSensor->readVoltageInVolts();
  
  // Check if voltage is already below threshold on startup
  if (_lastVoltage < _voltageCutoffThreshold) {
    Serial.print("Battery voltage (");
    Serial.print(_lastVoltage, 2);
    Serial.print("V) is below cutoff threshold (");
    Serial.print(_voltageCutoffThreshold, 2);
    Serial.println("V). Cutting off immediately.");
    _state = STATE_CUTOFF;
    _loadRelay->turnOff();
    _greenLED->off();
    _redLED->on();
  } else {
    Serial.print("Battery voltage: ");
    Serial.print(_lastVoltage, 2);
    Serial.println("V - Above threshold, circuit armed.");
    _state = STATE_ARMED;
    _loadRelay->turnOn();
    _greenLED->on();
    _redLED->off();
  }
  
  // Update display with initial state
  updateDisplay();
  
  Serial.println("Battery Protector ready!");
}

void BatteryProtector :: update() {
  unsigned long currentTime = millis();
  
  // Update voltage reading periodically
  if (currentTime - _lastUpdateTimeMs >= _updateIntervalMs) {
    _lastVoltage = _voltageSensor->readVoltageInVolts();
    _lastUpdateTimeMs = currentTime;
    updateDisplay(); // Update display when voltage updates
  }
  
  // Update display every second during countdown for smooth countdown display
  if (_isWaitingForRearm && (currentTime - _lastDisplayUpdateMs >= 1000)) {
    updateDisplay();
    _lastDisplayUpdateMs = currentTime;
  }
  
  // Handle test button
  _handleTestButton();
  
  // Update state based on voltage
  _updateState();
  
  // Update LEDs (called every loop iteration for blinking)
  _updateLEDs();
}

void BatteryProtector :: rearm() {
  // Manually rearm the circuit
  Serial.println("Manually rearming circuit...");
  _state = STATE_ARMED;
  _isWaitingForRearm = false;
  _rearmCountdownStartMs = 0;
  _loadRelay->turnOn();
  _lastRearmAttemptMs = millis();
  _greenLED->on();
  _redLED->off();
  
  // Update display immediately
  updateDisplay();
  
  Serial.println("Circuit rearmed.");
}

BatteryProtector::State BatteryProtector :: getState() {
  return _state;
}

float BatteryProtector :: getBatteryVoltage() {
  return _lastVoltage;
}

float BatteryProtector :: getVoltageCutoffThreshold() {
  return _voltageCutoffThreshold;
}

void BatteryProtector :: _handleTestButton() {
  if (_testButton->isPressed()) {
    delay(50); // Debounce
    if (_testButton->isPressed()) {
      if (_state == STATE_ARMED) {
        // Simulate voltage drop - trigger cutoff
        Serial.println("Test button: Simulating voltage drop below threshold");
        _performCutoff();
      } else if (_state == STATE_CUTOFF) {
        // Immediately rearm (skip waiting for 1-minute interval)
        Serial.println("Test button: Immediately rearming circuit");
        rearm();
      }
      // Wait for button release
      while (_testButton->isPressed()) {
        delay(10);
      }
    }
  }
}

void BatteryProtector :: _updateState() {
  switch (_state) {
    case STATE_ARMED:
      // Check if voltage dropped below threshold
      if (_shouldCutoff()) {
        _performCutoff();
      }
      break;
      
    case STATE_CUTOFF:
      // Check if voltage is good but we're waiting for rearm interval
      if (!_shouldCutoff() && !_isWaitingForRearm) {
        // Voltage is good, start countdown
        _isWaitingForRearm = true;
        _rearmCountdownStartMs = millis();
        Serial.println("Voltage is above threshold. Starting rearm countdown...");
      } else if (_shouldCutoff() && _isWaitingForRearm) {
        // Voltage dropped again during countdown, stop waiting
        _isWaitingForRearm = false;
        _rearmCountdownStartMs = 0;
        Serial.println("Voltage dropped below threshold during countdown. Cancelling rearm.");
        updateDisplay();
      } else if (_isWaitingForRearm) {
        // Check if countdown is complete
        _attemptRearm();
      } else {
        // Voltage still below threshold, attempt rearm periodically
        _attemptRearm();
      }
      break;
  }
}

void BatteryProtector :: _updateLEDs() {
  unsigned long currentTime = millis();
  
  switch (_state) {
    case STATE_ARMED:
      // Green LED solid ON (voltage above threshold, relay closed)
      _greenLED->on();
      _redLED->off();
      break;
      
    case STATE_CUTOFF:
      if (_isWaitingForRearm) {
        // Flash green LED while keeping red LED on during countdown
        // Blink every 500ms
        if (currentTime - _lastLEDUpdateMs >= 500) {
          _greenLED->toggle();
          _lastLEDUpdateMs = currentTime;
        }
        _redLED->on(); // Keep red LED on
      } else {
        // Red LED solid ON (voltage below threshold, relay opened)
        _greenLED->off();
        _redLED->on();
      }
      break;
  }
}

bool BatteryProtector :: _shouldCutoff() {
  // Cut off if voltage drops below threshold
  return _lastVoltage < _voltageCutoffThreshold;
}

void BatteryProtector :: _performCutoff() {
  _state = STATE_CUTOFF;
  _isWaitingForRearm = false; // Reset countdown state
  _rearmCountdownStartMs = 0;
  _loadRelay->turnOff();
  _greenLED->off();
  _redLED->on();
  
  // Update display immediately
  updateDisplay();
  
  Serial.print("CUTOFF: Battery voltage (");
  Serial.print(_lastVoltage, 2);
  Serial.print("V) dropped below threshold (");
  Serial.print(_voltageCutoffThreshold, 2);
  Serial.println("V). Relay opened.");
}

void BatteryProtector :: _attemptRearm() {
  unsigned long currentTime = millis();
  
  if (_isWaitingForRearm) {
    // Check if countdown is complete
    unsigned long elapsedMs = currentTime - _rearmCountdownStartMs;
    if (elapsedMs >= _rearmIntervalMs) {
      Serial.println("Attempting to rearm circuit...");
      
      // Close relay and check voltage
      _loadRelay->turnOn();
      delay(100); // Brief delay to allow voltage reading
      
      // Read voltage again
      float voltage = _voltageSensor->readVoltageInVolts();
      _lastVoltage = voltage;
      
      if (voltage >= _voltageCutoffThreshold) {
        // Voltage is above threshold, rearm successful
        _state = STATE_ARMED;
        _isWaitingForRearm = false;
        _rearmCountdownStartMs = 0;
        _greenLED->on();
        _redLED->off();
        updateDisplay(); // Update display immediately
        Serial.print("Rearm successful: Voltage (");
        Serial.print(voltage, 2);
        Serial.println("V) is above threshold.");
      } else {
        // Voltage dropped below threshold, reopen relay and reset countdown
        _loadRelay->turnOff();
        _isWaitingForRearm = false;
        _rearmCountdownStartMs = 0;
        updateDisplay(); // Update display immediately
        Serial.print("Rearm failed: Voltage (");
        Serial.print(voltage, 2);
        Serial.println("V) dropped below threshold. Relay reopened.");
      }
      
      _lastRearmAttemptMs = currentTime;
    }
  } else {
    // Check if it's time to attempt rearming (every minute) - old behavior for voltage still below threshold
    if (currentTime - _lastRearmAttemptMs >= _rearmIntervalMs) {
      Serial.println("Attempting to rearm circuit...");
      
      // Close relay and check voltage
      _loadRelay->turnOn();
      delay(100); // Brief delay to allow voltage reading
      
      // Read voltage again
      float voltage = _voltageSensor->readVoltageInVolts();
      _lastVoltage = voltage;
      
      if (voltage >= _voltageCutoffThreshold) {
        // Voltage is above threshold, rearm successful
        _state = STATE_ARMED;
        _greenLED->on();
        _redLED->off();
        updateDisplay(); // Update display immediately
        Serial.print("Rearm successful: Voltage (");
        Serial.print(voltage, 2);
        Serial.println("V) is above threshold.");
      } else {
        // Voltage still below threshold, reopen relay
        _loadRelay->turnOff();
        updateDisplay(); // Update display immediately
        Serial.print("Rearm failed: Voltage (");
        Serial.print(voltage, 2);
        Serial.println("V) is still below threshold. Relay reopened.");
      }
      
      _lastRearmAttemptMs = currentTime;
    }
  }
}

void BatteryProtector :: printStatus() {
  State state = getState();
  float voltage = getBatteryVoltage();
  
  Serial.print("State: ");
  switch (state) {
    case STATE_ARMED:
      Serial.print("ARMED");
      break;
    case STATE_CUTOFF:
      Serial.print("CUTOFF");
      break;
  }
  Serial.print(" | Voltage: ");
  Serial.print(voltage, 2);
  Serial.print("V | Threshold: ");
  Serial.print(_voltageCutoffThreshold, 2);
  Serial.println("V");
}

void BatteryProtector :: updateDisplay() {
  if (!_display) {
    return;
  }
  
  float voltage = getBatteryVoltage();
  State state = getState();
  
  // Top row: Battery voltage
  _display->setCursor(0, 0);
  _display->print("Battery: ");
  _display->print(voltage, 2);
  _display->print("V");
  // Clear rest of line
  _display->print("      ");
  
  // Bottom row: Relay state or countdown
  _display->setCursor(0, 1);
  if (_isWaitingForRearm && state == STATE_CUTOFF) {
    // Show countdown
    unsigned long currentTime = millis();
    unsigned long elapsedMs = currentTime - _rearmCountdownStartMs;
    unsigned long remainingMs = _rearmIntervalMs - elapsedMs;
    
    if (remainingMs > _rearmIntervalMs) {
      // Overflow protection
      remainingMs = 0;
    }
    
    unsigned long remainingSeconds = remainingMs / 1000;
    unsigned long minutes = remainingSeconds / 60;
    unsigned long seconds = remainingSeconds % 60;
    
    _display->print("Rearm in: ");
    if (minutes > 0) {
      _display->print((int)minutes);
      _display->print("m ");
    }
    _display->print((int)seconds);
    _display->print("s");
    // Clear rest of line
    _display->print("   ");
  } else {
    // Show relay state
    _display->print("Load relay: ");
    if (state == STATE_ARMED) {
      _display->print("ON ");
    } else {
      _display->print("OFF");
    }
    // Clear rest of line
    _display->print("   ");
  }
}
//////////////////////////////////////////////////////////
