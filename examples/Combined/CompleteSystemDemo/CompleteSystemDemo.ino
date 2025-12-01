/*
  Complete System Demo - Motors & S// Acceleration parameters
const uint8_t ACCEL_RATE = 10;       // Acceleration rate (1-100) - tuned for ~2 second ramp
const uint8_t TARGET_SPEED = 100;    // Full speed
const unsigned long COAST_TIME = 1500; // Coast for 1.5 seconds
const unsigned long PAUSE_TIME = 500;  // Short pause between motors
  
  This example demonstrates full control of a complete HiTechnic TETRIX system:
  - 3 Motor Controllers (6 DC motors total) with smooth acceleration
  - 1 Servo Controller (6 servos)
  
  MOTOR SEQUENCE:
  For each motor (1-6):
  1. Smoothly accelerate to full speed forward
  2. Coast to a stop
  3. Small pause
  
  SERVO SEQUENCE:
  After all motors complete:
  1. Center all servos (90°)
  2. Move from minimum (0°) to maximum (180°)
  3. Return to center
  
  HARDWARE REQUIREMENTS:
  - 3× HiTechnic Motor Controllers (NMO1038) at 0x01, 0x02, 0x03
  - 1× HiTechnic Servo Controller (NSR1038) at 0x04
  - Pin 22 HIGH via 10kΩ resistor to first controller
  
  Created: November 29, 2025
  Part of HiTechnic TETRIX Arduino Library
*/

#include <HiTechnicMotor.h>
#include <HiTechnicServo.h>

// Motor Controllers (3 controllers × 2 motors = 6 motors total)
HiTechnicMotor motor1(0x01);  // Position 1 in chain
HiTechnicMotor motor2(0x02);  // Position 2 in chain
HiTechnicMotor motor3(0x03);  // Position 3 in chain

// Servo Controller (6 servos)
HiTechnicServo servo1(0x04);  // Position 4 in chain

// Acceleration parameters
const uint8_t ACCEL_RATE = 1;        // Acceleration rate (1-100) - 1 = slowest, smoothest (2 seconds 0→100)
const uint8_t TARGET_SPEED = 100;    // Full speed
const unsigned long COAST_TIME = 2000; // Coast for 2 seconds
const unsigned long PAUSE_TIME = 1000; // Pause between motors

void setup() {
  Serial.begin(9600);
  delay(1000);
  
  Serial.println("========================================");
  Serial.println("  Complete System Demo");
  Serial.println("  Motors + Servos");
  Serial.println("========================================");
  Serial.println();
  
  // CRITICAL: Enable Pin 22 for daisy chain addressing
  pinMode(22, OUTPUT);
  digitalWrite(22, HIGH);
  Serial.println("[✓] Pin 22 enabled for daisy chain");
  
  // Initialize all motor controllers
  motor1.begin();
  motor2.begin();
  motor3.begin();
  Serial.println("[✓] Motor controllers initialized");
  
  // Initialize servo controller
  servo1.begin();
  Serial.println("[✓] Servo controller initialized");
  
  // Set acceleration rate for all motor controllers
  motor1.setAcceleration(ACCEL_RATE);
  motor2.setAcceleration(ACCEL_RATE);
  motor3.setAcceleration(ACCEL_RATE);
  Serial.print("[✓] Acceleration rate set to ");
  Serial.println(ACCEL_RATE);
  
  Serial.println();
  Serial.println("System ready!");
  Serial.println();
  delay(2000);
}

// Helper function to wait for user input
void waitForUser(const char* message) {
  Serial.println();
  Serial.print(message);
  Serial.println(" (Press Enter to continue)");
  
  // Wait for any input
  while (Serial.available() == 0) {
    delay(100);
  }
  
  // Clear the input buffer
  while (Serial.available() > 0) {
    Serial.read();
  }
  
  Serial.println();
}

void loop() {
  Serial.println("========================================");
  Serial.println("  INTERACTIVE MOTOR DEMONSTRATION");
  Serial.println("========================================");
  Serial.println();
  
  waitForUser("Ready to test motors?");
  
  // Test each motor individually with prompts
  testMotor(motor1, 1, 1, "Controller 1, Motor 1");
  waitForUser("Continue to next motor?");
  
  testMotor(motor1, 2, 2, "Controller 1, Motor 2");
  waitForUser("Continue to next motor?");
  
  testMotor(motor2, 1, 3, "Controller 2, Motor 1");
  waitForUser("Continue to next motor?");
  
  testMotor(motor2, 2, 4, "Controller 2, Motor 2");
  waitForUser("Continue to next motor?");
  
  testMotor(motor3, 1, 5, "Controller 3, Motor 1");
  waitForUser("Continue to next motor?");
  
  testMotor(motor3, 2, 6, "Controller 3, Motor 2");
  
  Serial.println("========================================");
  Serial.println("  SERVO DEMONSTRATION");
  Serial.println("========================================");
  Serial.println();
  
  waitForUser("Ready to test servos?");
  
  // Center all servos first
  Serial.println("Centering all servos...");
  for (int i = 1; i <= 6; i++) {
    servo1.centerServo(i);
  }
  delay(1500);
  Serial.println("[✓] All servos centered");
  Serial.println();
  
  // Test each servo with full range motion
  for (int i = 1; i <= 6; i++) {
    testServo(i);
  }
  
  Serial.println("========================================");
  Serial.println("  Demo Complete!");
  Serial.println("========================================");
  Serial.println();
  
  waitForUser("Press Enter to run demo again");
}

void testMotor(HiTechnicMotor& controller, uint8_t motorNum, uint8_t displayNum, const char* name) {
  Serial.println("----------------------------------------");
  Serial.print("Testing Motor ");
  Serial.print(displayNum);
  Serial.print(" (");
  Serial.print(name);
  Serial.println(")");
  Serial.println("----------------------------------------");
  
  // Phase 1: Ramp forward and test float mode
  waitForUser("Ready to start Phase 1 (Accel forward 0→100%)?");
  
  Serial.println("Phase 1: Ramping forward 0% → 100% (2 seconds)...");
  for (int power = 0; power <= TARGET_SPEED; power++) {
    controller.setMotorPower(motorNum, power);
    if (power % 20 == 0) {
      Serial.print("  Power: ");
      Serial.print(power);
      Serial.println("%");
    }
    delay(20);
  }
  Serial.println("  [✓] 100% forward reached");
  Serial.println();
  
  // Phase 2: Run at full speed, then test float mode from forward direction
  waitForUser("Ready for Phase 2 (Run at 100%, then float)?");
  
  Serial.println("Phase 2: Running at 100% forward for 2 seconds...");
  delay(2000);
  
  Serial.println("  Testing FLOAT MODE from forward direction...");
  Serial.println("  Setting power to -128 (float/coast)...");
  controller.setMotorPower(motorNum, -128);
  Serial.println("  Motor should coast freely for 3 seconds...");
  delay(3000);
  Serial.println("  [✓] Float mode test complete (forward)");
  Serial.println();
  
  // Phase 3: Ramp reverse and test float mode
  waitForUser("Ready for Phase 3 (Accel reverse 0→-100%)?");
  
  Serial.println("Phase 3: Ramping reverse 0% → -100% (2 seconds)...");
  for (int power = 0; power >= -TARGET_SPEED; power--) {
    controller.setMotorPower(motorNum, power);
    if (power % 20 == 0) {
      Serial.print("  Power: ");
      Serial.print(power);
      Serial.println("%");
    }
    delay(20);
  }
  Serial.println("  [✓] 100% reverse reached");
  Serial.println();
  
  // Phase 4: Run at full reverse, then test float mode from reverse direction
  waitForUser("Ready for Phase 4 (Run at -100%, then float)?");
  
  Serial.println("Phase 4: Running at -100% reverse for 2 seconds...");
  delay(2000);
  
  Serial.println("  Testing FLOAT MODE from reverse direction...");
  Serial.println("  Setting power to -128 (float/coast)...");
  controller.setMotorPower(motorNum, -128);
  Serial.println("  Motor should coast freely for 3 seconds...");
  delay(3000);
  Serial.println("  [✓] Float mode test complete (reverse)");
  Serial.println();
  
  // Small pause before next motor
  Serial.print("Pausing ");
  Serial.print(PAUSE_TIME / 1000.0, 1);
  Serial.println(" seconds...");
  delay(PAUSE_TIME);
  Serial.println();
}

void testServo(int servoNum) {
  Serial.println("----------------------------------------");
  Serial.print("Testing Servo ");
  Serial.println(servoNum);
  Serial.println("----------------------------------------");
  
  // Set fast step time for quick movements
  servo1.setStepTime(0);  // 0 = fastest movement
  
  // Start from center
  Serial.println("Starting from center (90°)");
  servo1.setServoAngle(servoNum, 90);
  delay(400);
  
  // Move to minimum extreme (0°) - FAST
  Serial.println("Moving to minimum (0°)... [FAST]");
  servo1.setServoAngle(servoNum, 0);
  delay(500);
  Serial.println("  [✓] Reached 0°");
  
  // Sweep to maximum extreme (180°) - FAST
  Serial.println("Sweeping to maximum (180°)... [FAST]");
  servo1.setServoAngle(servoNum, 180);
  delay(600);
  Serial.println("  [✓] Reached 180°");
  
  // Return to center - FAST
  Serial.println("Returning to center (90°)... [FAST]");
  servo1.setServoAngle(servoNum, 90);
  delay(400);
  Serial.println("  [✓] Centered");
  Serial.println();
  
  delay(300);  // Small pause between servos
}
