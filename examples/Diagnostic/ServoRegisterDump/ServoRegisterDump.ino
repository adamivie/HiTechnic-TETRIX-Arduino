/*
  Servo Controller Register Dump
  
  This diagnostic reads all key registers from the HiTechnic Servo Controller
  to help diagnose communication and configuration issues.
  
  Registers checked:
  - 0x00: Version
  - 0x08-0x0F: Manufacturer string  
  - 0x10-0x17: Sensor type string
  - 0x40: Status register
  - 0x41: Step time
  - 0x42-0x47: Servo positions 1-6
*/

#include <Wire.h>

#define SERVO_ADDRESS 0x04

// Register addresses
#define REG_VERSION      0x00
#define REG_MANUFACTURER 0x08
#define REG_SENSOR_TYPE  0x10
#define REG_STATUS       0x40
#define REG_STEP_TIME    0x41
#define REG_SERVO1_POS   0x42

uint8_t readRegister(uint8_t reg) {
  Wire.beginTransmission(SERVO_ADDRESS);
  Wire.write(reg);
  uint8_t error = Wire.endTransmission();
  
  if (error != 0) {
    Serial.print("I2C Error on read: ");
    Serial.println(error);
    return 0xFF;
  }
  
  Wire.requestFrom(SERVO_ADDRESS, (uint8_t)1);
  if (Wire.available()) {
    return Wire.read();
  }
  return 0xFF;
}

void writeRegister(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(SERVO_ADDRESS);
  Wire.write(reg);
  Wire.write(value);
  uint8_t error = Wire.endTransmission();
  
  if (error != 0) {
    Serial.print("I2C Error on write: ");
    Serial.println(error);
  }
  delay(2);
}

void readString(uint8_t startReg, uint8_t length) {
  for (uint8_t i = 0; i < length; i++) {
    uint8_t val = readRegister(startReg + i);
    if (val >= 32 && val < 127) {
      Serial.print((char)val);
    } else if (val == 0) {
      break;
    } else {
      Serial.print("[0x");
      Serial.print(val, HEX);
      Serial.print("]");
    }
  }
  Serial.println();
}

void setup() {
  Serial.begin(9600);
  while (!Serial);
  
  Wire.begin();
  delay(200);
  
  Serial.println("\n===== HiTechnic Servo Controller Register Dump =====");
  Serial.print("Controller Address: 0x");
  Serial.println(SERVO_ADDRESS, HEX);
  Serial.println();
  
  // Read version
  Serial.print("Version (0x00): ");
  uint8_t version = readRegister(REG_VERSION);
  Serial.print("0x");
  Serial.print(version, HEX);
  Serial.print(" (");
  Serial.print(version);
  Serial.println(")");
  
  // Read manufacturer string
  Serial.print("Manufacturer (0x08-0x0F): ");
  readString(REG_MANUFACTURER, 8);
  
  // Read sensor type string
  Serial.print("Sensor Type (0x10-0x17): ");
  readString(REG_SENSOR_TYPE, 8);
  
  // Read status
  Serial.print("Status (0x40): 0x");
  Serial.println(readRegister(REG_STATUS), HEX);
  
  // Read step time
  Serial.print("Step Time (0x41): ");
  Serial.println(readRegister(REG_STEP_TIME));
  
  // Read all servo positions
  Serial.println("\nServo Positions:");
  for (uint8_t i = 0; i < 6; i++) {
    Serial.print("  Servo ");
    Serial.print(i + 1);
    Serial.print(" (0x");
    Serial.print(REG_SERVO1_POS + i, HEX);
    Serial.print("): ");
    Serial.println(readRegister(REG_SERVO1_POS + i));
  }
  
  Serial.println("\n===== Testing Write Operations =====");
  
  // Try writing step time
  Serial.println("Writing step time = 5...");
  writeRegister(REG_STEP_TIME, 5);
  delay(10);
  Serial.print("Read back: ");
  Serial.println(readRegister(REG_STEP_TIME));
  
  // Try writing servo 1 position
  Serial.println("\nWriting Servo 1 = 127 (center)...");
  writeRegister(REG_SERVO1_POS, 127);
  delay(100);
  Serial.print("Read back: ");
  Serial.println(readRegister(REG_SERVO1_POS));
  
  Serial.println("\nWriting Servo 1 = 0...");
  writeRegister(REG_SERVO1_POS, 0);
  delay(1000);
  Serial.print("Read back: ");
  Serial.println(readRegister(REG_SERVO1_POS));
  
  Serial.println("\nWriting Servo 1 = 255...");
  writeRegister(REG_SERVO1_POS, 255);
  delay(1000);
  Serial.print("Read back: ");
  Serial.println(readRegister(REG_SERVO1_POS));
  
  Serial.println("\n===== Test Complete =====");
  Serial.println("Check if servo physically moved during position changes");
}

void loop() {
  // Sweep servo 1 slowly
  static unsigned long lastMove = 0;
  static uint8_t pos = 0;
  static int8_t dir = 1;
  
  if (millis() - lastMove > 100) {
    writeRegister(REG_SERVO1_POS, pos);
    Serial.print("Servo 1 Position: ");
    Serial.println(pos);
    
    pos += dir * 5;
    if (pos >= 250 || pos <= 5) {
      dir = -dir;
    }
    
    lastMove = millis();
  }
}
