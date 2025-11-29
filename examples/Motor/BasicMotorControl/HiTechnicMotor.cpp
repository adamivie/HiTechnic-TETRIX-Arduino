/*
  HiTechnicMotor.cpp - Library for controlling HiTechnic TETRIX Motor Controllers
  via I2C on Arduino Mega
*/

#include "HiTechnicMotor.h"

// Constructor
HiTechnicMotor::HiTechnicMotor(uint8_t address) {
  _address = address;
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
  
  if (motor == MOTOR_1 || motor == MOTOR_BOTH) {
    writeRegister(HT_MOTOR1_POWER, (uint8_t)power);
  }
  
  if (motor == MOTOR_2 || motor == MOTOR_BOTH) {
    writeRegister(HT_MOTOR2_POWER, (uint8_t)power);
  }
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
