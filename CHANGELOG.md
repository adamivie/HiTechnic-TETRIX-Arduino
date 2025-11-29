# Changelog

All notable changes to the HiTechnic TETRIX Arduino Library will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.0.0] - 2025-11-29

### Added - Servo Controller Support
- Complete HiTechnic TETRIX Servo Controller (NSR1038) support
- `HiTechnicServo` class with full API
- Configurable PWM enable modes (0xAA no-timeout default, 0x00 with timeout)
- `begin(pwmMode)` method to select PWM mode at initialization
- `refreshPWM()` method for periodic PWM refresh when using timeout mode
- Servo angle control (0-180°) via `setServoAngle()`
- Servo position control (0-255) via `setServoPosition()`
- Step time control for adjustable servo speed (0-15)
- Individual servo enable/disable functionality
- Center all servos with `centerAll()`
- Read firmware version and status registers

### Added - Examples
- `BasicServoControl` - Simple single servo control example
- `MultipleServos` - Control all 6 servo channels
- `WorkingServoControl` - Complete example with PWM management
- `TestBothPWMModes` - Interactive test for comparing PWM modes
- `PixhawkMotorControl` - QGroundControl/Pixhawk integration for motors
- `PixhawkMotorServoControl` - Combined motor and servo Pixhawk control

### Added - Documentation
- `SERVO_CONTROL_GUIDE.md` - Complete servo control guide
- `QGROUNDCONTROL_INTEGRATION.md` - QGC/Pixhawk integration guide
- `PIXHAWK_QUICKSTART.md` - Quick start for Pixhawk users
- `ROS2_FEATURES_STATUS.md` - ROS2 feature roadmap
- PWM mode documentation in README
- Servo specifications and register map

### Added - Motor Features
- Smooth acceleration control with `setMotorPowerSmooth()`
- Configurable acceleration rate (1-100)
- `update()` method for continuous motor ramping
- `getTargetPower()` and `getCurrentPower()` methods
- Enhanced motor examples with acceleration

### Fixed - Critical Issues
- **Pin 22 Setup**: Added critical Pin 22 HIGH requirement for daisy chain addressing
  - All examples now include `pinMode(22, OUTPUT); digitalWrite(22, HIGH);`
  - Without this, only first controller is detected
  - 10kΩ resistor from Pin 22 to first controller Pin 5 enables analog detection
  
- **PWM Enable**: Servo controller defaults to PWM disabled (0xFF) at power-on
  - Library now automatically enables PWM in `begin()` method
  - Default mode 0xAA (no timeout) confirmed working
  - Optional 0x00 mode (with timeout) available for users who prefer it
  
- **I2C Addressing**: Corrected documentation for 7-bit vs 8-bit addressing
  - Arduino Wire library uses 7-bit addresses
  - Servo controller at 0x04 (4th in daisy chain)
  - Motor controllers at 0x01, 0x02, 0x03

### Changed
- Updated all examples to include Pin 22 setup
- Improved I2C Scanner with Pin 22 detection message
- Enhanced error handling in examples
- Reorganized examples into Motor/Servo/Combined/Diagnostic folders
- Moved experimental examples to Archive folder

### Technical Details
- Extracted and analyzed HiTechnic specifications (motor and servo)
- Confirmed I2C communication protocol
- Verified register map for both motor and servo controllers
- Tested both PWM enable modes (0xAA and 0x00) successfully
- Validated with MG90D metal gear micro servos

### Development Notes
- First Arduino library for HiTechnic TETRIX Servo Controller
- No existing Arduino examples found - this is pioneering work
- Comprehensive testing with 4-controller daisy chain setup
- Pin 22 discovery was breakthrough for full controller detection
- PWM enable mode testing confirmed 0xAA as most reliable default

## [0.1.0] - 2025-11-XX (Pre-release)

### Initial Release
- Basic motor control functionality
- Encoder reading
- Position control
- PID control foundation
- Initial examples and documentation

---

## Version History Summary

- **v1.0.0** - Full servo support, PWM modes, Pixhawk integration, critical fixes
- **v0.1.0** - Initial motor control implementation

## Upgrade Notes

### Upgrading to 1.0.0

**CRITICAL**: All users must add Pin 22 setup to their code:

```cpp
void setup() {
  // REQUIRED for daisy chain addressing
  pinMode(22, OUTPUT);
  digitalWrite(22, HIGH);
  
  // ... rest of setup
}
```

Without this, only the first controller will be detected on the I2C bus.

### Servo Controller Users

The library defaults to PWM mode 0xAA (no timeout). If your servos work with the previous test code using 0x00, you can specify:

```cpp
servoController.begin(0x00);  // Use timeout mode

// Then call refreshPWM() every 5-10 seconds in loop()
void loop() {
  if (millis() - lastRefresh > 5000) {
    servoController.refreshPWM();
    lastRefresh = millis();
  }
}
```

However, 0xAA mode (default) is recommended as it requires no refresh calls.

---

**For detailed information, see the README.md and docs/ folder.**
