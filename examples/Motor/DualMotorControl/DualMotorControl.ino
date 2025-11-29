/*
  Dual Motor Control Example
  
  This example demonstrates controlling both motors on the HiTechnic
  TETRIX Motor Controller simultaneously.
  
  Great for robot drive bases with two motors.
  
  Connections:
  - Arduino SDA (Pin 20) -> HiTechnic SDA
  - Arduino SCL (Pin 21) -> HiTechnic SCL
  - Connect GND between Arduino and HiTechnic controller
  - HiTechnic motor controller must be powered separately (12V TETRIX battery)
*/

#include "HiTechnicMotor.h"

HiTechnicMotor motorController;

void setup() {
  Serial.begin(9600);
  Serial.println("HiTechnic Motor Controller - Dual Motor Example");
  
  motorController.begin();
  delay(1000);
}

void loop() {
  // Move forward - both motors at 60%
  Serial.println("Forward");
  motorController.setMotorPower(MOTOR_1, 60);
  motorController.setMotorPower(MOTOR_2, 60);
  delay(2000);
  
  // Stop both motors
  Serial.println("Stop");
  motorController.stopAll();
  delay(1000);
  
  // Move backward - both motors at -60%
  Serial.println("Backward");
  motorController.setMotorPower(MOTOR_1, -60);
  motorController.setMotorPower(MOTOR_2, -60);
  delay(2000);
  
  // Stop
  Serial.println("Stop");
  motorController.stopAll();
  delay(1000);
  
  // Turn right - left motor forward, right motor reverse
  Serial.println("Turn Right");
  motorController.setMotorPower(MOTOR_1, 50);
  motorController.setMotorPower(MOTOR_2, -50);
  delay(1500);
  
  // Stop
  Serial.println("Stop");
  motorController.stopAll();
  delay(1000);
  
  // Turn left - right motor forward, left motor reverse
  Serial.println("Turn Left");
  motorController.setMotorPower(MOTOR_1, -50);
  motorController.setMotorPower(MOTOR_2, 50);
  delay(1500);
  
  // Stop
  Serial.println("Stop");
  motorController.stopAll();
  delay(3000);
}
