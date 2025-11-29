/*
  Encoder Reading Example
  
  This example demonstrates reading encoder values from motors.
  Encoders are useful for precise position control and measuring distance.
  
  Note: This requires TETRIX motors with encoders attached.
  
  Connections:
  - Arduino SDA (Pin 20) -> HiTechnic SDA
  - Arduino SCL (Pin 21) -> HiTechnic SCL
  - Connect GND between Arduino and HiTechnic controller
  - HiTechnic motor controller must be powered separately (12V TETRIX battery)
  - TETRIX motors with encoders connected to motor controller
*/

#include "HiTechnicMotor.h"

HiTechnicMotor motorController;

void setup() {
  Serial.begin(9600);
  Serial.println("HiTechnic Motor Controller - Encoder Example");
  
  motorController.begin();
  
  // Reset encoders to zero
  motorController.resetAllEncoders();
  Serial.println("Encoders reset to zero");
  
  delay(1000);
}

void loop() {
  // Run motor 1 forward for 3 seconds
  Serial.println("\n--- Running Motor 1 Forward ---");
  motorController.setMotorPower(MOTOR_1, 70);
  
  // Monitor encoder while running
  for (int i = 0; i < 30; i++) {
    int32_t encoder1 = motorController.readEncoder(MOTOR_1);
    Serial.print("Motor 1 Encoder: ");
    Serial.println(encoder1);
    delay(100);
  }
  
  // Stop and read final encoder value
  motorController.stopMotor(MOTOR_1);
  delay(500);
  
  int32_t finalEncoder = motorController.readEncoder(MOTOR_1);
  Serial.print("Final Encoder Value: ");
  Serial.println(finalEncoder);
  
  delay(2000);
  
  // Run motor 1 backward to return to start
  Serial.println("\n--- Returning to Start ---");
  motorController.setMotorPower(MOTOR_1, -70);
  
  // Run until encoder is close to zero
  while (abs(motorController.readEncoder(MOTOR_1)) > 50) {
    int32_t encoder1 = motorController.readEncoder(MOTOR_1);
    Serial.print("Motor 1 Encoder: ");
    Serial.println(encoder1);
    delay(100);
  }
  
  motorController.stopMotor(MOTOR_1);
  Serial.println("Back at start position");
  
  // Reset encoder for next cycle
  motorController.resetEncoder(MOTOR_1);
  
  delay(3000);
}
