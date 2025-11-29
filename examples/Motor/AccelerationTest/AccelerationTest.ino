/*
  Simple Acceleration Test
  
  Tests smooth acceleration on a single motor with debug output.
*/

#include "HiTechnicMotor.h"

#define ANALOG_DETECT_PIN 22

HiTechnicMotor motor(0x01);

void setup() {
  Serial.begin(115200);
  Serial.println("=== Acceleration Test ===");
  
  // Enable analog detection
  pinMode(ANALOG_DETECT_PIN, OUTPUT);
  digitalWrite(ANALOG_DETECT_PIN, HIGH);
  delay(100);
  
  motor.begin();
  motor.setAcceleration(1);  // 1 = ~2 second ramp to 100%
  
  Serial.println("Motor initialized");
  Serial.println("Starting 2-second ramp test...\n");
  
  // Set target to 50%
  Serial.println("Setting target to 50% (should take ~1 second)...");
  motor.setMotorPowerSmooth(MOTOR_1, 50, 1);
  
  // Update with detailed output
  int count = 0;
  while (motor.update()) {
    count++;
    Serial.print("Step ");
    Serial.print(count);
    Serial.print(": Current = ");
    Serial.print(motor.getCurrentPower(MOTOR_1));
    Serial.print("%, Target = ");
    Serial.print(motor.getTargetPower(MOTOR_1));
    Serial.println("%");
    delay(50);  // Longer delay to see it happen
  }
  
  Serial.println("\nTarget reached!");
  Serial.print("Final power: ");
  Serial.println(motor.getCurrentPower(MOTOR_1));
  
  delay(3000);
  
  Serial.println("\n=== Testing Freewheel Transition ===");
  Serial.println("Motor will freewheel for 1 second before controlled deceleration");
  Serial.println("Stopping motor...");
  motor.setMotorPowerSmooth(MOTOR_1, 0, 1);
  
  Serial.println("Phase 1: FREEWHEELING for 1 second...");
  unsigned long freewheelStart = millis();
  
  count = 0;
  while (motor.update()) {
    count++;
    unsigned long elapsed = millis() - freewheelStart;
    
    Serial.print("Step ");
    Serial.print(count);
    Serial.print(": Current = ");
    Serial.print(motor.getCurrentPower(MOTOR_1));
    Serial.print("%, Time = ");
    Serial.print(elapsed);
    Serial.print("ms");
    
    if (elapsed < 1000) {
      Serial.print(" [FREEWHEELING]");
    } else if (abs(motor.getCurrentPower(MOTOR_1)) < 25 && motor.getCurrentPower(MOTOR_1) > 0) {
      Serial.print(" [Controlled decel - approaching final freewheel]");
    } else if (motor.getCurrentPower(MOTOR_1) == 0) {
      Serial.print(" [FINAL FREEWHEEL]");
    } else {
      Serial.print(" [Controlled deceleration]");
    }
    Serial.println();
    
    delay(50);
  }
  
  Serial.println("\nMotor stopped!");
}

void loop() {
  // Nothing - test runs once in setup
  delay(1000);
}
