/*
  HiTechnicServo.cpp - Library for controlling HiTechnic TETRIX Servo Controller
  via I2C on Arduino Mega
*/

#include "HiTechnicServo.h"

// Constructor
HiTechnicServo::HiTechnicServo(uint8_t address) {
  _address = address;
  // Initialize position tracking to center
  for (int i = 0; i < 6; i++) {
    _servoPositions[i] = SERVO_CENTER;
  }
}

// Initialize the servo controller
void HiTechnicServo::begin() {
  Wire.begin();
  delay(100); // Allow controller to initialize
  
  // Set step time to moderate speed (5)
  setStepTime(5);
  
  // Center all servos
  centerAll();
}

// Set servo position (0-255)
void HiTechnicServo::setServoPosition(uint8_t servo, uint8_t position) {
  if (servo < 1 || servo > 6) return;
  
  // Constrain position to valid range
  position = constrain(position, SERVO_MIN_POS, SERVO_MAX_POS);
  
  uint8_t reg = getServoRegister(servo);
  writeRegister(reg, position);
  
  // Update tracking
  _servoPositions[servo - 1] = position;
}

// Set servo position using angle (0-180 degrees)
void HiTechnicServo::setServoAngle(uint8_t servo, uint8_t angle) {
  // Constrain angle to 0-180
  angle = constrain(angle, 0, 180);
  
  // Map angle to position (0-180 -> 0-255)
  uint8_t position = map(angle, 0, 180, SERVO_MIN_POS, SERVO_MAX_POS);
  
  setServoPosition(servo, position);
}

// Set step time (servo movement speed)
void HiTechnicServo::setStepTime(uint8_t stepTime) {
  // Constrain to valid range
  stepTime = constrain(stepTime, 0, 15);
  writeRegister(HT_SERVO_STEP_TIME, stepTime);
}

// Get current servo position
uint8_t HiTechnicServo::getServoPosition(uint8_t servo) {
  if (servo < 1 || servo > 6) return 127;
  
  uint8_t reg = getServoRegister(servo);
  return readRegister(reg);
}

// Center a specific servo
void HiTechnicServo::centerServo(uint8_t servo) {
  setServoPosition(servo, SERVO_CENTER);
}

// Center all servos
void HiTechnicServo::centerAll() {
  for (uint8_t i = 1; i <= 6; i++) {
    centerServo(i);
  }
}

// Read firmware version
uint8_t HiTechnicServo::readVersion() {
  return readRegister(HT_SERVO_VERSION);
}

// Read status register
uint8_t HiTechnicServo::readStatus() {
  return readRegister(HT_SERVO_STATUS);
}

// Disable servo (255 = no pulse output)
void HiTechnicServo::disableServo(uint8_t servo) {
  if (servo < 1 || servo > 6) return;
  
  uint8_t reg = getServoRegister(servo);
  writeRegister(reg, 255);
}

// Enable servo (restore last position)
void HiTechnicServo::enableServo(uint8_t servo) {
  if (servo < 1 || servo > 6) return;
  
  // Restore last known position
  setServoPosition(servo, _servoPositions[servo - 1]);
}

// Get register address for servo number
uint8_t HiTechnicServo::getServoRegister(uint8_t servo) {
  switch(servo) {
    case 1: return HT_SERVO1_POS;
    case 2: return HT_SERVO2_POS;
    case 3: return HT_SERVO3_POS;
    case 4: return HT_SERVO4_POS;
    case 5: return HT_SERVO5_POS;
    case 6: return HT_SERVO6_POS;
    default: return HT_SERVO1_POS;
  }
}

// Write single byte to register
void HiTechnicServo::writeRegister(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(_address);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
  delay(1); // Small delay for I2C
}

// Read single byte from register
uint8_t HiTechnicServo::readRegister(uint8_t reg) {
  Wire.beginTransmission(_address);
  Wire.write(reg);
  Wire.endTransmission();
  
  Wire.requestFrom(_address, (uint8_t)1);
  if (Wire.available()) {
    return Wire.read();
  }
  return 0;
}
