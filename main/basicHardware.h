#ifndef basicHardware_h
#define basicHardware_h

#include "Arduino.h"
#include "LiquidCrystal_I2C.h"

//////////////////////////////////////////////////////////
// PIN
//////////////////////////////////////////////////////////
class Pin {
  protected:
    uint8_t _pinAddress;

  public:
    virtual void setPinMode(uint8_t mode);
    virtual void doDigitalWrite(uint8_t val);
    virtual int doDigitalRead();
    virtual int doAnalogRead();
};
//////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////
// PIN NATIVE
//////////////////////////////////////////////////////////
class PinNative : public Pin {
  public:
    PinNative(uint8_t pinAddress);

    void setPinMode(uint8_t mode);
    void doDigitalWrite(uint8_t val);
    int doDigitalRead();
    int doAnalogRead();
    uint8_t getPinAddress() { return _pinAddress; }
};
//////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////
// SWITCH
//////////////////////////////////////////////////////////
class Switch {
  public:
    Switch(Pin* pin);

    bool isPressed(); 

  private:
    Pin* _pin;
};
//////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////
// RELAY
//////////////////////////////////////////////////////////
class Relay {
  public:
    Relay(Pin* controlPin);
    void turnOn();
    void turnOff();

  private:
    Pin* _controlPin;
};
//////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////
// LED
//////////////////////////////////////////////////////////
class LED {
  public:
    LED(Pin* pin);
    void on();
    void off();
    void toggle();
    void blink(unsigned long intervalMs);
    void update(); // Call in loop for blinking

  private:
    Pin* _pin;
    bool _blinking;
    unsigned long _blinkIntervalMs;
    unsigned long _lastBlinkTimeMs;
    bool _currentState;
};
//////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////
// BUZZER (Piezo Buzzer)
//////////////////////////////////////////////////////////
class Buzzer {
  public:
    Buzzer(PinNative* pin);
    void startAlarm(unsigned int frequencyHz = 1000, unsigned long durationMs = 5000); // Start alarm with frequency and duration
    void stop(); // Stop the alarm immediately
    void update(); // Call in loop to handle auto-stop after duration

  private:
    PinNative* _pin;
    bool _isAlarming;
    unsigned long _alarmStartTimeMs;
    unsigned long _alarmDurationMs;
};
//////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////
// VOLTAGE SENSOR (ADC with Voltage Divider)
//////////////////////////////////////////////////////////
class VoltageSensor {
  public:
    // Constructor with divider ratio (0.0 to 1.0)
    // Example: VoltageSensor(new PinNative(A0), 0.1887f) for 100k/530k divider
    VoltageSensor(Pin* pin, float dividerRatio);
    
    // Constructor with divider ratio and calibration factor
    // Example: VoltageSensor(new PinNative(A0), 0.1887f, 1.20f) for 100k/530k divider with calibration
    VoltageSensor(Pin* pin, float dividerRatio, float calibrationFactor);
    
    // Constructor with resistor values (in ohms)
    // rTopOhms: resistor connected to source positive (top of divider)
    // rBottomOhms: resistor connected to ground (bottom of divider)
    // Measures voltage drop across rTopOhms
    // Example: VoltageSensor(new PinNative(A0), 100000.0f, 530000.0f) for 100k/530k divider
    VoltageSensor(Pin* pin, float rTopOhms, float rBottomOhms, float calibrationFactor);
    
    bool init(); // Initialize the sensor (sets pin mode)
    float readVoltageInVolts(); // Returns battery voltage in Volts
    
  private:
    Pin* _pin;
    float _dividerRatio; // Ratio = R1 / (R1 + R2) when measuring across R1
    float _calibrationFactor; // Multiplier to compensate for internal voltage divider
    bool _initialized;
    static const float ADC_REFERENCE_VOLTAGE; // ESP8266 WeMos D1 Mini A0 max input: 3.3V
    static const int ADC_RESOLUTION; // 10-bit ADC: 0-1023
};
//////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////
// DISPLAY (I2C LCD)
//////////////////////////////////////////////////////////
class Display {
  public:
    Display(uint8_t i2cAddress = 0x27, uint8_t columns = 16, uint8_t rows = 2);
    void init();
    void backlight();
    void noBacklight();
    void clear();
    void setCursor(uint8_t col, uint8_t row);
    void print(const char* text);
    void print(float value, int decimals = 2);
    void print(int value);
    void print(unsigned long value);
    
  private:
    LiquidCrystal_I2C* _lcd;
    uint8_t _columns;
    uint8_t _rows;
};
//////////////////////////////////////////////////////////


#endif