/*
  Acceleration Control Example
  
  Demonstrates smooth motor acceleration and deceleration using
  the setMotorPowerSmooth() function.
  
  This example shows how to:
  - Set different acceleration rates
  - Smoothly ramp motors up and down
  - Create smooth starts and stops
  
  Hardware:
  - Arduino Mega 2560
  - HiTechnic DC Motor Controller at address 0x01
  - 10kΩ resistor from Pin 22 to first controller Pin 5 (for daisy chain)
  - Motors connected to controller outputs
  
  Connections:
  - SDA (Pin 20) → All controllers Pin 6
  - SCL (Pin 21) → All controllers Pin 5
  - Pin 22 → 10kΩ resistor → First controller Pin 5
  - GND → All controllers Pin 1/2
*/

#include <HiTechnicMotor.h>

// Create motor controller at address 0x01
HiTechnicMotor motorController(0x01);

void setup() {
  Serial.begin(115200);
  Serial.println("=== HiTechnic Motor - Acceleration Control ===");
  
  // Initialize motor controller
  motorController.begin();
  delay(100);
  
  Serial.println("Motor controller initialized");
  Serial.println("Starting acceleration demos...\n");
}

void loop() {
  // Demo 1: Slow acceleration
  Serial.println("Demo 1: Slow acceleration (rate=5)");
  motorController.setAcceleration(5);  // Slow ramp
  motorController.setMotorPowerSmooth(MOTOR_1, 80, 5);  // Ramp to 80% forward
  
  // Call update() until ramping is complete
  while (motorController.update()) {
    Serial.print("Current Power: ");
    Serial.print(motorController.getCurrentPower(MOTOR_1));
    Serial.print("% → Target: ");
    Serial.print(motorController.getTargetPower(MOTOR_1));
    Serial.println("%");
    delay(50);
  }
  Serial.println("Target reached!\n");
  delay(2000);
  
  // Demo 2: Slow deceleration
  Serial.println("Demo 2: Slow deceleration (rate=5)");
  motorController.setMotorPowerSmooth(MOTOR_1, 0, 5);  // Ramp down to stop
  
  while (motorController.update()) {
    Serial.print("Current Power: ");
    Serial.print(motorController.getCurrentPower(MOTOR_1));
    Serial.println("%");
    delay(50);
  }
  Serial.println("Motor stopped!\n");
  delay(2000);
  
  // Demo 3: Fast acceleration
  Serial.println("Demo 3: Fast acceleration (rate=20)");
  motorController.setAcceleration(20);  // Fast ramp
  motorController.setMotorPowerSmooth(MOTOR_1, -80, 20);  // Ramp to 80% reverse
  
  while (motorController.update()) {
    Serial.print("Current Power: ");
    Serial.print(motorController.getCurrentPower(MOTOR_1));
    Serial.println("%");
    delay(50);
  }
  Serial.println("Target reached!\n");
  delay(2000);
  
  // Demo 4: Fast deceleration
  Serial.println("Demo 4: Fast deceleration (rate=20)");
  motorController.setMotorPowerSmooth(MOTOR_1, 0, 20);
  
  while (motorController.update()) {
    Serial.print("Current Power: ");
    Serial.print(motorController.getCurrentPower(MOTOR_1));
    Serial.println("%");
    delay(50);
  }
  Serial.println("Motor stopped!\n");
  delay(2000);
  
  // Demo 5: Multiple speed changes with smooth transitions
  Serial.println("Demo 5: Multiple speed changes");
  motorController.setAcceleration(10);  // Medium ramp
  
  Serial.println("Ramping to 50%...");
  motorController.setMotorPowerSmooth(MOTOR_BOTH, 50, 10);
  while (motorController.update()) { delay(20); }
  delay(1000);
  
  Serial.println("Ramping to 100%...");
  motorController.setMotorPowerSmooth(MOTOR_BOTH, 100, 10);
  while (motorController.update()) { delay(20); }
  delay(1000);
  
  Serial.println("Ramping to 30%...");
  motorController.setMotorPowerSmooth(MOTOR_BOTH, 30, 10);
  while (motorController.update()) { delay(20); }
  delay(1000);
  
  Serial.println("Ramping to stop...");
  motorController.setMotorPowerSmooth(MOTOR_BOTH, 0, 10);
  while (motorController.update()) { delay(20); }
  
  Serial.println("Demo complete!\n");
  delay(3000);
  
  // Demo 6: Continuous update in loop (non-blocking)
  Serial.println("Demo 6: Non-blocking acceleration");
  Serial.println("Setting target to 60%, continuing with other tasks...");
  motorController.setMotorPowerSmooth(MOTOR_1, 60, 8);
  
  // You can do other things while motor ramps up
  for (int i = 0; i < 50; i++) {
    motorController.update();  // Update motor ramping
    
    // Do other tasks here
    Serial.print("Doing other work... Motor at: ");
    Serial.print(motorController.getCurrentPower(MOTOR_1));
    Serial.println("%");
    
    delay(50);
  }
  
  Serial.println("\nStopping motor...");
  motorController.setMotorPowerSmooth(MOTOR_1, 0, 15);
  while (motorController.update()) { delay(20); }
  
  Serial.println("\n=== All demos complete! Restarting... ===\n");
  delay(5000);
}
