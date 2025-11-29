# Acceleration Control Guide

The HiTechnic TETRIX Arduino library includes smooth acceleration control to prevent jerky motor starts and stops. This feature gradually ramps motor power up and down instead of instant changes.

## Why Use Acceleration Control?

- **Smoother Operation**: Motors ramp up and down gradually instead of jerking to full speed
- **Reduced Mechanical Stress**: Gentler on gears, belts, and mechanical components
- **Better Control**: More predictable and controllable robot movements
- **Improved Battery Life**: Reduces current spikes that can drain batteries
- **Quieter Operation**: Eliminates sudden noise from instant power changes

## Basic Usage

### 1. Simple Acceleration

```cpp
#include <HiTechnicMotor.h>

HiTechnicMotor motor(0x01);

void setup() {
  motor.begin();
}

void loop() {
  // Ramp motor to 80% power smoothly
  motor.setMotorPowerSmooth(MOTOR_1, 80);
  
  // Call update() in a loop until ramping is complete
  while (motor.update()) {
    delay(20);
  }
  
  delay(2000);
  
  // Ramp back to stop
  motor.setMotorPowerSmooth(MOTOR_1, 0);
  while (motor.update()) {
    delay(20);
  }
}
```

### 2. Setting Acceleration Rate

The acceleration rate controls how fast the motor ramps up/down:
- **Lower values (1-5)**: Slow, gentle acceleration
- **Medium values (6-15)**: Moderate acceleration (default is 10)
- **Higher values (16-100)**: Fast acceleration

```cpp
// Set slow acceleration
motor.setAcceleration(5);
motor.setMotorPowerSmooth(MOTOR_1, 60);

// Or specify acceleration in the function call
motor.setMotorPowerSmooth(MOTOR_1, 60, 20);  // Fast ramp
```

### 3. Non-Blocking Updates

For advanced control, you can update motors while doing other tasks:

```cpp
void loop() {
  // Start ramping to 70%
  motor.setMotorPowerSmooth(MOTOR_1, 70, 8);
  
  // Do other tasks while motor ramps up
  for (int i = 0; i < 100; i++) {
    motor.update();  // Update acceleration
    
    // Your other code here
    readSensors();
    updateDisplay();
    
    delay(20);
  }
}
```

## API Reference

### setMotorPowerSmooth()

Smoothly ramp motor power to target value.

```cpp
void setMotorPowerSmooth(uint8_t motor, int8_t power, uint8_t acceleration = 10);
```

**Parameters:**
- `motor`: MOTOR_1, MOTOR_2, or MOTOR_BOTH
- `power`: Target power (-100 to 100, negative = reverse)
- `acceleration`: Optional acceleration rate (1-100, default 10)

**Example:**
```cpp
motor.setMotorPowerSmooth(MOTOR_1, 75, 15);  // Fast ramp to 75%
motor.setMotorPowerSmooth(MOTOR_2, -50, 5);  // Slow ramp to 50% reverse
```

### setAcceleration()

Set default acceleration rate for all future smooth power changes.

```cpp
void setAcceleration(uint8_t acceleration);
```

**Parameters:**
- `acceleration`: Rate from 1 (slowest) to 100 (fastest)

**Example:**
```cpp
motor.setAcceleration(8);  // All future smooth commands use rate 8
motor.setMotorPowerSmooth(MOTOR_1, 60);  // Uses rate 8
motor.setMotorPowerSmooth(MOTOR_2, 80);  // Uses rate 8
```

### update()

Update motor power ramping. Call regularly (every 20-50ms) in loop().

```cpp
bool update();
```

**Returns:** `true` if motors are still ramping, `false` if all motors reached target

**Example:**
```cpp
// Blocking: wait until ramping complete
while (motor.update()) {
  delay(20);
}

// Non-blocking: check status
if (motor.update()) {
  Serial.println("Still ramping...");
}
```

### getTargetPower()

Get the target power that motor is ramping toward.

```cpp
int8_t getTargetPower(uint8_t motor);
```

**Returns:** Target power value (-100 to 100)

### getCurrentPower()

Get the current actual power being sent to motor.

```cpp
int8_t getCurrentPower(uint8_t motor);
```

**Returns:** Current power value (-100 to 100)

**Example:**
```cpp
Serial.print("Current: ");
Serial.print(motor.getCurrentPower(MOTOR_1));
Serial.print("% → Target: ");
Serial.print(motor.getTargetPower(MOTOR_1));
Serial.println("%");
```

## Acceleration vs. Instant Power

### Instant Power (setMotorPower)

Use when you need immediate response:
- Emergency stops
- Testing motors
- When you need exact timing
- Quick directional changes

```cpp
motor.setMotorPower(MOTOR_1, 100);  // Instant full power
```

### Smooth Power (setMotorPowerSmooth)

Use for normal operation:
- Starting/stopping motors
- Changing speeds
- Smoother robot movements
- Reducing mechanical stress

```cpp
motor.setMotorPowerSmooth(MOTOR_1, 100);  // Gradual ramp to full power
while (motor.update()) { delay(20); }
```

## Multiple Motors

Control multiple motors with coordinated smooth movements:

```cpp
HiTechnicMotor controller1(0x01);
HiTechnicMotor controller2(0x02);

void setup() {
  controller1.begin();
  controller2.begin();
  
  // Set same acceleration rate for coordination
  controller1.setAcceleration(10);
  controller2.setAcceleration(10);
}

void loop() {
  // Start all motors ramping together
  controller1.setMotorPowerSmooth(MOTOR_BOTH, 60);
  controller2.setMotorPowerSmooth(MOTOR_BOTH, 60);
  
  // Update all controllers until ramping complete
  while (controller1.update() | controller2.update()) {
    delay(20);
  }
}
```

## Practical Examples

### Robot Drive Base

```cpp
// Smooth forward acceleration
driveLeft.setMotorPowerSmooth(MOTOR_1, 70, 12);
driveRight.setMotorPowerSmooth(MOTOR_1, 70, 12);
while (driveLeft.update() | driveRight.update()) { delay(20); }

delay(3000);  // Drive forward

// Smooth stop
driveLeft.setMotorPowerSmooth(MOTOR_1, 0, 12);
driveRight.setMotorPowerSmooth(MOTOR_1, 0, 12);
while (driveLeft.update() | driveRight.update()) { delay(20); }
```

### Arm Movement

```cpp
// Slow, controlled arm movement
armMotor.setAcceleration(3);  // Very slow
armMotor.setMotorPowerSmooth(MOTOR_1, 40);
while (armMotor.update()) {
  delay(20);
  // Can monitor encoders here
}
```

### Speed Changes During Movement

```cpp
// Start at medium speed
motor.setMotorPowerSmooth(MOTOR_1, 50, 10);
while (motor.update()) { delay(20); }

delay(2000);

// Increase to high speed smoothly
motor.setMotorPowerSmooth(MOTOR_1, 90, 10);
while (motor.update()) { delay(20); }

delay(2000);

// Decrease back to medium
motor.setMotorPowerSmooth(MOTOR_1, 50, 10);
while (motor.update()) { delay(20); }
```

## Tips and Best Practices

1. **Call update() regularly**: For smooth operation, call `update()` every 20-50ms
2. **Match acceleration rates**: Use same rates for coordinated multi-motor movements
3. **Tune for your application**: Test different rates to find what works best
4. **Emergency stops**: Use instant `setMotorPower(MOTOR_BOTH, 0)` for immediate stops
5. **Monitor progress**: Use `getCurrentPower()` to track ramping progress
6. **Non-blocking updates**: Don't block your main loop - update motors each cycle

## Troubleshooting

**Motors not ramping smoothly:**
- Ensure you're calling `update()` regularly (every 20-50ms)
- Check that acceleration rate isn't too high (try lower values)
- Verify power supply can handle current demands

**Ramping too slow:**
- Increase acceleration rate: `setAcceleration(20)`
- Or specify faster rate: `setMotorPowerSmooth(MOTOR_1, 100, 25)`

**Ramping too fast:**
- Decrease acceleration rate: `setAcceleration(5)`
- Try values between 1-10 for gentle ramping

**Motors jerking:**
- Lower acceleration rate for smoother transitions
- Ensure update() is called at consistent intervals
- Check mechanical connections aren't binding
