/*
  Position Control Example
  
  This example demonstrates precise position control using encoders.
  The motor will move to specific encoder positions.
  
  Note: This requires TETRIX motors with encoders attached.
  
  Connections:
  - Arduino SDA (Pin 20) -> HiTechnic SDA
  - Arduino SCL (Pin 21) -> HiTechnic SCL
  - Connect GND between Arduino and HiTechnic controller
  - HiTechnic motor controller must be powered separately (12V TETRIX battery)
  - TETRIX motors with encoders connected
*/

#include "HiTechnicMotor.h"

HiTechnicMotor motorController;

void setup() {
  Serial.begin(9600);
  Serial.println("HiTechnic Motor Controller - Position Control Example");
  
  motorController.begin();
  motorController.resetAllEncoders();
  
  delay(1000);
}

void loop() {
  // Move to position 1000
  moveToPosition(MOTOR_1, 1000, 80);
  delay(1000);
  
  // Move to position 2000
  moveToPosition(MOTOR_1, 2000, 80);
  delay(1000);
  
  // Move back to position 500
  moveToPosition(MOTOR_1, 500, 80);
  delay(1000);
  
  // Return to start (0)
  moveToPosition(MOTOR_1, 0, 80);
  delay(3000);
}

// Function to move motor to a specific encoder position
void moveToPosition(uint8_t motor, int32_t targetPosition, uint8_t power) {
  Serial.print("Moving to position: ");
  Serial.println(targetPosition);
  
  int32_t currentPos = motorController.readEncoder(motor);
  int32_t error = targetPosition - currentPos;
  
  // Determine direction
  int8_t direction = (error > 0) ? 1 : -1;
  
  // Move towards target
  motorController.setMotorPower(motor, direction * power);
  
  // Wait until close to target (within 20 encoder counts)
  while (abs(targetPosition - motorController.readEncoder(motor)) > 20) {
    currentPos = motorController.readEncoder(motor);
    
    // Slow down as we approach target
    error = abs(targetPosition - currentPos);
    if (error < 200) {
      motorController.setMotorPower(motor, direction * 30);
    }
    
    // Print progress
    Serial.print("Current: ");
    Serial.print(currentPos);
    Serial.print(" | Target: ");
    Serial.print(targetPosition);
    Serial.print(" | Error: ");
    Serial.println(targetPosition - currentPos);
    
    delay(50);
  }
  
  // Stop motor
  motorController.stopMotor(motor);
  
  Serial.print("Reached position: ");
  Serial.println(motorController.readEncoder(motor));
}
