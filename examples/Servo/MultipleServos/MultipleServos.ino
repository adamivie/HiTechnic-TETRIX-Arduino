/*
  Multiple Servos Example
  
  This example demonstrates controlling multiple servos simultaneously
  on the HiTechnic TETRIX Servo Controller.
  
  The servo controller can control up to 6 servos.
*/

#include "HiTechnicServo.h"

HiTechnicServo servoController;

void setup() {
  Serial.begin(9600);
  Serial.println("HiTechnic Servo Controller - Multiple Servos Example");
  
  servoController.begin();
  
  // Set moderate speed
  servoController.setStepTime(5);
  
  delay(1000);
}

void loop() {
  // Center all servos
  Serial.println("Centering all servos");
  servoController.centerAll();
  delay(2000);
  
  // Move servos in sequence
  Serial.println("Sequential movement");
  for (uint8_t i = 1; i <= 6; i++) {
    Serial.print("Moving servo ");
    Serial.println(i);
    servoController.setServoAngle(i, 0);
    delay(500);
    servoController.setServoAngle(i, 180);
    delay(500);
    servoController.centerServo(i);
    delay(500);
  }
  
  delay(1000);
  
  // Synchronized movement - all servos together
  Serial.println("Synchronized movement");
  for (uint8_t angle = 0; angle <= 180; angle += 10) {
    for (uint8_t servo = 1; servo <= 6; servo++) {
      servoController.setServoAngle(servo, angle);
    }
    delay(100);
  }
  
  delay(500);
  
  // Return to center
  for (uint8_t angle = 180; angle > 0; angle -= 10) {
    for (uint8_t servo = 1; servo <= 6; servo++) {
      servoController.setServoAngle(servo, angle);
    }
    delay(100);
  }
  
  servoController.centerAll();
  delay(2000);
  
  // Wave pattern
  Serial.println("Wave pattern");
  for (int cycle = 0; cycle < 3; cycle++) {
    for (uint8_t servo = 1; servo <= 6; servo++) {
      servoController.setServoAngle(servo, 45);
      delay(200);
    }
    for (uint8_t servo = 1; servo <= 6; servo++) {
      servoController.setServoAngle(servo, 135);
      delay(200);
    }
  }
  
  servoController.centerAll();
  delay(3000);
}
