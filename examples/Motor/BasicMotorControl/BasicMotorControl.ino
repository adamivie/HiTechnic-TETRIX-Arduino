/*
  Basic Motor Control Example
  
  This example demonstrates basic control of a single motor using
  the HiTechnic TETRIX Motor Controller.
  
  Connections:
  - Arduino SDA (Pin 20) -> HiTechnic SDA
  - Arduino SCL (Pin 21) -> HiTechnic SCL
  - Connect GND between Arduino and HiTechnic controller
  - HiTechnic motor controller must be powered separately (12V TETRIX battery)
  
  Default I2C Address: 0x02
*/

#include "HiTechnicMotor.h"

// Create motor controller object with default address (0x02)
HiTechnicMotor motorController;

void setup() {
  Serial.begin(9600);
  Serial.println("HiTechnic Motor Controller - Basic Example");
  
  // Initialize the motor controller
  motorController.begin();
  
  // Read and display firmware version
  uint8_t version = motorController.readVersion();
  Serial.print("Controller Version: ");
  Serial.println(version);
  
  delay(1000);
}

void loop() {
  // Forward at 50% power
  Serial.println("Motor 1 Forward 50%");
  motorController.setMotorPower(MOTOR_1, 50);
  delay(2000);
  
  // Stop
  Serial.println("Motor 1 Stop");
  motorController.stopMotor(MOTOR_1);
  delay(1000);
  
  // Reverse at 75% power
  Serial.println("Motor 1 Reverse 75%");
  motorController.setMotorPower(MOTOR_1, -75);
  delay(2000);
  
  // Stop
  Serial.println("Motor 1 Stop");
  motorController.stopMotor(MOTOR_1);
  delay(1000);
  
  // Full power forward
  Serial.println("Motor 1 Forward 100%");
  motorController.setMotorPower(MOTOR_1, 100);
  delay(2000);
  
  // Stop
  Serial.println("Motor 1 Stop");
  motorController.stopMotor(MOTOR_1);
  delay(3000);
}
