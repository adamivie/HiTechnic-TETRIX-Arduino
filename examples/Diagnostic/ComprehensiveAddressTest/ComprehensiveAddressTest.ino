/*
  Comprehensive Address Scanner
  
  Tests motor controller (known working) and scans all addresses
  to find the servo controller and compare behavior.
*/

#include <Wire.h>

void testAddress(uint8_t addr, const char* expectedDevice) {
  Serial.println("========================================");
  Serial.print("Testing Address: 0x");
  if (addr < 0x10) Serial.print("0");
  Serial.print(addr, HEX);
  Serial.print(" (");
  Serial.print(addr);
  Serial.print(") - ");
  Serial.println(expectedDevice);
  Serial.println("========================================");
  
  // Test 1: Ping (simple transmission)
  Serial.print("Ping test: ");
  Wire.beginTransmission(addr);
  uint8_t pingError = Wire.endTransmission();
  if (pingError == 0) {
    Serial.println("✓ Device responds");
  } else {
    Serial.print("✗ No response (error ");
    Serial.print(pingError);
    Serial.println(")");
    Serial.println();
    return; // Skip rest if device doesn't respond
  }
  
  // Test 2: Read version (0x00)
  Serial.print("Read version (0x00): ");
  Wire.beginTransmission(addr);
  Wire.write(0x00);
  uint8_t error = Wire.endTransmission();
  if (error == 0) {
    Wire.requestFrom(addr, (uint8_t)1);
    if (Wire.available()) {
      uint8_t version = Wire.read();
      Serial.print("0x");
      if (version < 0x10) Serial.print("0");
      Serial.print(version, HEX);
      Serial.print(" (");
      Serial.print(version);
      Serial.println(")");
    } else {
      Serial.println("✗ No data");
    }
  } else {
    Serial.print("✗ Error ");
    Serial.println(error);
  }
  
  // Test 3: Read manufacturer (0x08)
  Serial.print("Read manufacturer (0x08-0x0F): ");
  Wire.beginTransmission(addr);
  Wire.write(0x08);
  error = Wire.endTransmission();
  if (error == 0) {
    Wire.requestFrom(addr, (uint8_t)8);
    String mfg = "";
    while (Wire.available()) {
      char c = Wire.read();
      if (c >= 32 && c < 127) mfg += c;
    }
    if (mfg.length() > 0) {
      Serial.print("'");
      Serial.print(mfg);
      Serial.println("'");
    } else {
      Serial.println("✗ No readable data");
    }
  } else {
    Serial.print("✗ Error ");
    Serial.println(error);
  }
  
  // Test 4: Read sensor type (0x10)
  Serial.print("Read sensor type (0x10-0x17): ");
  Wire.beginTransmission(addr);
  Wire.write(0x10);
  error = Wire.endTransmission();
  if (error == 0) {
    Wire.requestFrom(addr, (uint8_t)8);
    String sensor = "";
    while (Wire.available()) {
      char c = Wire.read();
      if (c >= 32 && c < 127) sensor += c;
    }
    if (sensor.length() > 0) {
      Serial.print("'");
      Serial.print(sensor);
      Serial.println("'");
    } else {
      Serial.println("✗ No readable data");
    }
  } else {
    Serial.print("✗ Error ");
    Serial.println(error);
  }
  
  // Test 5: Write test to register 0x41 (step time for servo, gear ratio for motor)
  Serial.print("Write test to 0x41: ");
  Wire.beginTransmission(addr);
  Wire.write(0x41);
  Wire.write(5);
  error = Wire.endTransmission();
  if (error == 0) {
    Serial.println("✓ Write successful");
    
    // Read back
    delay(5);
    Wire.beginTransmission(addr);
    Wire.write(0x41);
    Wire.endTransmission();
    Wire.requestFrom(addr, (uint8_t)1);
    if (Wire.available()) {
      Serial.print("  Read back: ");
      Serial.println(Wire.read());
    }
  } else {
    Serial.print("✗ Write failed (error ");
    Serial.print(error);
    Serial.println(")");
  }
  
  // Test 6: Write to position register (0x42 for both)
  Serial.print("Write test to 0x42: ");
  Wire.beginTransmission(addr);
  Wire.write(0x42);
  Wire.write(127);
  error = Wire.endTransmission();
  if (error == 0) {
    Serial.println("✓ Write successful");
    
    // Read back
    delay(5);
    Wire.beginTransmission(addr);
    Wire.write(0x42);
    Wire.endTransmission();
    Wire.requestFrom(addr, (uint8_t)1);
    if (Wire.available()) {
      Serial.print("  Read back: ");
      Serial.println(Wire.read());
    }
  } else {
    Serial.print("✗ Write failed (error ");
    Serial.print(error);
    Serial.println(")");
  }
  
  // Test 7: For servo controller, try PWM enable (0x48)
  if (addr == 0x04) {
    Serial.print("Write PWM enable (0x48) = 0xAA: ");
    Wire.beginTransmission(addr);
    Wire.write(0x48);
    Wire.write(0xAA);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.println("✓ Write successful");
      
      delay(10);
      Wire.beginTransmission(addr);
      Wire.write(0x48);
      Wire.endTransmission();
      Wire.requestFrom(addr, (uint8_t)1);
      if (Wire.available()) {
        uint8_t val = Wire.read();
        Serial.print("  Read back: 0x");
        if (val < 0x10) Serial.print("0");
        Serial.println(val, HEX);
      }
    } else {
      Serial.print("✗ Write failed (error ");
      Serial.print(error);
      Serial.println(")");
    }
  }
  
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  while (!Serial);
  
  Wire.begin();
  delay(200);
  
  Serial.println("\n\n");
  Serial.println("╔════════════════════════════════════════════════════════╗");
  Serial.println("║     COMPREHENSIVE I2C ADDRESS SCANNER & TESTER        ║");
  Serial.println("╚════════════════════════════════════════════════════════╝");
  Serial.println();
  
  Serial.println("This will test all I2C devices found and compare behavior");
  Serial.println("between the working motor controller and servo controller.");
  Serial.println();
  
  delay(500);
  
  // First, scan for all devices
  Serial.println(">>> Quick scan for all devices...\n");
  uint8_t foundDevices[10];
  uint8_t deviceCount = 0;
  
  for (uint8_t addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      foundDevices[deviceCount++] = addr;
      Serial.print("Found device at 0x");
      if (addr < 0x10) Serial.print("0");
      Serial.println(addr, HEX);
    }
  }
  
  Serial.print("\nTotal devices found: ");
  Serial.println(deviceCount);
  Serial.println();
  
  delay(1000);
  
  // Now test each device in detail
  Serial.println(">>> Detailed testing of each device...\n");
  
  for (uint8_t i = 0; i < deviceCount; i++) {
    uint8_t addr = foundDevices[i];
    String deviceName = "Unknown";
    
    if (addr == 0x01) deviceName = "Motor Controller 1 (KNOWN WORKING)";
    else if (addr == 0x02) deviceName = "Motor Controller 2";
    else if (addr == 0x03) deviceName = "Motor Controller 3";
    else if (addr == 0x04) deviceName = "Servo Controller (PROBLEM DEVICE)";
    
    testAddress(addr, deviceName.c_str());
    delay(100);
  }
  
  Serial.println("========================================");
  Serial.println("COMPARISON SUMMARY");
  Serial.println("========================================");
  Serial.println("Compare the results above to identify differences");
  Serial.println("between working motor controllers and servo controller.");
  Serial.println();
  Serial.println("Key things to check:");
  Serial.println("1. Does servo respond to ping? (should be YES)");
  Serial.println("2. Can we read version/manufacturer? (should be YES)");
  Serial.println("3. Can we write to registers? (THIS is likely the issue)");
  Serial.println("4. Do writes get acknowledged but data doesn't change?");
  Serial.println();
  Serial.println("=== Test Complete ===");
}

void loop() {
  // Nothing
}
