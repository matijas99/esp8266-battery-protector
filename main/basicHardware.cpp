#include "Arduino.h"
#include "LiquidCrystal_I2C.h"
#include "Wire.h"
#include "basicHardware.h"


//////////////////////////////////////////////////////////
// PIN NATIVE
//////////////////////////////////////////////////////////
PinNative :: PinNative(uint8_t pinAddress) {
    _pinAddress = pinAddress;
}
void PinNative :: setPinMode(uint8_t mode) {
  pinMode(_pinAddress, mode);
}
void PinNative :: doDigitalWrite(uint8_t val) {
  digitalWrite(_pinAddress, val);
}
int PinNative :: doDigitalRead() {
  return digitalRead(_pinAddress);
}

int PinNative :: doAnalogRead() {
  return analogRead(_pinAddress);
}
//////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////
// SWITCH
//////////////////////////////////////////////////////////
Switch :: Switch(Pin* pin) {

  _pin = pin;
  _pin->setPinMode(INPUT_PULLUP);
}

bool Switch :: isPressed() {
  return _pin->doDigitalRead() == LOW;
}
//////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////
// RELAY
//////////////////////////////////////////////////////////
Relay :: Relay(Pin* controlPin) {
  _controlPin = controlPin;
  _controlPin->setPinMode(OUTPUT);
  _controlPin->doDigitalWrite(LOW);
}

void Relay :: turnOn() {
  _controlPin->doDigitalWrite(HIGH);
}

void Relay :: turnOff() {
  _controlPin->doDigitalWrite(LOW);
}
//////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////
// LED
//////////////////////////////////////////////////////////
LED :: LED(Pin* pin) {
  _pin = pin;
  _pin->setPinMode(OUTPUT);
  _blinking = false;
  _blinkIntervalMs = 500;
  _lastBlinkTimeMs = 0;
  _currentState = false;
  off();
}

void LED :: on() {
  _blinking = false;
  _currentState = true;
  _pin->doDigitalWrite(HIGH);
}

void LED :: off() {
  _blinking = false;
  _currentState = false;
  _pin->doDigitalWrite(LOW);
}

void LED :: toggle() {
  _blinking = false;
  _currentState = !_currentState;
  _pin->doDigitalWrite(_currentState ? HIGH : LOW);
}

void LED :: blink(unsigned long intervalMs) {
  _blinking = true;
  _blinkIntervalMs = intervalMs;
  _lastBlinkTimeMs = millis();
}

void LED :: update() {
  if (_blinking) {
    unsigned long currentTime = millis();
    if (currentTime - _lastBlinkTimeMs >= _blinkIntervalMs) {
      _currentState = !_currentState;
      _pin->doDigitalWrite(_currentState ? HIGH : LOW);
      _lastBlinkTimeMs = currentTime;
    }
  }
}
//////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////
// VOLTAGE SENSOR (ADC with Voltage Divider)
//////////////////////////////////////////////////////////
const float VoltageSensor::ADC_REFERENCE_VOLTAGE = 3.3f; // WeMos D1 Mini A0 max input
const int VoltageSensor::ADC_RESOLUTION = 1023; // 10-bit ADC

// Constructor with divider ratio (no calibration)
VoltageSensor :: VoltageSensor(Pin* pin, float dividerRatio) {
  _pin = pin;
  _dividerRatio = dividerRatio;
  _calibrationFactor = 1.0f;
  _initialized = false;
}

// Constructor with divider ratio (with calibration)
VoltageSensor :: VoltageSensor(Pin* pin, float dividerRatio, float calibrationFactor) {
  _pin = pin;
  _dividerRatio = dividerRatio;
  _calibrationFactor = calibrationFactor;
  _initialized = false;
}

// Constructor with resistor values (calibration factor required to avoid ambiguity)
VoltageSensor :: VoltageSensor(Pin* pin, float rTopOhms, float rBottomOhms, float calibrationFactor) {
  _pin = pin;
  // Calculate divider ratio: rTopOhms / (rTopOhms + rBottomOhms)
  // Measures voltage drop across rTopOhms (top resistor connected to battery positive)
  _dividerRatio = rTopOhms / (rTopOhms + rBottomOhms);
  _calibrationFactor = calibrationFactor;
  _initialized = false;
}

bool VoltageSensor :: init() {
  // Set pin mode for analog input
  _pin->setPinMode(INPUT);
  _initialized = true;
  return true;
}

float VoltageSensor :: readVoltageInVolts() {
  if (!_initialized) {
    return 0.0;
  }
  
  // Read ADC value (0-1023)
  int adcValue = _pin->doAnalogRead();
  
  // Convert ADC reading to voltage at pin
  // ADC reading ranges from 0 to 1023, representing 0V to 3.3V
  float pinVoltage = (adcValue / (float)ADC_RESOLUTION) * ADC_REFERENCE_VOLTAGE;
  
  // Convert pin voltage back to battery voltage using divider ratio
  // Battery voltage = pin voltage / divider ratio
  float batteryVoltage = pinVoltage / _dividerRatio;
  
  // Apply calibration factor to compensate for internal voltage divider
  // (WeMos D1 Mini has internal 220k/100k divider that affects measurement)
  batteryVoltage *= _calibrationFactor;
  
  return batteryVoltage;
}
//////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////
// DISPLAY (I2C LCD)
//////////////////////////////////////////////////////////
Display :: Display(uint8_t i2cAddress, uint8_t columns, uint8_t rows) {
  _columns = columns;
  _rows = rows;
  _lcd = new LiquidCrystal_I2C(i2cAddress, columns, rows);
}

void Display :: init() {
  _lcd->init();
}

void Display :: backlight() {
  _lcd->backlight();
}

void Display :: noBacklight() {
  _lcd->noBacklight();
}

void Display :: clear() {
  _lcd->clear();
}

void Display :: setCursor(uint8_t col, uint8_t row) {
  _lcd->setCursor(col, row);
}

void Display :: print(const char* text) {
  _lcd->print(text);
}

void Display :: print(float value, int decimals) {
  _lcd->print(value, decimals);
}

void Display :: print(int value) {
  _lcd->print(value);
}

void Display :: print(unsigned long value) {
  _lcd->print(value);
}
//////////////////////////////////////////////////////////


