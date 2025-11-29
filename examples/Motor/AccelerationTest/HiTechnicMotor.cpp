/*
  HiTechnicMotor.cpp - Library for controlling HiTechnic TETRIX Motor Controllers
  via I2C on Arduino Mega
*/

#include "HiTechnicMotor.h"

// Constructor
HiTechnicMotor::HiTechnicMotor(uint8_t address) {
  _address = address;
  _motor1TargetPower = 0;
  _motor2TargetPower = 0;
  _motor1CurrentPower = 0;
  _motor2CurrentPower = 0;
  _motor1PreviousTarget = 0;
  _motor2PreviousTarget = 0;
  _acceleration = 10;    // Default acceleration rate
  _deceleration = 10;    // Default deceleration rate
  _lastUpdateTime = 0;
  _motor1FreewheelStartTime = 0;
  _motor2FreewheelStartTime = 0;
  _motor1Freewheeling = false;
  _motor2Freewheeling = false;
}

// Initialize the motor controller
void HiTechnicMotor::begin() {
  Wire.begin();
  delay(100); // Allow controller to initialize
  
  // Set both motors to power mode by default
  setMotorMode(MOTOR_BOTH, MOTOR_MODE_POWER);
  
  // Stop all motors
  stopAll();
  
  // Reset encoders
  resetAllEncoders();
}

// Set motor power (-100 to 100)
void HiTechnicMotor::setMotorPower(uint8_t motor, int8_t power) {
  // Constrain power to valid range
  power = constrain(power, -100, 100);
  
  // Update internal tracking variables for immediate power changes
  if (motor == MOTOR_1 || motor == MOTOR_BOTH) {
    _motor1CurrentPower = power;
    _motor1TargetPower = power;
  }
  if (motor == MOTOR_2 || motor == MOTOR_BOTH) {
    _motor2CurrentPower = power;
    _motor2TargetPower = power;
  }
  
  // Per spec: must set MODE before POWER for proper operation
  if (motor == MOTOR_1 || motor == MOTOR_BOTH) {
    writeRegister(HT_MOTOR1_MODE, MOTOR_MODE_POWER);  // Set to power control mode
    writeRegister(HT_MOTOR1_POWER, (uint8_t)power);
  }
  
  if (motor == MOTOR_2 || motor == MOTOR_BOTH) {
    writeRegister(HT_MOTOR2_MODE, MOTOR_MODE_POWER);  // Set to power control mode
    writeRegister(HT_MOTOR2_POWER, (uint8_t)power);
  }
}

// Set motor power with smooth acceleration ramping
void HiTechnicMotor::setMotorPowerSmooth(uint8_t motor, int8_t power, uint8_t acceleration, uint8_t deceleration) {
  // Constrain power to valid range
  power = constrain(power, -100, 100);
  
  // Detect transition from acceleration to deceleration (going from high power to lower/zero)
  if (motor == MOTOR_1 || motor == MOTOR_BOTH) {
    int8_t currentAbs = abs(_motor1CurrentPower);
    int8_t targetAbs = abs(power);
    
    // If we're currently moving and new target is significantly lower (deceleration)
    if (currentAbs > 30 && targetAbs < currentAbs - 20) {
      _motor1Freewheeling = true;
      _motor1FreewheelStartTime = millis();
      // Send freewheel command immediately
      writeRegister(HT_MOTOR1_MODE, MOTOR_MODE_POWER);
      writeRegister(HT_MOTOR1_POWER, (uint8_t)MOTOR_FLOAT);
    }
    
    _motor1TargetPower = power;
    _motor1PreviousTarget = power;
  }
  
  if (motor == MOTOR_2 || motor == MOTOR_BOTH) {
    int8_t currentAbs = abs(_motor2CurrentPower);
    int8_t targetAbs = abs(power);
    
    // If we're currently moving and new target is significantly lower (deceleration)
    if (currentAbs > 30 && targetAbs < currentAbs - 20) {
      _motor2Freewheeling = true;
      _motor2FreewheelStartTime = millis();
      // Send freewheel command immediately
      writeRegister(HT_MOTOR2_MODE, MOTOR_MODE_POWER);
      writeRegister(HT_MOTOR2_POWER, (uint8_t)MOTOR_FLOAT);
    }
    
    _motor2TargetPower = power;
    _motor2PreviousTarget = power;
  }
  
  // Store acceleration rate if provided
  if (acceleration > 0) {
    _acceleration = constrain(acceleration, 1, 100);
  }
  
  // Store deceleration rate if provided (0 means use acceleration rate)
  if (deceleration > 0) {
    _deceleration = constrain(deceleration, 1, 100);
  } else if (acceleration > 0) {
    _deceleration = _acceleration;  // Use acceleration rate if deceleration not specified
  }
}

// Update motor power ramping (call this regularly in loop())
bool HiTechnicMotor::update() {
  unsigned long currentTime = millis();
  
  // Limit update rate to avoid overwhelming I2C bus
  if (currentTime - _lastUpdateTime < 20) {  // Update every 20ms max
    return (_motor1CurrentPower != _motor1TargetPower || 
            _motor2CurrentPower != _motor2TargetPower);
  }
    _lastUpdateTime = currentTime;
  bool stillRamping = false;
  
  // Handle Motor 1 freewheel period
  if (_motor1Freewheeling) {
    // Check if 1 second has elapsed
    if (currentTime - _motor1FreewheelStartTime >= 1000) {
      _motor1Freewheeling = false;
      // Now start controlled deceleration from current (lower) speed
    } else {
      // Still freewheeling, don't do normal ramping
      stillRamping = true;
    }
  }
  
  // Ramp Motor 1 (only if not freewheeling)
  if (!_motor1Freewheeling && _motor1CurrentPower != _motor1TargetPower) {
    stillRamping = true;
    
    // Determine if we're accelerating (increasing power magnitude) or decelerating (decreasing)
    int8_t currentAbs = abs(_motor1CurrentPower);
    int8_t targetAbs = abs(_motor1TargetPower);
    uint8_t rampRate = (targetAbs > currentAbs) ? _acceleration : _deceleration;
    
    if (_motor1CurrentPower < _motor1TargetPower) {
      _motor1CurrentPower += rampRate;
      if (_motor1CurrentPower > _motor1TargetPower) {
        _motor1CurrentPower = _motor1TargetPower;
      }
    } else {
      _motor1CurrentPower -= rampRate;
      if (_motor1CurrentPower < _motor1TargetPower) {
        _motor1CurrentPower = _motor1TargetPower;
      }
    }
    
    // If decelerating and power drops below threshold, switch to freewheel
    int8_t powerToWrite = _motor1CurrentPower;
    if (targetAbs < currentAbs && abs(_motor1CurrentPower) < FREEWHEEL_THRESHOLD && _motor1TargetPower == 0) {
      powerToWrite = MOTOR_FLOAT;  // -128 = freewheel mode
      _motor1CurrentPower = 0;     // Consider it stopped for tracking purposes
    }
    
    writeRegister(HT_MOTOR1_MODE, MOTOR_MODE_POWER);
    writeRegister(HT_MOTOR1_POWER, (uint8_t)powerToWrite);
  }
  
  // Handle Motor 2 freewheel period
  if (_motor2Freewheeling) {
    // Check if 1 second has elapsed
    if (currentTime - _motor2FreewheelStartTime >= 1000) {
      _motor2Freewheeling = false;
      // Now start controlled deceleration from current (lower) speed
    } else {
      // Still freewheeling, don't do normal ramping
      stillRamping = true;
    }
  }
  
  // Ramp Motor 2 (only if not freewheeling)
  if (!_motor2Freewheeling && _motor2CurrentPower != _motor2TargetPower) {
    stillRamping = true;
    
    // Determine if we're accelerating (increasing power magnitude) or decelerating (decreasing)
    int8_t currentAbs = abs(_motor2CurrentPower);
    int8_t targetAbs = abs(_motor2TargetPower);
    uint8_t rampRate = (targetAbs > currentAbs) ? _acceleration : _deceleration;
    
    if (_motor2CurrentPower < _motor2TargetPower) {
      _motor2CurrentPower += rampRate;
      if (_motor2CurrentPower > _motor2TargetPower) {
        _motor2CurrentPower = _motor2TargetPower;
      }
    } else {
      _motor2CurrentPower -= rampRate;
      if (_motor2CurrentPower < _motor2TargetPower) {
        _motor2CurrentPower = _motor2TargetPower;
      }
    }
    
    // If decelerating and power drops below threshold, switch to freewheel
    int8_t powerToWrite = _motor2CurrentPower;
    if (targetAbs < currentAbs && abs(_motor2CurrentPower) < FREEWHEEL_THRESHOLD && _motor2TargetPower == 0) {
      powerToWrite = MOTOR_FLOAT;  // -128 = freewheel mode
      _motor2CurrentPower = 0;     // Consider it stopped for tracking purposes
    }
    
    writeRegister(HT_MOTOR2_MODE, MOTOR_MODE_POWER);
    writeRegister(HT_MOTOR2_POWER, (uint8_t)powerToWrite);
  }
  
  return stillRamping;
}

// Set acceleration rate for smooth power changes
void HiTechnicMotor::setAcceleration(uint8_t acceleration) {
  _acceleration = constrain(acceleration, 1, 100);
}

// Set deceleration rate for smooth power changes
void HiTechnicMotor::setDeceleration(uint8_t deceleration) {
  _deceleration = constrain(deceleration, 1, 100);
}

// Get current target power
int8_t HiTechnicMotor::getTargetPower(uint8_t motor) {
  if (motor == MOTOR_1) {
    return _motor1TargetPower;
  } else if (motor == MOTOR_2) {
    return _motor2TargetPower;
  }
  return 0;
}

// Get current actual power
int8_t HiTechnicMotor::getCurrentPower(uint8_t motor) {
  if (motor == MOTOR_1) {
    return _motor1CurrentPower;
  } else if (motor == MOTOR_2) {
    return _motor2CurrentPower;
  }
  return 0;
}

// Set motor mode
void HiTechnicMotor::setMotorMode(uint8_t motor, uint8_t mode) {
  if (motor == MOTOR_1 || motor == MOTOR_BOTH) {
    writeRegister(HT_MOTOR1_MODE, mode);
  }
  
  if (motor == MOTOR_2 || motor == MOTOR_BOTH) {
    writeRegister(HT_MOTOR2_MODE, mode);
  }
}

// Stop specified motor
void HiTechnicMotor::stopMotor(uint8_t motor) {
  setMotorPower(motor, 0);
}

// Stop all motors
void HiTechnicMotor::stopAll() {
  setMotorPower(MOTOR_BOTH, 0);
}

// Reset encoder
void HiTechnicMotor::resetEncoder(uint8_t motor) {
  if (motor == MOTOR_1) {
    writeRegister(HT_MOTOR1_MODE, MOTOR_MODE_RESET_ENCODER);
    delay(10);
    writeRegister(HT_MOTOR1_MODE, MOTOR_MODE_POWER);
  } else if (motor == MOTOR_2) {
    writeRegister(HT_MOTOR2_MODE, MOTOR_MODE_RESET_ENCODER);
    delay(10);
    writeRegister(HT_MOTOR2_MODE, MOTOR_MODE_POWER);
  }
}

// Reset all encoders
void HiTechnicMotor::resetAllEncoders() {
  resetEncoder(MOTOR_1);
  resetEncoder(MOTOR_2);
}

// Read encoder value
int32_t HiTechnicMotor::readEncoder(uint8_t motor) {
  if (motor == MOTOR_1) {
    return readRegister32(HT_ENCODER1_CURRENT);
  } else if (motor == MOTOR_2) {
    return readRegister32(HT_ENCODER2_CURRENT);
  }
  return 0;
}

// Set target position for position control mode
void HiTechnicMotor::setTargetPosition(uint8_t motor, int32_t target) {
  if (motor == MOTOR_1) {
    writeRegister32(HT_ENCODER1_TARGET, target);
  } else if (motor == MOTOR_2) {
    writeRegister32(HT_ENCODER2_TARGET, target);
  }
}

// Read firmware version
uint8_t HiTechnicMotor::readVersion() {
  return readRegister(HT_MOTOR_VERSION);
}

// Change I2C address
bool HiTechnicMotor::setI2CAddress(uint8_t newAddress) {
  // Validate address range
  if (newAddress < 0x02 || newAddress > 0x7F) {
    return false;
  }
  
  // Write new address to register 0x70
  writeRegister(HT_MOTOR_I2C_ADDRESS, newAddress);
  delay(100); // Allow time for change to take effect
  
  // Update internal address
  _address = newAddress;
  
  return true;
}

// Get current I2C address
uint8_t HiTechnicMotor::getI2CAddress() {
  return _address;
}

// Check if motor is at target position
bool HiTechnicMotor::isAtTarget(uint8_t motor, int32_t tolerance) {
  int32_t current = readEncoder(motor);
  int32_t target;
  
  if (motor == MOTOR_1) {
    target = readRegister32(HT_ENCODER1_TARGET);
  } else if (motor == MOTOR_2) {
    target = readRegister32(HT_ENCODER2_TARGET);
  } else {
    return false;
  }
  
  return abs(current - target) <= tolerance;
}

// Write single byte to register
void HiTechnicMotor::writeRegister(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(_address);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
  delay(1); // Small delay for I2C
}

// Write 32-bit value to register (big-endian)
void HiTechnicMotor::writeRegister32(uint8_t reg, int32_t value) {
  Wire.beginTransmission(_address);
  Wire.write(reg);
  Wire.write((uint8_t)((value >> 24) & 0xFF)); // MSB
  Wire.write((uint8_t)((value >> 16) & 0xFF));
  Wire.write((uint8_t)((value >> 8) & 0xFF));
  Wire.write((uint8_t)(value & 0xFF));         // LSB
  Wire.endTransmission();
  delay(1);
}

// Read single byte from register
uint8_t HiTechnicMotor::readRegister(uint8_t reg) {
  Wire.beginTransmission(_address);
  Wire.write(reg);
  Wire.endTransmission();
  
  Wire.requestFrom(_address, (uint8_t)1);
  if (Wire.available()) {
    return Wire.read();
  }
  return 0;
}

// Read 32-bit value from register (big-endian)
int32_t HiTechnicMotor::readRegister32(uint8_t reg) {
  Wire.beginTransmission(_address);
  Wire.write(reg);
  Wire.endTransmission();
  
  Wire.requestFrom(_address, (uint8_t)4);
  
  int32_t value = 0;
  if (Wire.available() >= 4) {
    value = (int32_t)Wire.read() << 24; // MSB
    value |= (int32_t)Wire.read() << 16;
    value |= (int32_t)Wire.read() << 8;
    value |= (int32_t)Wire.read();      // LSB
  }
  
  return value;
}
