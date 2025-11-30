# Daisy Chain Configuration Guide

## Overview

The HiTechnic TETRIX controllers support daisy chain configurations, allowing you to connect up to 4 controllers on a single I2C bus. The automatic addressing system assigns addresses based on the controller's position in the chain.

## How Automatic Addressing Works

When Pin 22 is set HIGH through a 10kΩ resistor to the first controller's Pin 5:

1. **First controller** detects 5V on Pin 5 → assigns itself address **0x01**
2. **Second controller** detects forwarded voltage → assigns itself address **0x02**
3. **Third controller** detects forwarded voltage → assigns itself address **0x03**
4. **Fourth controller** detects forwarded voltage → assigns itself address **0x04**

**The type of controller (motor or servo) doesn't matter** - the address is determined purely by position in the chain!

## Common Configurations

### Configuration 1: 3 Motor + 1 Servo (Your Setup)
```
Arduino Pin 22 → [10kΩ] → Motor1 (0x01) → Motor2 (0x02) → Motor3 (0x03) → Servo (0x04)
```

**Code Example:**
```cpp
#include <HiTechnicMotor.h>
#include <HiTechnicServo.h>

// Motors at addresses 0x01, 0x02, 0x03
HiTechnicMotor motor1(0x01);
HiTechnicMotor motor2(0x02);
HiTechnicMotor motor3(0x03);

// Servo at address 0x04
HiTechnicServo servo(0x04);

void setup() {
  // CRITICAL: Enable Pin 22 for daisy chain
  pinMode(22, OUTPUT);
  digitalWrite(22, HIGH);
  
  motor1.begin();
  motor2.begin();
  motor3.begin();
  servo.begin();
}
```

### Configuration 2: 1 Servo + 3 Motors
```
Arduino Pin 22 → [10kΩ] → Servo (0x01) → Motor1 (0x02) → Motor2 (0x03) → Motor3 (0x04)
```

**Code Example:**
```cpp
// Servo at address 0x01 (first in chain)
HiTechnicServo servo(0x01);

// Motors at addresses 0x02, 0x03, 0x04
HiTechnicMotor motor1(0x02);
HiTechnicMotor motor2(0x03);
HiTechnicMotor motor3(0x04);
```

### Configuration 3: 2 Motors + 2 Servos
```
Arduino Pin 22 → [10kΩ] → Motor1 (0x01) → Motor2 (0x02) → Servo1 (0x03) → Servo2 (0x04)
```

**Code Example:**
```cpp
HiTechnicMotor motor1(0x01);
HiTechnicMotor motor2(0x02);
HiTechnicServo servo1(0x03);
HiTechnicServo servo2(0x04);
```

### Configuration 4: 2 Motors Only
```
Arduino Pin 22 → [10kΩ] → Motor1 (0x01) → Motor2 (0x02)
```

**Code Example:**
```cpp
HiTechnicMotor motor1(0x01);
HiTechnicMotor motor2(0x02);
```

### Configuration 5: 4 Motors (Maximum)
```
Arduino Pin 22 → [10kΩ] → Motor1 (0x01) → Motor2 (0x02) → Motor3 (0x03) → Motor4 (0x04)
```

**Code Example:**
```cpp
HiTechnicMotor motor1(0x01);
HiTechnicMotor motor2(0x02);
HiTechnicMotor motor3(0x03);
HiTechnicMotor motor4(0x04);
```

### Configuration 6: Mixed Configuration
```
Arduino Pin 22 → [10kΩ] → Servo1 (0x01) → Motor1 (0x02) → Servo2 (0x03) → Motor2 (0x04)
```

**Code Example:**
```cpp
HiTechnicServo servo1(0x01);
HiTechnicMotor motor1(0x02);
HiTechnicServo servo2(0x03);
HiTechnicMotor motor2(0x04);
```

## Finding Your Configuration

### Step 1: Run I2C Scanner

Upload the `I2CScanner` example from `examples/Diagnostic/I2CScanner/`:

```cpp
cd "c:\tetrix motor controllers"
arduino-cli compile --fqbn arduino:avr:mega examples/Diagnostic/I2CScanner
arduino-cli upload -p COM12 --fqbn arduino:avr:mega examples/Diagnostic/I2CScanner
arduino-cli monitor -p COM12 -c baudrate=9600
```

**Expected Output:**
```
I2C Scanner
Pin 22 analog detection enabled for daisy chain
Scanning...

I2C device found at address 0x01  !
I2C device found at address 0x02  !
I2C device found at address 0x03  !
I2C device found at address 0x04  !

done
```

### Step 2: Identify Controller Types

You need to physically check which controller is in which position in your daisy chain:

1. **Look at the controllers** - Motor controllers and servo controllers are physically different
2. **Check the labels** - Motor: NMO1038, Servo: NSR1038
3. **Note the order** - Which one is connected to Pin 22? That's 0x01

### Step 3: Create Your Configuration

Based on your physical setup, create objects with the correct addresses:

```cpp
// Example: If your chain is Servo → Motor → Motor → Motor
HiTechnicServo servo(0x01);    // First in chain
HiTechnicMotor motor1(0x02);   // Second in chain
HiTechnicMotor motor2(0x03);   // Third in chain
HiTechnicMotor motor3(0x04);   // Fourth in chain
```

## Wiring Diagram

```
                    ┌─────────────┐
                    │  Arduino    │
                    │  Mega 2560  │
                    └─────────────┘
                          │
         ┌────────────────┼────────────────┐
         │                │                │
      Pin 20           Pin 21           Pin 22
       (SDA)            (SCL)        (via 10kΩ)
         │                │                │
         │                │                │
    ┌────┴────────────────┴────────────────┴─────┐
    │                                             │
    │         ┌──────────────────────┐           │
    │         │  Controller #1       │           │
    │         │  Address: 0x01       │◄──────────┘
    │         │  (Motor or Servo)    │
    │         └──────────────────────┘
    │                  │
    │         ┌──────────────────────┐
    │         │  Controller #2       │
    │         │  Address: 0x02       │
    │         │  (Motor or Servo)    │
    │         └──────────────────────┘
    │                  │
    │         ┌──────────────────────┐
    │         │  Controller #3       │
    │         │  Address: 0x03       │
    │         │  (Motor or Servo)    │
    │         └──────────────────────┘
    │                  │
    │         ┌──────────────────────┐
    │         │  Controller #4       │
    │         │  Address: 0x04       │
    │         │  (Motor or Servo)    │
    │         └──────────────────────┘
    │                  │
    └──────────────────┴──────────────────────────
                      GND
```

## Important Notes

### ⚠️ Critical Requirements

1. **Pin 22 MUST be HIGH** - Without this, only the first controller is detected:
   ```cpp
   pinMode(22, OUTPUT);
   digitalWrite(22, HIGH);
   ```

2. **10kΩ resistor required** - From Arduino Pin 22 to first controller Pin 5

3. **Physical position = Address** - The address is determined by position, not controller type

4. **Power all controllers** - Each controller needs its own 12V battery/power supply

5. **Common ground** - Arduino GND must connect to all controller GNDs

### 🔍 Troubleshooting

**Problem: Only first controller detected**
- Check Pin 22 is HIGH in your code
- Verify 10kΩ resistor connection
- Check daisy chain cable connections

**Problem: Wrong number of controllers detected**
- Power cycle all controllers
- Check cable connections between controllers
- Verify cables are 6P6C (RJ12) straight-through, not crossover

**Problem: Controllers work individually but not in chain**
- Ensure daisy chain cables are properly seated
- Check that each controller is forwarding the detection signal
- Try different cable order to isolate bad cable/controller

## Configuration Template

Use this template to document your setup:

```cpp
/*
  My Daisy Chain Configuration
  
  Physical Order (Pin 22 → Controller1 → Controller2 → Controller3 → Controller4):
  Position 1: [Motor/Servo] Controller - Address 0x01 - [Description]
  Position 2: [Motor/Servo] Controller - Address 0x02 - [Description]
  Position 3: [Motor/Servo] Controller - Address 0x03 - [Description]
  Position 4: [Motor/Servo] Controller - Address 0x04 - [Description]
*/

#include <HiTechnicMotor.h>
#include <HiTechnicServo.h>

// Declare controllers based on your physical setup
HiTechnicMotor  motor1(0x01);   // Replace with your configuration
HiTechnicMotor  motor2(0x02);
HiTechnicMotor  motor3(0x03);
HiTechnicServo  servo1(0x04);

void setup() {
  // CRITICAL: Enable daisy chain addressing
  pinMode(22, OUTPUT);
  digitalWrite(22, HIGH);
  
  // Initialize all controllers
  motor1.begin();
  motor2.begin();
  motor3.begin();
  servo1.begin();
}

void loop() {
  // Your code here
}
```

## Advanced: Manual Address Override

If you need to override the automatic addressing (not recommended), you can do so by not connecting Pin 22 and manually setting jumpers on each controller. Refer to the HiTechnic documentation for jumper settings.

## Example Projects

See `examples/` folder for complete examples:

- **Motor/SixMotorsWorking** - Controls 6 motors (3 motor controllers)
- **Servo/MultipleServos** - Controls 6 servos (1 servo controller)
- **Combined/PixhawkMotorServoControl** - Combined motor + servo control

## Summary

**Key Takeaway**: The I2C address is determined by **physical position** in the daisy chain, not by controller type. Always:

1. ✅ Enable Pin 22 HIGH
2. ✅ Use 10kΩ resistor to first controller
3. ✅ Map your physical setup to addresses
4. ✅ Declare objects with correct addresses
5. ✅ Initialize all controllers in `setup()`

**Position in chain = I2C address** - It's that simple! 🎯
