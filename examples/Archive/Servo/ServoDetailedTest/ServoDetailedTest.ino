/*
  Detailed Servo Test for MG90D
  
  This test specifically checks PWM enable and tests with MG90D servo
  which operates on 4.8-6V and standard 1-2ms PWM signals.
*/

#include <Wire.h>

#define SERVO_ADDRESS 0x04  // Testing different address

// Registers
#define REG_VERSION      0x00
#define REG_MANUFACTURER 0x08
#define REG_SENSOR_TYPE  0x10
#define REG_STATUS       0x40
#define REG_STEP_TIME    0x41
#define REG_SERVO1_POS   0x42
#define REG_PWM_ENABLE   0x48

void writeRegister(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(SERVO_ADDRESS);
  Wire.write(reg);
  Wire.write(value);
  uint8_t error = Wire.endTransmission();
  
  Serial.print("Write 0x");
  Serial.print(value, HEX);
  Serial.print(" to reg 0x");
  Serial.print(reg, HEX);
  if (error == 0) {
    Serial.println(" - OK");
  } else {
    Serial.print(" - ERROR: ");
    Serial.println(error);
  }
  delay(5);
}

uint8_t readRegister(uint8_t reg) {
  Wire.beginTransmission(SERVO_ADDRESS);
  Wire.write(reg);
  Wire.endTransmission();
  
  Wire.requestFrom(SERVO_ADDRESS, (uint8_t)1);
  if (Wire.available()) {
    return Wire.read();
  }
  return 0xFF;
}

void readString(uint8_t startReg, uint8_t length) {
  for (uint8_t i = 0; i < length; i++) {
    uint8_t val = readRegister(startReg + i);
    if (val >= 32 && val < 127) {
      Serial.print((char)val);
    } else if (val == 0) {
      break;
    }
  }
}

void setup() {
  Serial.begin(9600);
  while (!Serial);
  
  Wire.begin();
  delay(200);
  
  Serial.println("\n=== MG90D Servo Test ===");
  Serial.println("Controller Address: 0x08 (4th in chain)");
  Serial.println();
  
  // Read version
  Serial.print("Version: ");
  Serial.println(readRegister(REG_VERSION));
  
  // Read manufacturer
  Serial.print("Manufacturer: ");
  readString(REG_MANUFACTURER, 8);
  Serial.println();
  
  // Read sensor type
  Serial.print("Sensor Type: ");
  readString(REG_SENSOR_TYPE, 8);
  Serial.println();
  
  Serial.println("\n=== Checking PWM Enable Register ===");
  Serial.print("Current PWM Enable (0x48): 0x");
  Serial.println(readRegister(REG_PWM_ENABLE), HEX);
  
  Serial.println("\nEnabling PWM (writing 0xAA to reg 0x48)...");
  writeRegister(REG_PWM_ENABLE, 0xAA);
  
  delay(50);
  Serial.print("Read back PWM Enable: 0x");
  Serial.println(readRegister(REG_PWM_ENABLE), HEX);
  
  // Set slow speed for testing
  Serial.println("\nSetting step time to 10 (slow)...");
  writeRegister(REG_STEP_TIME, 10);
  
  Serial.println("\n=== Starting MG90D Movement Test ===");
  Serial.println("Watch the servo on Port 1...");
  Serial.println();
  
  delay(1000);
}

void loop() {
  // Test positions suitable for MG90D (1ms to 2ms pulse)
  // Position 0-255 maps to 0.75-2.25ms
  // We want 1-2ms which is roughly positions 50-200
  
  Serial.println("Moving to 90 degrees (center - pos 127)");
  writeRegister(REG_SERVO1_POS, 127);
  delay(2000);
  
  Serial.println("Moving to 0 degrees (pos 50)");
  writeRegister(REG_SERVO1_POS, 50);  // ~1ms pulse
  delay(2000);
  
  Serial.println("Moving to 180 degrees (pos 200)");
  writeRegister(REG_SERVO1_POS, 200);  // ~2ms pulse
  delay(2000);
  
  Serial.println("Moving to center (pos 127)");
  writeRegister(REG_SERVO1_POS, 127);  // ~1.5ms pulse
  delay(2000);
  
  // Check PWM enable status
  Serial.print("PWM Enable status: 0x");
  Serial.println(readRegister(REG_PWM_ENABLE), HEX);
  
  // Check servo status (busy?)
  Serial.print("Servo Status: ");
  Serial.println(readRegister(REG_STATUS));
  
  Serial.println("---");
  delay(1000);
}
