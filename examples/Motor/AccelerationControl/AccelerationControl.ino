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

// Pin 22 provides 5V for analog detection (through 10kΩ resistor to Pin 5 of first controller)
#define ANALOG_DETECT_PIN 22

// Create motor controller at address 0x01
HiTechnicMotor motorController(0x01);

void setup() {
  Serial.begin(115200);
  Serial.println("=== HiTechnic Motor - Acceleration Control ===");
  
  // Enable analog detection for daisy chain addressing
  pinMode(ANALOG_DETECT_PIN, OUTPUT);
  digitalWrite(ANALOG_DETECT_PIN, HIGH);
  delay(100);
  
  // Initialize motor controller
  motorController.begin();
  delay(100);
  
  Serial.println("Motor controller initialized");
  Serial.println("Starting acceleration demos...\n");
}

void loop() {
  // Demo 1: Very slow acceleration (2 second ramp)
  Serial.println("Demo 1: Very slow acceleration (rate=1, ~2 second ramp)");
  motorController.setAcceleration(1);  // 2 second ramp to 100%
  motorController.setMotorPowerSmooth(MOTOR_1, 80, 1);  // Ramp to 80% forward (~1.6 sec)
  
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
  
  // Demo 2: Very slow deceleration (2 second ramp)
  Serial.println("Demo 2: Very slow deceleration (rate=1, ~1.6 second ramp)");
  motorController.setMotorPowerSmooth(MOTOR_1, 0, 1);  // Ramp down to stop
  
  while (motorController.update()) {
    Serial.print("Current Power: ");
    Serial.print(motorController.getCurrentPower(MOTOR_1));
    Serial.println("%");
    delay(50);
  }
  Serial.println("Motor stopped!\n");
  delay(2000);
  
  // Demo 3: Medium acceleration
  Serial.println("Demo 3: Medium acceleration (rate=5, ~0.5 second ramp)");
  motorController.setAcceleration(5);  // Medium ramp
  motorController.setMotorPowerSmooth(MOTOR_1, -80, 5);  // Ramp to 80% reverse
  
  while (motorController.update()) {
    Serial.print("Current Power: ");
    Serial.print(motorController.getCurrentPower(MOTOR_1));
    Serial.println("%");
    delay(50);
  }
  Serial.println("Target reached!\n");
  delay(2000);
  
  // Demo 4: Medium deceleration
  Serial.println("Demo 4: Medium deceleration (rate=5)");
  motorController.setMotorPowerSmooth(MOTOR_1, 0, 5);
  
  while (motorController.update()) {
    Serial.print("Current Power: ");
    Serial.print(motorController.getCurrentPower(MOTOR_1));
    Serial.println("%");
    delay(50);
  }
  Serial.println("Motor stopped!\n");
  delay(2000);
  
  // Demo 5: Multiple speed changes with smooth transitions
  Serial.println("Demo 5: Multiple speed changes (rate=2, ~2 sec ramps)");
  motorController.setAcceleration(2);  // Slow ramp
  
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
  // Demo 6: Non-blocking acceleration
  Serial.println("Demo 6: Non-blocking acceleration (rate=2)");
  Serial.println("Setting target to 60%, continuing with other tasks...");
  motorController.setMotorPowerSmooth(MOTOR_1, 60, 2);
  
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
  motorController.setMotorPowerSmooth(MOTOR_1, 0, 2);
  while (motorController.update()) { delay(20); }
  
  Serial.println("\n=== All demos complete! Restarting... ===\n");
  delay(5000);
}