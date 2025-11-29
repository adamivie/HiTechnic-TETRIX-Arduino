/*
  DiagnosticsTest - Test basic motor control features
  
  This example demonstrates:
  - Smooth acceleration/deceleration
  - Encoder reading
  - Position control
  - Motor status checking
  
  Hardware Setup:
  - Arduino Mega 2560
  - HiTechnic TETRIX Motor Controller at address 0x01
  - 10kΩ resistor from Pin 22 to controller Pin 5
  - I2C: Pin 20 (SDA), Pin 21 (SCL)
*/

#include <HiTechnicMotor.h>

// Create motor controller
HiTechnicMotor motor1(0x01);

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // Wait for serial port to connect
  }
  
  Serial.println(F("HiTechnic Motor Controller - Basic Test"));
  Serial.println(F("========================================\n"));
  
  // Setup Pin 22 for analog detection (required for daisy chain addressing)
  pinMode(22, OUTPUT);
  digitalWrite(22, HIGH);  // 5V to first controller Pin 5 via 10kΩ resistor
  
  // Initialize motor controller
  motor1.begin();
  
  Serial.println(F("Motor controller initialized."));
  Serial.print(F("Firmware version: "));
  Serial.println(motor1.readVersion(), HEX);
  Serial.println();
  delay(1000);
  
  Serial.println(F("=== Test 1: Smooth Acceleration ==="));
  motor1.setMotorPowerSmooth(MOTOR_1, 75, 5);  // 75% power, rate=5
  
  for (int i = 0; i < 30; i++) {
    motor1.update();
    
    Serial.print(F("Target: "));
    Serial.print(motor1.getTargetPower(MOTOR_1));
    Serial.print(F("%, Current: "));
    Serial.print(motor1.getCurrentPower(MOTOR_1));
    Serial.print(F("%, Encoder: "));
    Serial.println(motor1.readEncoder(MOTOR_1));
    
    delay(100);
  }
  
  Serial.println(F("\n=== Test 2: Smooth Deceleration ==="));
  motor1.setMotorPowerSmooth(MOTOR_1, 0, 5);  // Stop with rate=5
  
  for (int i = 0; i < 30; i++) {
    motor1.update();
    
    Serial.print(F("Target: "));
    Serial.print(motor1.getTargetPower(MOTOR_1));
    Serial.print(F("%, Current: "));
    Serial.print(motor1.getCurrentPower(MOTOR_1));
    Serial.print(F("%, Encoder: "));
    Serial.println(motor1.readEncoder(MOTOR_1));
    
    delay(100);
  }
  
  delay(1000);
  
  Serial.println(F("\n=== Test 3: Position Control ==="));
  motor1.resetEncoder(MOTOR_1);
  motor1.setMotorMode(MOTOR_1, MOTOR_MODE_POSITION);
  motor1.setTargetPosition(MOTOR_1, 1440);  // 1 revolution
  motor1.setMotorPower(MOTOR_1, 50);  // Max 50% power
  
  Serial.println(F("Moving to position 1440..."));
  
  unsigned long startTime = millis();
  while (!motor1.isAtTarget(MOTOR_1, 50) && (millis() - startTime < 5000)) {
    int32_t position = motor1.readEncoder(MOTOR_1);
    
    Serial.print(F("Position: "));
    Serial.print(position);
    Serial.print(F(" / 1440"));
    Serial.println();
    
    delay(100);
  }
  
  Serial.println(F("\nPosition reached!"));
  motor1.stopAll();
  
  Serial.println(F("\n=== Test Complete ==="));
  Serial.println(F("All basic features working!"));
}

void loop() {
  // Print encoder values every 2 seconds
  static unsigned long lastPrint = 0;
  
  if (millis() - lastPrint >= 2000) {
    Serial.print(F("Motor 1 Encoder: "));
    Serial.println(motor1.readEncoder(MOTOR_1));
    lastPrint = millis();
  }
  
  delay(100);
}
