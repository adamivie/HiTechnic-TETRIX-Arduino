/*
  Deep I2C Analysis for Servo Controller
  
  This performs comprehensive analysis of I2C communication with timing,
  signal analysis, and detailed error reporting.
*/

#include <Wire.h>

#define SERVO_ADDR 0x04

// Test results structure
struct I2CTest {
  const char* name;
  uint8_t reg;
  bool isWrite;
  uint8_t writeValue;
  uint8_t expectedBytes;
  bool success;
  uint8_t error;
  uint8_t readData[16];
  unsigned long duration_us;
};

I2CTest tests[20];
uint8_t testCount = 0;

void printBinary(uint8_t val) {
  for (int i = 7; i >= 0; i--) {
    Serial.print((val >> i) & 1);
    if (i == 4) Serial.print(" ");
  }
}

void performRead(const char* name, uint8_t reg, uint8_t numBytes) {
  I2CTest &test = tests[testCount++];
  test.name = name;
  test.reg = reg;
  test.isWrite = false;
  test.expectedBytes = numBytes;
  
  unsigned long start = micros();
  
  // Write register address
  Wire.beginTransmission(SERVO_ADDR);
  Wire.write(reg);
  test.error = Wire.endTransmission(false); // Keep connection
  
  if (test.error == 0) {
    // Read data
    uint8_t received = Wire.requestFrom(SERVO_ADDR, numBytes);
    
    for (uint8_t i = 0; i < received && i < 16; i++) {
      if (Wire.available()) {
        test.readData[i] = Wire.read();
      }
    }
    test.success = (received == numBytes);
  } else {
    test.success = false;
  }
  
  test.duration_us = micros() - start;
}

void performWrite(const char* name, uint8_t reg, uint8_t value) {
  I2CTest &test = tests[testCount++];
  test.name = name;
  test.reg = reg;
  test.isWrite = true;
  test.writeValue = value;
  test.expectedBytes = 0;
  
  unsigned long start = micros();
  
  Wire.beginTransmission(SERVO_ADDR);
  Wire.write(reg);
  Wire.write(value);
  test.error = Wire.endTransmission();
  
  test.success = (test.error == 0);
  test.duration_us = micros() - start;
}

void printTestResults() {
  Serial.println("\n=== DETAILED I2C TEST RESULTS ===\n");
  
  for (uint8_t i = 0; i < testCount; i++) {
    I2CTest &test = tests[i];
    
    Serial.println("----------------------------------------");
    Serial.print("Test #");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.println(test.name);
    Serial.println("----------------------------------------");
    
    Serial.print("Operation: ");
    Serial.println(test.isWrite ? "WRITE" : "READ");
    
    Serial.print("Register: 0x");
    if (test.reg < 0x10) Serial.print("0");
    Serial.print(test.reg, HEX);
    Serial.print(" (");
    printBinary(test.reg);
    Serial.println(")");
    
    if (test.isWrite) {
      Serial.print("Write Value: 0x");
      if (test.writeValue < 0x10) Serial.print("0");
      Serial.print(test.writeValue, HEX);
      Serial.print(" (");
      printBinary(test.writeValue);
      Serial.print(") = ");
      Serial.println(test.writeValue);
    }
    
    Serial.print("Result: ");
    if (test.success) {
      Serial.println("SUCCESS");
    } else {
      Serial.print("FAILED - Error Code: ");
      Serial.println(test.error);
      Serial.println("  Error codes:");
      Serial.println("    0 = Success");
      Serial.println("    1 = Data too long");
      Serial.println("    2 = NACK on address");
      Serial.println("    3 = NACK on data");
      Serial.println("    4 = Other error");
    }
    
    if (!test.isWrite && test.success) {
      Serial.print("Bytes Read: ");
      Serial.println(test.expectedBytes);
      Serial.print("Data (hex): ");
      for (uint8_t j = 0; j < test.expectedBytes; j++) {
        if (test.readData[j] < 0x10) Serial.print("0");
        Serial.print(test.readData[j], HEX);
        Serial.print(" ");
      }
      Serial.println();
      
      Serial.print("Data (dec): ");
      for (uint8_t j = 0; j < test.expectedBytes; j++) {
        Serial.print(test.readData[j]);
        Serial.print(" ");
      }
      Serial.println();
      
      Serial.print("Data (chr): ");
      for (uint8_t j = 0; j < test.expectedBytes; j++) {
        if (test.readData[j] >= 32 && test.readData[j] < 127) {
          Serial.print((char)test.readData[j]);
        } else {
          Serial.print(".");
        }
      }
      Serial.println();
    }
    
    Serial.print("Duration: ");
    Serial.print(test.duration_us);
    Serial.println(" microseconds");
    
    Serial.println();
  }
  
  // Summary
  Serial.println("========================================");
  Serial.println("SUMMARY");
  Serial.println("========================================");
  
  uint8_t passCount = 0;
  uint8_t failCount = 0;
  uint8_t readFails = 0;
  uint8_t writeFails = 0;
  
  for (uint8_t i = 0; i < testCount; i++) {
    if (tests[i].success) {
      passCount++;
    } else {
      failCount++;
      if (tests[i].isWrite) writeFails++;
      else readFails++;
    }
  }
  
  Serial.print("Total Tests: ");
  Serial.println(testCount);
  Serial.print("Passed: ");
  Serial.print(passCount);
  Serial.print(" (");
  Serial.print(passCount * 100 / testCount);
  Serial.println("%)");
  Serial.print("Failed: ");
  Serial.print(failCount);
  Serial.print(" (");
  Serial.print(failCount * 100 / testCount);
  Serial.println("%)");
  Serial.print("  - Read failures: ");
  Serial.println(readFails);
  Serial.print("  - Write failures: ");
  Serial.println(writeFails);
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  while (!Serial);
  
  Serial.println("\n\n");
  Serial.println("╔════════════════════════════════════════════════════════╗");
  Serial.println("║   DEEP I2C ANALYSIS - HiTechnic Servo Controller      ║");
  Serial.println("╚════════════════════════════════════════════════════════╝");
  Serial.println();
  
  Serial.println("Target: Servo Controller at 0x04");
  Serial.println("I2C Bus: Arduino Mega Wire (pins 20/21)");
  Serial.println();
  
  Wire.begin();
  Wire.setClock(100000); // 100kHz standard mode
  delay(100);
  
  Serial.println("Performing comprehensive I2C tests...\n");
  delay(500);
  
  // ===== READ TESTS =====
  Serial.println(">>> Starting READ tests...");
  
  // Test 1: Read version (single byte)
  performRead("Read Version", 0x00, 1);
  delay(10);
  
  // Test 2: Read manufacturer (8 bytes)
  performRead("Read Manufacturer", 0x08, 8);
  delay(10);
  
  // Test 3: Read sensor type (8 bytes)
  performRead("Read Sensor Type", 0x10, 8);
  delay(10);
  
  // Test 4: Read status
  performRead("Read Status", 0x40, 1);
  delay(10);
  
  // Test 5: Read step time
  performRead("Read Step Time", 0x41, 1);
  delay(10);
  
  // Test 6: Read servo 1 position
  performRead("Read Servo 1 Position", 0x42, 1);
  delay(10);
  
  // Test 7: Read PWM enable
  performRead("Read PWM Enable", 0x48, 1);
  delay(10);
  
  Serial.println(">>> READ tests complete\n");
  delay(100);
  
  // ===== WRITE TESTS =====
  Serial.println(">>> Starting WRITE tests...");
  
  // Test 8: Write step time
  performWrite("Write Step Time = 5", 0x41, 5);
  delay(10);
  
  // Test 9: Write PWM enable
  performWrite("Write PWM Enable = 0xAA", 0x48, 0xAA);
  delay(10);
  
  // Test 10: Write servo 1 position = 127 (center)
  performWrite("Write Servo 1 = 127", 0x42, 127);
  delay(100);
  
  // Test 11: Read back servo 1 position
  performRead("Read Back Servo 1", 0x42, 1);
  delay(10);
  
  // Test 12: Write servo 1 position = 0
  performWrite("Write Servo 1 = 0", 0x42, 0);
  delay(100);
  
  // Test 13: Read back servo 1 position
  performRead("Read Back Servo 1", 0x42, 1);
  delay(10);
  
  // Test 14: Write servo 1 position = 255
  performWrite("Write Servo 1 = 255", 0x42, 255);
  delay(100);
  
  // Test 15: Read back servo 1 position
  performRead("Read Back Servo 1", 0x42, 1);
  delay(10);
  
  Serial.println(">>> WRITE tests complete\n");
  delay(100);
  
  // Print all results
  printTestResults();
  
  // Diagnostic suggestions
  Serial.println("========================================");
  Serial.println("DIAGNOSTIC SUGGESTIONS");
  Serial.println("========================================");
  
  bool hasReadSuccess = false;
  bool hasWriteSuccess = false;
  
  for (uint8_t i = 0; i < testCount; i++) {
    if (tests[i].success) {
      if (tests[i].isWrite) hasWriteSuccess = true;
      else hasReadSuccess = true;
    }
  }
  
  if (!hasReadSuccess && !hasWriteSuccess) {
    Serial.println("⚠ No communication possible!");
    Serial.println("  - Check servo controller is powered (12V battery)");
    Serial.println("  - Verify I2C wiring (SDA=pin20, SCL=pin21, GND)");
    Serial.println("  - Confirm address 0x04 is correct");
    Serial.println("  - Check for shorts or damaged cables");
  } else if (hasReadSuccess && !hasWriteSuccess) {
    Serial.println("⚠ Reads work but writes fail!");
    Serial.println("  - Controller may be in read-only mode");
    Serial.println("  - Check if controller has write-protect jumper");
    Serial.println("  - Firmware may need special unlock sequence");
  } else if (!hasReadSuccess && hasWriteSuccess) {
    Serial.println("⚠ Writes work but reads fail!");
    Serial.println("  - Unusual - check I2C pullup resistors");
    Serial.println("  - Try slower I2C clock speed");
  } else {
    Serial.println("✓ Communication working!");
    Serial.println("  - If servo not moving, check:");
    Serial.println("    * Servo connected to correct port");
    Serial.println("    * Servo power requirements (4.8-6V for MG90D)");
    Serial.println("    * PWM enable register (0x48) set correctly");
    Serial.println("    * Servo type compatible (standard PWM)");
  }
  
  Serial.println("\n=== Analysis Complete ===\n");
}

void loop() {
  // Nothing - all analysis in setup()
}
