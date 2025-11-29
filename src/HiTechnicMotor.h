/*
  HiTechnicMotor.h - Library for controlling HiTechnic TETRIX Motor Controllers
  via I2C on Arduino Mega
  
  Compatible with HiTechnic DC Motor Controller (NMO1038)
  
  Default I2C Address: 0x02 (can be changed via jumpers)
  
  Created: November 2025
*/

#ifndef HiTechnicMotor_h
#define HiTechnicMotor_h

#include "Arduino.h"
#include <Wire.h>

// Register addresses for HiTechnic Motor Controller
#define HT_MOTOR_VERSION      0x00  // Version number
#define HT_MOTOR_MANUFACTURER 0x08  // Manufacturer
#define HT_MOTOR_SENSOR_TYPE  0x10  // Sensor type
#define HT_MOTOR_I2C_ADDRESS  0x70  // I2C address change register
#define HT_MOTOR_MODE         0x41  // Motor mode
#define HT_MOTOR1_POWER       0x45  // Motor 1 power (-100 to 100)
#define HT_MOTOR2_POWER       0x46  // Motor 2 power (-100 to 100)
#define HT_MOTOR1_MODE        0x44  // Motor 1 mode
#define HT_MOTOR2_MODE        0x47  // Motor 2 mode
#define HT_ENCODER1_TARGET    0x48  // Motor 1 encoder target (4 bytes)
#define HT_ENCODER2_TARGET    0x4C  // Motor 2 target (4 bytes)
#define HT_ENCODER1_CURRENT   0x50  // Motor 1 current encoder (4 bytes)
#define HT_ENCODER2_CURRENT   0x54  // Motor 2 current encoder (4 bytes)

// Motor selection
#define MOTOR_1 1
#define MOTOR_2 2
#define MOTOR_BOTH 3

// Motor modes
#define MOTOR_MODE_POWER      0x00  // Power control mode
#define MOTOR_MODE_SPEED      0x01  // Speed control mode (requires encoders)
#define MOTOR_MODE_POSITION   0x02  // Position control mode (requires encoders)
#define MOTOR_MODE_RESET_ENCODER 0x04 // Reset encoder

// Motor direction
#define MOTOR_FORWARD  1
#define MOTOR_REVERSE -1
#define MOTOR_BRAKE    0

class HiTechnicMotor {
  public:
    // Constructor - specify I2C address (default 0x02)
    HiTechnicMotor(uint8_t address = 0x02);
    
    // Initialize the motor controller
    void begin();
    
    // Set motor power (-100 to 100, negative = reverse)
    void setMotorPower(uint8_t motor, int8_t power);
    
    // Set motor mode (MOTOR_MODE_POWER, MOTOR_MODE_SPEED, or MOTOR_MODE_POSITION)
    void setMotorMode(uint8_t motor, uint8_t mode);
    
    // Stop motor (applies brake)
    void stopMotor(uint8_t motor);
    
    // Stop all motors
    void stopAll();
    
    // Reset encoder for specified motor
    void resetEncoder(uint8_t motor);
    
    // Reset all encoders
    void resetAllEncoders();
    
    // Read current encoder value
    int32_t readEncoder(uint8_t motor);
    
    // Set target encoder position (for position mode)
    void setTargetPosition(uint8_t motor, int32_t target);
    
    // Read firmware version
    uint8_t readVersion();
    
    // Check if motor is at target position
    bool isAtTarget(uint8_t motor, int32_t tolerance = 10);
    
    // Change I2C address (WARNING: Changes persist after power cycle!)
    // newAddress should be 0x02 to 0x10 (even numbers recommended)
    // Returns true if successful
    bool setI2CAddress(uint8_t newAddress);
    
    // Get current I2C address
    uint8_t getI2CAddress();
    
  private:
    uint8_t _address;
    
    // I2C communication helpers
    void writeRegister(uint8_t reg, uint8_t value);
    void writeRegister32(uint8_t reg, int32_t value);
    uint8_t readRegister(uint8_t reg);
    int32_t readRegister32(uint8_t reg);
};

#endif
