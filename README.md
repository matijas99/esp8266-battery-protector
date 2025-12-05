# esp8266-battery-protector

This circuit protects a 12V car battery (or similar lead-acid battery) from deep discharge by continuously monitoring the battery voltage and disconnecting the load once the voltage drops below 10.5V (indicating low battery capacity).

The circuit continuously monitors the battery voltage regardless of load state. When the voltage drops below 10.5V (the cutoff threshold), the relay opens immediately and disconnects the load to prevent deep discharge damage to the battery.

**Important**: If the circuit is connected to a battery with voltage already below 10.5V, it will cut off immediately upon startup.

The controller is powered from the 12V battery via a DC-DC converter that supplies the required 5V rail. The same battery connection is used for both powering the circuit and monitoring the voltage.

**Auto-Rearming Logic:**
- After the relay opens due to low voltage, the circuit attempts to rearm every minute by closing the relay and checking the voltage again.
- If the voltage drops below 10.5V again immediately, the relay reopens.
- This periodic rearming prevents the relay from staying permanently open, allowing the circuit to automatically reconnect if the battery voltage recovers.

LED behavior:
- Green solid: battery voltage is above threshold and relay is closed (load connected).
- Red solid: relay opened; battery voltage dropped below 10.5V cutoff threshold.

**Testing Button Functionality:**
The hardware button serves as a testing/manual control button:
- **Pressed while armed** (relay closed, voltage OK): Simulates voltage drop below 10.5V, immediately triggering cutoff and opening the relay. Useful for testing the cutoff functionality.
- **Pressed while triggered** (relay open, voltage low): Immediately rearms the circuit, skipping the 1-minute wait period. Useful for manual recovery or testing the rearming functionality.

# Hardware:
- ESP8266 WeMos D1 Mini Pro v3.0 NodeMcu 4MB/16MB WiFi board
Pinout:
RST      TX
A0       RX
D0       D1
D5       D2
D6       D3
D7       D4
D8       GND
3V3      5V

- Voltage Divider Resistors
  - R1: 100kΩ (top resistor, connected to battery positive)
  - R2: 430kΩ (bottom resistor, made from 100kΩ + 330kΩ in series, connected to ground)

- Relay module Songle 10A 250V SRD-05VDC-SL-C
Pinout:
S
+
-

- LCD Module LCD1602 16x2 PCF8574T I2C (currently unused)
Pinout:
GND
VCC
SDA
SCL

- Red LED

- Green LED

- Hardware button

# Wiring Diagram

## ESP8266 WeMos D1 Mini Pro Pinout Analysis

The ESP8266 WeMos D1 Mini Pro v3.0 has the following pin types:

**Digital Pins (GPIO):**
- **D0** (GPIO16): Digital I/O, no pull-up resistor
- **D1** (GPIO5): Digital I/O, commonly used for I²C SCL
- **D2** (GPIO4): Digital I/O, commonly used for I²C SDA
- **D3** (GPIO0): Digital I/O, pulled HIGH on boot (boot mode pin)
- **D4** (GPIO2): Digital I/O, pulled HIGH on boot
- **D5** (GPIO14): Digital I/O (SPI CLK)
- **D6** (GPIO12): Digital I/O (SPI MISO)
- **D7** (GPIO13): Digital I/O (SPI MOSI)
- **D8** (GPIO15): Digital I/O, pulled LOW on boot (boot mode pin)

**Power Pins:**
- **5V**: 5V power input/output
- **3V3**: 3.3V power output
- **GND**: Ground

**Other:**
- **RST**: Reset pin (active LOW)
- **TX/RX**: Serial communication pins

## Component Connections

### Power Supply
- **12V Battery** → **5V DC-DC Converter** → **5V rail**
- **5V rail** → ESP8266 **5V pin**, Relay module **VCC**
- **GND**: Common ground for all components (ESP8266, relay, LEDs, button, voltage divider)

### Power Flow Path (12V Battery → Load)
The relay controls power flow to the load:
- **12V Battery Positive** → **Relay COM** → **Relay NO** → **Load Positive Terminal**
- **12V Battery Negative** → **Load Negative Terminal** (common ground)

**Note**: The voltage divider is connected in parallel to the battery, allowing it to monitor voltage without affecting the power flow path. The circuit continuously monitors battery voltage regardless of whether the load is connected or not.

### Voltage Divider Circuit
The voltage divider uses two resistors to scale down the battery voltage to a safe level for the ESP8266's A0 analog input pin (maximum 3.3V).

**Connections:**
- **12V Battery Positive** → **R1 (100kΩ)** → **A0 pin** (junction between R1 and R2) → **R2 (430kΩ)** → **GND**
- The voltage divider ratio is: R1 / (R1 + R2) = 100k / 530k = 0.1887
- Maximum measurable voltage: 3.3V / 0.1887 = ~17.5V (covers 12V battery and charging voltages up to ~17V)

**Voltage Measurement Setup:**
- R1 (100kΩ) is the top resistor connected to battery positive
- R2 (430kΩ) is the bottom resistor (made from 100kΩ + 330kΩ in series) connected to ground
- A0 pin connects to the junction between R1 and R2
- The ESP8266 reads the voltage at A0 (0-3.3V range) and converts it back to battery voltage using the divider ratio
- The circuit monitors battery voltage continuously, regardless of load state

### Relay Module (Songle SRD-05VDC-SL-C)
- **VCC (+)** → **5V** (from 5V rail)
- **GND (-)** → **GND** (common ground)
- **S (Signal/IN)** → **D6 (GPIO12)** (digital control pin)
- **COM** → Connect to **12V Battery Positive**
- **NO** → Connect to load positive terminal
- **Note**: When GPIO12 is set HIGH, the relay activates and connects COM to NO, allowing current to flow to the load. When LOW, the circuit is broken and no current flows. The relay opens immediately when voltage drops below 10.5V, and attempts to rearm every minute.

### Green LED (Status Indicator)
- **Anode (+)** → **D4 (GPIO2)** via **220Ω current-limiting resistor**
- **Cathode (-)** → **GND**
- **Behavior**: 
  - Solid ON: Battery voltage above threshold and relay is closed (load connected)
  - OFF: Relay opened (voltage below threshold)

### Red LED (Cutoff Indicator)
- **Anode (+)** → **D5 (GPIO14)** via **220Ω current-limiting resistor**
- **Cathode (-)** → **GND**
- **Behavior**: ON when relay is opened (battery voltage below 10.5V cutoff threshold)

### Hardware Testing Button
- **Terminal 1** → **D3 (GPIO0)** (with internal pull-up enabled in code)
- **Terminal 2** → **GND**
- **Functionality**:
  - **Pressed while armed** (relay closed, voltage above threshold): Simulates voltage drop below 10.5V, immediately triggering cutoff and opening the relay. Useful for testing.
  - **Pressed while triggered** (relay open, voltage below threshold): Immediately rearms the circuit, skipping the 1-minute auto-rearm wait period. Useful for manual recovery or testing.

### LCD Module (LCD1602 PCF8574T I²C)
- **VCC** → **5V** (from 5V rail)
- **GND** → **GND** (common ground)
- **SDA** → **D2 (GPIO4)** (I²C data line)
- **SCL** → **D1 (GPIO5)** (I²C clock line)
- **I²C Address**: Typically `0x27` (may vary - check with I²C scanner if needed)
- **Display Format**:
  - **Top row**: `Battery: X.XX V` - Current battery voltage
  - **Bottom row**: `Load relay: ON` or `Load relay: OFF` - Current relay state

## Pin Assignment Summary

| Component | ESP8266 Pin | Pin Type | GPIO Number | Notes |
|-----------|-------------|----------|-------------|-------|
| Voltage Sensor | A0 | Analog | ADC | Voltage divider input (max 3.3V) |
| Relay Control | D6 | Digital | GPIO12 | Controls relay ON/OFF |
| Green LED | D4 | Digital | GPIO2 | Status indicator (pulled HIGH on boot) |
| Red LED | D5 | Digital | GPIO14 | Cutoff indicator |
| Test Button | D3 | Digital | GPIO0 | Testing/manual control (pulled HIGH on boot) |
| LCD SDA | D2 | Digital | GPIO4 | I²C data line |
| LCD SCL | D1 | Digital | GPIO5 | I²C clock line |

