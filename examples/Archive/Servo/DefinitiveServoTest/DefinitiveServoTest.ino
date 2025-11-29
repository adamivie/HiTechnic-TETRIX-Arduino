/*
  Definitive Servo Test - Following HiTechnic Spec EXACTLY
  
  Based on official HiTechnic specification.
  Tests servos on channels 1 and 2 where user confirmed servos are connected.
*/

#include <Wire.h>

#define SERVO_ADDR 0x04

void setup() {
  Serial.begin(9600);
  while (!Serial);
  delay(500);
  
  Serial.println("\n╔════════════════════════════════════════╗");
  Serial.println("║  HiTechnic Servo Controller Test      ║");
  Serial.println("║  Following Official Specification     ║");
  Serial.println("╚════════════════════════════════════════╝");
  Serial.println();
  Serial.println("Servos on Channels 1 & 2");
  Serial.println("Controller Address: 0x04");
  Serial.println();
  
  Wire.begin();
  delay(100);
  
  // Step 1: Enable PWM - CRITICAL!
  Serial.println("STEP 1: Enable PWM Output");
  Serial.println("  Writing 0x00 to register 0x48 (enable + 10s timeout)");
  Wire.beginTransmission(SERVO_ADDR);
  Wire.write(0x48);  // PWM enable register
  Wire.write(0x00);  // Enable with timeout (will keep refreshing)
  Wire.endTransmission();
  delay(50);
  Serial.println("  ✓ PWM Enabled");
  Serial.println();
  
  // Step 2: Set step time
  Serial.println("STEP 2: Set Step Time");
  Serial.println("  Writing 0 to register 0x41 (fastest movement)");
  Wire.beginTransmission(SERVO_ADDR);
  Wire.write(0x41);  // Step time register
  Wire.write(0);     // 0 = no timing, servos move at max rate
  Wire.endTransmission();
  delay(50);
  Serial.println("  ✓ Step Time Set");
  Serial.println();
  
  Serial.println("STEP 3: Moving Servos");
  Serial.println("════════════════════════════════════════");
  Serial.println();
  
  // Move servo 1 to center
  Serial.println("Channel 1 → 127 (CENTER)");
  Wire.beginTransmission(SERVO_ADDR);
  Wire.write(0x42);  // Servo 1 position register
  Wire.write(127);   // Center position
  Wire.endTransmission();
  delay(1000);
  Serial.println("  ✓ Command sent");
  Serial.println();
  
  // Move servo 2 to center
  Serial.println("Channel 2 → 127 (CENTER)");
  Wire.beginTransmission(SERVO_ADDR);
  Wire.write(0x43);  // Servo 2 position register
  Wire.write(127);   // Center position
  Wire.endTransmission();
  delay(1000);
  Serial.println("  ✓ Command sent");
  Serial.println();
  
  // Move servo 1 to 0
  Serial.println("Channel 1 → 0 (LEFT/MIN)");
  Wire.beginTransmission(SERVO_ADDR);
  Wire.write(0x42);
  Wire.write(0);
  Wire.endTransmission();
  delay(1000);
  Serial.println("  ✓ Command sent");
  Serial.println();
  
  // Move servo 2 to 255
  Serial.println("Channel 2 → 255 (RIGHT/MAX)");
  Wire.beginTransmission(SERVO_ADDR);
  Wire.write(0x43);
  Wire.write(255);
  Wire.endTransmission();
  delay(1000);
  Serial.println("  ✓ Command sent");
  Serial.println();
  
  // Move servo 1 to 255
  Serial.println("Channel 1 → 255 (RIGHT/MAX)");
  Wire.beginTransmission(SERVO_ADDR);
  Wire.write(0x42);
  Wire.write(255);
  Wire.endTransmission();
  delay(1000);
  Serial.println("  ✓ Command sent");
  Serial.println();
  
  // Move servo 2 to 0
  Serial.println("Channel 2 → 0 (LEFT/MIN)");
  Wire.beginTransmission(SERVO_ADDR);
  Wire.write(0x43);
  Wire.write(0);
  Wire.endTransmission();
  delay(1000);
  Serial.println("  ✓ Command sent");
  Serial.println();
  
  Serial.println("════════════════════════════════════════");
  Serial.println("Setup Complete - Starting Continuous Test");
  Serial.println("════════════════════════════════════════");
  Serial.println();
}

void loop() {
  static unsigned long lastCommand = 0;
  static unsigned long lastEnable = 0;
  static int pos1 = 127;
  static int pos2 = 127;
  static int dir1 = 1;
  static int dir2 = -1;
  
  // Re-enable PWM every 5 seconds to prevent timeout
  if (millis() - lastEnable > 5000) {
    Wire.beginTransmission(SERVO_ADDR);
    Wire.write(0x48);
    Wire.write(0x00);  // Re-enable
    Wire.endTransmission();
    lastEnable = millis();
    Serial.println("[PWM refreshed]");
  }
  
  // Update servo positions every 50ms
  if (millis() - lastCommand > 50) {
    // Update servo 1
    Wire.beginTransmission(SERVO_ADDR);
    Wire.write(0x42);
    Wire.write(pos1);
    Wire.endTransmission();
    
    // Update servo 2
    Wire.beginTransmission(SERVO_ADDR);
    Wire.write(0x43);
    Wire.write(pos2);
    Wire.endTransmission();
    
    Serial.print("CH1: ");
    Serial.print(pos1);
    Serial.print("  CH2: ");
    Serial.println(pos2);
    
    // Update positions
    pos1 += dir1 * 5;
    pos2 += dir2 * 5;
    
    // Reverse direction at limits
    if (pos1 >= 255 || pos1 <= 0) dir1 = -dir1;
    if (pos2 >= 255 || pos2 <= 0) dir2 = -dir2;
    
    lastCommand = millis();
  }
}
