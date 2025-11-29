/*
  Basic Servo Control Example
  
  This example demonstrates basic control of servos using
  the HiTechnic TETRIX Servo Controller.
  
  Connections:
  - Arduino SDA (Pin 20) -> HiTechnic Servo SDA
  - Arduino SCL (Pin 21) -> HiTechnic Servo SCL
  - Connect GND between Arduino and HiTechnic controller
  - HiTechnic servo controller must be powered separately (12V TETRIX battery)
  
  Default I2C Address: 0x04 (can be changed with jumpers)
*/

#include "HiTechnicServo.h"

// Create servo controller object with default address (0x04)
HiTechnicServo servoController;

void setup() {
  Serial.begin(9600);
  Serial.println("HiTechnic Servo Controller - Basic Example");
  
  // Initialize the servo controller
  servoController.begin();
  
  // Read and display firmware version
  uint8_t version = servoController.readVersion();
  Serial.print("Controller Version: ");
  Serial.println(version);
  
  // Set servo speed (0=fastest, 15=slowest)
  servoController.setStepTime(5);
  
  delay(1000);
}

void loop() {
  // Move servo 1 to 0 degrees
  Serial.println("Servo 1: 0 degrees");
  servoController.setServoAngle(SERVO_1, 0);
  delay(1000);
  
  // Move servo 1 to 90 degrees (center)
  Serial.println("Servo 1: 90 degrees");
  servoController.setServoAngle(SERVO_1, 90);
  delay(1000);
  
  // Move servo 1 to 180 degrees
  Serial.println("Servo 1: 180 degrees");
  servoController.setServoAngle(SERVO_1, 180);
  delay(1000);
  
  // Center servo
  Serial.println("Servo 1: Center");
  servoController.centerServo(SERVO_1);
  delay(2000);
  
  // Sweep servo 1
  Serial.println("Sweeping servo 1");
  for (uint8_t angle = 0; angle <= 180; angle += 5) {
    servoController.setServoAngle(SERVO_1, angle);
    delay(50);
  }
  
  delay(500);
  
  // Sweep back
  for (uint8_t angle = 180; angle > 0; angle -= 5) {
    servoController.setServoAngle(SERVO_1, angle);
    delay(50);
  }
  
  // Center and pause
  servoController.centerServo(SERVO_1);
  delay(2000);
}
