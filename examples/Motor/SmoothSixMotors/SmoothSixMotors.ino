/*
  Smooth Six Motors Demo
  
  Demonstrates smooth acceleration control across all 6 motors
  on 3 daisy-chained HiTechnic DC Motor Controllers.
  
  Creates coordinated, smooth movements with all motors ramping
  up and down together without jerky starts or stops.
  
  Hardware:
  - Arduino Mega 2560
  - 3x HiTechnic DC Motor Controllers (addresses 0x01, 0x02, 0x03)
  - 10kΩ resistor from Pin 22 to first controller Pin 5 (for daisy chain)
  - 6 motors total (2 per controller)
  
  Connections:
  - SDA (Pin 20) → All controllers Pin 6
  - SCL (Pin 21) → All controllers Pin 5
  - Pin 22 → 10kΩ resistor → First controller Pin 5
  - GND → All controllers Pin 1/2
*/

#include "HiTechnicMotor.h"

// Pin 22 provides 5V for analog detection (through 10kΩ resistor to Pin 5 of first controller)
#define ANALOG_DETECT_PIN 22

// Create three motor controllers at addresses 0x01, 0x02, 0x03
HiTechnicMotor controller1(0x01);
HiTechnicMotor controller2(0x02);
HiTechnicMotor controller3(0x03);

void setup() {
  Serial.begin(115200);
  Serial.println("=== Smooth Six Motors Demo ===");
  
  // Enable analog detection for daisy chain addressing
  pinMode(ANALOG_DETECT_PIN, OUTPUT);
  digitalWrite(ANALOG_DETECT_PIN, HIGH);
  delay(100);
  
  // Initialize all motor controllers
  controller1.begin();
  controller2.begin();
  controller3.begin();
  delay(200);
  
  // Set acceleration rate for all controllers (1 = 2 second ramp to 100%)
  controller1.setAcceleration(1);  // Very smooth, 2 second ramp
  controller2.setAcceleration(1);
  controller3.setAcceleration(1);
  
  Serial.println("All 6 motors initialized with 2-second ramp time");
  Serial.println("Starting coordinated movement patterns...\n");
}

void loop() {
  // Pattern 1: All motors ramp up together
  Serial.println("Pattern 1: All motors ramping to 70% forward");
  controller1.setMotorPowerSmooth(MOTOR_BOTH, 70);
  controller2.setMotorPowerSmooth(MOTOR_BOTH, 70);
  controller3.setMotorPowerSmooth(MOTOR_BOTH, 70);
  
  // Update all controllers until ramping complete
  while (controller1.update() | controller2.update() | controller3.update()) {
    delay(20);
  }
  Serial.println("All motors at 70%");
  delay(2000);
  
  // Pattern 2: Smooth stop
  Serial.println("Pattern 2: All motors ramping to stop");
  controller1.setMotorPowerSmooth(MOTOR_BOTH, 0);
  controller2.setMotorPowerSmooth(MOTOR_BOTH, 0);
  controller3.setMotorPowerSmooth(MOTOR_BOTH, 0);
  
  while (controller1.update() | controller2.update() | controller3.update()) {
    delay(20);
  }
  Serial.println("All motors stopped");
  delay(2000);
  
  // Pattern 3: Wave pattern - motors ramp up one at a time
  Serial.println("Pattern 3: Wave pattern startup");
  
  Serial.println("  C1M1 starting...");
  controller1.setMotorPowerSmooth(MOTOR_1, 60);
  while (controller1.update()) { delay(20); }
  delay(300);
  
  Serial.println("  C1M2 starting...");
  controller1.setMotorPowerSmooth(MOTOR_2, 60);
  while (controller1.update()) { delay(20); }
  delay(300);
  
  Serial.println("  C2M1 starting...");
  controller2.setMotorPowerSmooth(MOTOR_1, 60);
  while (controller2.update()) { delay(20); }
  delay(300);
  
  Serial.println("  C2M2 starting...");
  controller2.setMotorPowerSmooth(MOTOR_2, 60);
  while (controller2.update()) { delay(20); }
  delay(300);
  
  Serial.println("  C3M1 starting...");
  controller3.setMotorPowerSmooth(MOTOR_1, 60);
  while (controller3.update()) { delay(20); }
  delay(300);
  
  Serial.println("  C3M2 starting...");
  controller3.setMotorPowerSmooth(MOTOR_2, 60);
  while (controller3.update()) { delay(20); }
  
  Serial.println("All motors at 60%");
  delay(2000);
  
  // Pattern 4: Wave pattern shutdown
  Serial.println("Pattern 4: Wave pattern shutdown");
  
  controller3.setMotorPowerSmooth(MOTOR_2, 0);
  while (controller3.update()) { delay(20); }
  delay(300);
  
  controller3.setMotorPowerSmooth(MOTOR_1, 0);
  while (controller3.update()) { delay(20); }
  delay(300);
  
  controller2.setMotorPowerSmooth(MOTOR_2, 0);
  while (controller2.update()) { delay(20); }
  delay(300);
  
  controller2.setMotorPowerSmooth(MOTOR_1, 0);
  while (controller2.update()) { delay(20); }
  delay(300);
  
  controller1.setMotorPowerSmooth(MOTOR_2, 0);
  while (controller1.update()) { delay(20); }
  delay(300);
  
  controller1.setMotorPowerSmooth(MOTOR_1, 0);
  while (controller1.update()) { delay(20); }
  
  Serial.println("All motors stopped");
  delay(2000);
  
  // Pattern 5: Alternating speeds with smooth transitions
  Serial.println("Pattern 5: Alternating speed patterns");
  
  // Set different speeds for each controller
  Serial.println("  Controllers at different speeds...");
  controller1.setMotorPowerSmooth(MOTOR_BOTH, 40);
  controller2.setMotorPowerSmooth(MOTOR_BOTH, 70);
  controller3.setMotorPowerSmooth(MOTOR_BOTH, 100);
  
  while (controller1.update() | controller2.update() | controller3.update()) {
    delay(20);
  }
  delay(2000);
  
  // Swap speeds smoothly
  Serial.println("  Swapping speeds...");
  controller1.setMotorPowerSmooth(MOTOR_BOTH, 100);
  controller2.setMotorPowerSmooth(MOTOR_BOTH, 40);
  controller3.setMotorPowerSmooth(MOTOR_BOTH, 70);
  
  while (controller1.update() | controller2.update() | controller3.update()) {
    delay(20);
  }
  delay(2000);
  
  // All to medium speed
  Serial.println("  All to medium speed...");
  controller1.setMotorPowerSmooth(MOTOR_BOTH, 50);
  controller2.setMotorPowerSmooth(MOTOR_BOTH, 50);
  controller3.setMotorPowerSmooth(MOTOR_BOTH, 50);
  
  while (controller1.update() | controller2.update() | controller3.update()) {
    delay(20);
  }
  delay(2000);
  
  // Stop all
  Serial.println("  Stopping all motors...");
  controller1.setMotorPowerSmooth(MOTOR_BOTH, 0);
  controller2.setMotorPowerSmooth(MOTOR_BOTH, 0);
  controller3.setMotorPowerSmooth(MOTOR_BOTH, 0);
  
  while (controller1.update() | controller2.update() | controller3.update()) {
    delay(20);
  }
  Serial.println("All motors stopped");
  delay(2000);
  
  // Pattern 6: Opposite directions with smooth reversal
  Serial.println("Pattern 6: Forward/reverse with smooth transitions");
  
  Serial.println("  Controllers 1&2 forward, 3 reverse...");
  controller1.setMotorPowerSmooth(MOTOR_BOTH, 60);
  controller2.setMotorPowerSmooth(MOTOR_BOTH, 60);
  controller3.setMotorPowerSmooth(MOTOR_BOTH, -60);
  
  while (controller1.update() | controller2.update() | controller3.update()) {
    delay(20);
  }
  delay(2000);
  
  Serial.println("  Reversing directions...");
  controller1.setMotorPowerSmooth(MOTOR_BOTH, -60);
  controller2.setMotorPowerSmooth(MOTOR_BOTH, -60);
  controller3.setMotorPowerSmooth(MOTOR_BOTH, 60);
  
  while (controller1.update() | controller2.update() | controller3.update()) {
    delay(20);
  }
  delay(2000);
  
  Serial.println("  Smooth stop...");
  controller1.setMotorPowerSmooth(MOTOR_BOTH, 0);
  controller2.setMotorPowerSmooth(MOTOR_BOTH, 0);
  controller3.setMotorPowerSmooth(MOTOR_BOTH, 0);
  
  while (controller1.update() | controller2.update() | controller3.update()) {
    delay(20);
  }
  Serial.println("All motors stopped");
  
  Serial.println("\n=== All patterns complete! Restarting... ===\n");
  delay(5000);
}
