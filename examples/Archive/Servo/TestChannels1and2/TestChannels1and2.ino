/*
  Servo Channel 1 & 2 Specific Test
  
  Since you have servos on channels 1 and 2, this will test those specifically
  with detailed feedback on what's being sent vs received.
*/

#include <Wire.h>

#define SERVO_ADDR 0x04

void writeReg(uint8_t reg, uint8_t val) {
  Wire.beginTransmission(SERVO_ADDR);
  Wire.write(reg);
  Wire.write(val);
  uint8_t error = Wire.endTransmission();
  
  Serial.print("  Write reg 0x");
  if (reg < 0x10) Serial.print("0");
  Serial.print(reg, HEX);
  Serial.print(" = ");
  Serial.print(val);
  if (error == 0) {
    Serial.println(" ✓");
  } else {
    Serial.print(" ✗ Error: ");
    Serial.println(error);
  }
}

uint8_t readReg(uint8_t reg) {
  Wire.beginTransmission(SERVO_ADDR);
  Wire.write(reg);
  Wire.endTransmission();
  Wire.requestFrom(SERVO_ADDR, (uint8_t)1);
  if (Wire.available()) return Wire.read();
  return 0xFF;
}

void setup() {
  Serial.begin(9600);
  while (!Serial);
  
  Serial.println("\n=== SERVO CHANNELS 1 & 2 TEST ===");
  Serial.println("Testing servos you confirmed are on channels 1 & 2");
  Serial.println();
  
  Wire.begin();
  delay(200);
  
  Serial.println("Controller Info:");
  Serial.print("  Version: ");
  Serial.println(readReg(0x00));
  Serial.print("  Status: ");
  Serial.println(readReg(0x40));
  Serial.println();
  
  Serial.println("Enabling PWM (no timeout):");
  writeReg(0x48, 0xAA);
  delay(10);
  Serial.print("  PWM Enable readback: 0x");
  Serial.println(readReg(0x48), HEX);
  Serial.println();
  
  Serial.println("Setting step time = 5 (moderate speed):");
  writeReg(0x41, 5);
  delay(10);
  Serial.print("  Step time readback: ");
  Serial.println(readReg(0x41));
  Serial.println();
  
  Serial.println("=== CHANNEL 1 TEST (Servo 1, Register 0x42) ===");
  
  Serial.println("\nMoving to position 0:");
  writeReg(0x42, 0);
  delay(1000);
  Serial.print("  Readback: ");
  Serial.println(readReg(0x42));
  
  Serial.println("\nMoving to position 127 (center):");
  writeReg(0x42, 127);
  delay(1000);
  Serial.print("  Readback: ");
  Serial.println(readReg(0x42));
  
  Serial.println("\nMoving to position 255:");
  writeReg(0x42, 255);
  delay(1000);
  Serial.print("  Readback: ");
  Serial.println(readReg(0x42));
  
  Serial.println("\n=== CHANNEL 2 TEST (Servo 2, Register 0x43) ===");
  
  Serial.println("\nMoving to position 0:");
  writeReg(0x43, 0);
  delay(1000);
  Serial.print("  Readback: ");
  Serial.println(readReg(0x43));
  
  Serial.println("\nMoving to position 127 (center):");
  writeReg(0x43, 127);
  delay(1000);
  Serial.print("  Readback: ");
  Serial.println(readReg(0x43));
  
  Serial.println("\nMoving to position 255:");
  writeReg(0x43, 255);
  delay(1000);
  Serial.print("  Readback: ");
  Serial.println(readReg(0x43));
  
  Serial.println("\n=== Setup Complete ===");
  Serial.println("Now doing alternating sweep on both channels...");
  Serial.println("Watch for servo movement!");
  Serial.println();
}

void loop() {
  static unsigned long lastMove = 0;
  static uint8_t phase = 0;
  
  if (millis() - lastMove > 1000) {
    // Re-enable PWM periodically
    writeReg(0x48, 0xAA);
    
    switch(phase) {
      case 0:
        Serial.println("CH1=0, CH2=255");
        writeReg(0x42, 0);    // Channel 1 left
        writeReg(0x43, 255);  // Channel 2 right
        break;
      case 1:
        Serial.println("CH1=127, CH2=127");
        writeReg(0x42, 127);  // Both center
        writeReg(0x43, 127);
        break;
      case 2:
        Serial.println("CH1=255, CH2=0");
        writeReg(0x42, 255);  // Channel 1 right
        writeReg(0x43, 0);    // Channel 2 left
        break;
      case 3:
        Serial.println("CH1=127, CH2=127");
        writeReg(0x42, 127);  // Both center
        writeReg(0x43, 127);
        break;
    }
    
    phase = (phase + 1) % 4;
    lastMove = millis();
  }
}
