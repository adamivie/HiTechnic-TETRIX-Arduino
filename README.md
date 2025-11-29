# HiTechnic TETRIX Motor Controller Library for Arduino

Complete Arduino library for controlling HiTechnic TETRIX DC Motor and Servo Controllers via I2C.

![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)

## Features

- ✅ Control HiTechnic TETRIX DC Motor Controllers (NMO1038)
- ✅ Control HiTechnic TETRIX Servo Controllers (NSR1038)
- ✅ Support for daisy-chained controllers (up to 4 controllers)
- ✅ Independent control of 6 DC motors (3 controllers × 2 motors)
- ✅ Independent control of 6 servo motors (1 controller × 6 servos)
- ✅ Encoder reading and position control
- ✅ PID control for motors
- ✅ Comprehensive examples and documentation

## Hardware Requirements

- Arduino Mega 2560 (or compatible with hardware I2C)
- HiTechnic TETRIX Motor Controller (NMO1038) - up to 3 units
- HiTechnic TETRIX Servo Controller (NSR1038) - optional
- 10kΩ resistor (for daisy chain addressing)
- 6P6C (RJ12) cables for daisy chain connections

## Quick Start

### Wiring

```
Arduino Mega 2560
├─ Pin 20 (SDA) ──────────> All controllers Pin 6 (daisy chained)
├─ Pin 21 (SCL) ──────────> All controllers Pin 5 (daisy chained)
├─ Pin 22 ───[10kΩ]───────> First controller Pin 5 (5V analog detection)
└─ GND ───────────────────> All controllers Pin 1 or 2
```

The 10kΩ resistor provides 5V analog detection for automatic daisy chain addressing:
- First controller → Address 0x01
- Second controller → Address 0x02
- Third controller → Address 0x03
- Fourth controller → Address 0x04

### Installation

1. Download this repository as ZIP
2. In Arduino IDE: Sketch → Include Library → Add .ZIP Library
3. Select the downloaded ZIP file

Or manually:
```bash
cd ~/Documents/Arduino/libraries/
git clone https://github.com/adamivie/HiTechnic-TETRIX-Arduino.git
```

### Basic Usage

```cpp
#include "HiTechnicMotor.h"

#define ANALOG_DETECT_PIN 22

HiTechnicMotor controller1(0x01);
HiTechnicMotor controller2(0x02);
HiTechnicMotor controller3(0x03);

void setup() {
  // Enable daisy chain addressing
  pinMode(ANALOG_DETECT_PIN, OUTPUT);
  digitalWrite(ANALOG_DETECT_PIN, HIGH);
  
  controller1.begin();
  controller2.begin();
  controller3.begin();
}

void loop() {
  // Run motor 1 on controller 1 at 50% forward
  controller1.setMotorPower(MOTOR_1, 50);
  delay(2000);
  
  // Stop motor
  controller1.setMotorPower(MOTOR_1, 0);
  delay(1000);
}
```

### Smooth Acceleration

```cpp
#include "HiTechnicMotor.h"

HiTechnicMotor controller(0x01);

void setup() {
  controller.begin();
  controller.setAcceleration(10);  // Set acceleration rate (1-100)
}

void loop() {
  // Smoothly ramp motor to 80% power
  controller.setMotorPowerSmooth(MOTOR_1, 80);
  
  // Call update() regularly to handle ramping
  while (controller.update()) {
    delay(20);  // Update every 20ms
  }
  
  delay(2000);
  
  // Smoothly ramp back to stop
  controller.setMotorPowerSmooth(MOTOR_1, 0);
  while (controller.update()) {
    delay(20);
  }
  
  delay(1000);
}
```

## Examples

### Motor Control
- **SixMotorsWorking** - Complete demo of 6 independent motors (recommended)
- **BasicMotorControl** - Simple single motor control
- **DualMotorControl** - Control both motors on one controller
- **EncoderReading** - Read motor encoder values
- **PositionControl** - Move motors to specific positions

### Servo Control
- **BasicServoControl** - Control single servo (recommended starting point)
- **MultipleServos** - Control all 6 servos
- **WorkingServoControl** - Complete working example with PWM management
- **TestBothPWMModes** - Interactive test comparing PWM modes (0xAA vs 0x00)

### Diagnostic
- **I2CScanner** - Scan for connected controllers
- **DaisyChainAddressTest** - Verify daisy chain addressing

## Library Reference

### HiTechnicMotor Class

```cpp
HiTechnicMotor(uint8_t address);  // Constructor
void begin();                      // Initialize controller

// Basic motor control
void setMotorPower(uint8_t motor, int8_t power);  // Set motor power (-100 to 100)

// Smooth acceleration control
void setMotorPowerSmooth(uint8_t motor, int8_t power, uint8_t acceleration = 10);
void setAcceleration(uint8_t acceleration);  // Set default acceleration rate (1-100)
bool update();                     // Update motor ramping (call in loop())
int8_t getTargetPower(uint8_t motor);   // Get target power
int8_t getCurrentPower(uint8_t motor);  // Get current actual power

// Motor control
void stopMotor(uint8_t motor);     // Stop specific motor
void stopAllMotors();              // Stop both motors

// Encoder functions
int32_t readEncoder(uint8_t motor);  // Read encoder value
void resetEncoder(uint8_t motor);  // Reset encoder to zero
void setTargetPosition(uint8_t motor, int32_t target);  // Position control

// Utility
uint8_t readVersion();             // Read firmware version
```

### HiTechnicServo Class

```cpp
HiTechnicServo(uint8_t address);   // Constructor
void begin(uint8_t pwmMode = 0xAA); // Initialize controller
                                    // pwmMode: 0xAA = no timeout (default)
                                    //          0x00 = 10-second timeout
void setServoAngle(uint8_t servo, uint8_t angle);  // Set angle (0-180°)
void setServoPosition(uint8_t servo, uint8_t position);  // Set position (0-255)
void centerServo(uint8_t servo);   // Center servo (90°)
void centerAll();                  // Center all servos
void setStepTime(uint8_t time);    // Set movement speed (0-15, 0=fastest)
void disableServo(uint8_t servo);  // Disable servo (no pulse)
void enableServo(uint8_t servo);   // Re-enable servo
void refreshPWM();                 // Refresh PWM (for 0x00 mode only)
uint8_t readVersion();             // Read firmware version
uint8_t readStatus();              // Read status register
```

## Motor Controller Specifications

- **Power Range**: -100 to +100 (0 = brake, -128 = float)
- **Encoder Resolution**: 1440 counts per motor shaft revolution (0.25° per count)
- **Operating Modes**: Power control, constant speed, position control, encoder reset
- **Current Capacity**: 4 amps per motor channel
- **Battery Voltage**: 9-15V
- **I2C Timeout**: 2.5 seconds (can be disabled with NTO bit)

## Servo Controller Specifications

- **Channels**: 6 servo outputs
- **PWM Range**: 0.75ms - 2.25ms (0-255 position value)
- **Step Time**: 0-15 (0=immediate, 1-15 adjustable movement speed)
- **PWM Enable Modes**: 
  - 0xAA = No timeout (default, recommended)
  - 0x00 = 10-second timeout (requires periodic `refreshPWM()` calls)
  - 0xFF = Disabled (power-on default)
- **I2C Address**: 0x04 (default, for 4th controller in daisy chain)

## Troubleshooting

### Controllers not found
- Verify 10kΩ resistor connection from Pin 22 to first controller Pin 5
- Power cycle all controllers
- Run `I2CScanner` example to verify addresses

### Motor crosstalk (one motor triggers another)
- Ensure you're setting MODE register before POWER register
- This is handled automatically by the library
- Verify you're using the latest version

### Timeout issues
- Set NTO (No Time Out) bit in mode register
- Send I2C commands at least every 2 seconds
- See examples for proper implementation

## Technical Details

### Daisy Chain Addressing

The HiTechnic controllers use 5V analog detection through a 10kΩ resistor for automatic address assignment:

1. First controller detects 5V on Pin 5 → assigns address 0x01
2. First controller forwards signal to second controller
3. Second controller detects forwarded voltage → assigns address 0x02
4. Process continues for up to 4 controllers

### Register Map

See `docs/SPECIFICATIONS.md` for complete register documentation.

### Critical Fix: MODE Before POWER

The library automatically sets the MODE register before the POWER register, which is **required** by the HiTechnic firmware for proper operation (per specification page 6).

## Documentation

- [Complete Wiring Guide](docs/WIRING.md)
- [Daisy Chain Setup](docs/DAISY_CHAIN_ADDRESSING.md)
- [API Reference](docs/API_REFERENCE.md)
- [Specifications](docs/SPECIFICATIONS.md)
- [Troubleshooting](docs/TROUBLESHOOTING.md)

## Contributing

Contributions are welcome! Please:
1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Submit a pull request

## License

MIT License - see LICENSE.txt for details

## Credits

- Library by: Adam Ivie
- Based on HiTechnic FIRST Motor Controller Specification Rev 1.4
- Based on HiTechnic FIRST Servo Controller Specification Rev 1.3
- Hardware by: HiTechnic / Dataport Systems, Inc.

## Support

For issues, questions, or contributions:
- GitHub Issues: https://github.com/adamivie/HiTechnic-TETRIX-Arduino/issues
- Email: adamivie@gmail.com

## Acknowledgments

- HiTechnic for excellent TETRIX hardware
- Arduino community for I2C libraries
- FIRST Robotics for inspiring education

---

**Made with ❤️ for robotics education**
