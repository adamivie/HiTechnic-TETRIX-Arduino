/*
  HiTechnicServo.h - Library for controlling HiTechnic TETRIX Servo Controller
  via I2C on Arduino Mega
  
  Compatible with HiTechnic Servo Controller (NSR1038)
  Supports up to 6 servos per controller
  
  Default I2C Address: 0x04 (can be changed via jumpers)
  
  Created: November 2025
*/

#ifndef HiTechnicServo_h
#define HiTechnicServo_h

#include "Arduino.h"
#include <Wire.h>

// Register addresses for HiTechnic Servo Controller
#define HT_SERVO_VERSION      0x00  // Version number
#define HT_SERVO_MANUFACTURER 0x08  // Manufacturer
#define HT_SERVO_SENSOR_TYPE  0x10  // Sensor type
#define HT_SERVO_STATUS       0x40  // Status register
#define HT_SERVO_STEP_TIME    0x41  // Step time (servo speed)
#define HT_SERVO1_POS         0x42  // Servo 1 position (0-255)
#define HT_SERVO2_POS         0x43  // Servo 2 position
#define HT_SERVO3_POS         0x44  // Servo 3 position
#define HT_SERVO4_POS         0x45  // Servo 4 position
#define HT_SERVO5_POS         0x46  // Servo 5 position
#define HT_SERVO6_POS         0x47  // Servo 6 position

// Servo selection
#define SERVO_1 1
#define SERVO_2 2
#define SERVO_3 3
#define SERVO_4 4
#define SERVO_5 5
#define SERVO_6 6

// Servo position constants
#define SERVO_MIN_POS 0
#define SERVO_MAX_POS 255
#define SERVO_CENTER  127

class HiTechnicServo {
  public:
    // Constructor - specify I2C address (default 0x04)
    HiTechnicServo(uint8_t address = 0x04);
    
    // Initialize the servo controller
    void begin();
    
    // Set servo position (0-255, where 127 is typically center)
    void setServoPosition(uint8_t servo, uint8_t position);
    
    // Set servo position using angle in degrees (0-180)
    void setServoAngle(uint8_t servo, uint8_t angle);
    
    // Set step time (servo speed) - lower = faster, 0 = full speed
    // Range: 0-15, where 0 is fastest and 15 is slowest
    void setStepTime(uint8_t stepTime);
    
    // Get current servo position
    uint8_t getServoPosition(uint8_t servo);
    
    // Center a specific servo (set to 127)
    void centerServo(uint8_t servo);
    
    // Center all servos
    void centerAll();
    
    // Read firmware version
    uint8_t readVersion();
    
    // Read status register
    uint8_t readStatus();
    
    // Disable servo (set to 255 - no pulse)
    void disableServo(uint8_t servo);
    
    // Enable servo (restore last position or center)
    void enableServo(uint8_t servo);
    
  private:
    uint8_t _address;
    uint8_t _servoPositions[6]; // Track last known positions
    
    // I2C communication helpers
    void writeRegister(uint8_t reg, uint8_t value);
    uint8_t readRegister(uint8_t reg);
    uint8_t getServoRegister(uint8_t servo);
};

#endif
