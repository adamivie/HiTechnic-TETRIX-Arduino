/*
 * Test 5V Analog Detection for Daisy Chain Addressing
 * 
 * This program tests the theory that controllers need 5V through 10kΩ
 * resistor on Pin 5 (SCL line) to automatically assign addresses.
 * 
 * HARDWARE SETUP REQUIRED:
 * 
 * 1. Connect a 10kΩ resistor between Arduino Pin 22 and Pin 5 of the 
 *    FIRST controller's 6P6C cable (the SCL line)
 * 
 * 2. Standard I2C connections:
 *    - Arduino Pin 20 (SDA) → Pin 6 of all controllers
 *    - Arduino Pin 21 (SCL) → Pin 5 of all controllers  
 *    - Arduino GND → Pin 1 or 2 of all controllers
 * 
 * 3. Daisy chain the controllers normally
 * 
 * THEORY:
 * - First controller detects 5V on Pin 5 → assigns address 0x01 (0x02/0x03 in 8-bit)
 * - First controller supplies 5V to second controller
 * - Second controller detects 5V from first → assigns address 0x02 (0x04/0x05)
 * - Third gets 0x03 (0x06/0x07), Fourth gets 0x04 (0x08/0x09)
 */

#include <Wire.h>

#define ANALOG_DETECT_PIN 22  // Pin to supply 5V through 10kΩ resistor

void setup() {
  Serial.begin(9600);
  while (!Serial) { delay(10); }
  
  Serial.println("\n=== HiTechnic Daisy Chain Address Test ===");
  Serial.println("Testing 5V analog detection for auto-addressing\n");
  
  // Supply 5V to first controller through analog detection
  pinMode(ANALOG_DETECT_PIN, OUTPUT);
  digitalWrite(ANALOG_DETECT_PIN, HIGH);
  
  Serial.println("Step 1: Supplying 5V to Pin 22 (connect through 10kΩ to controller Pin 5)");
  Serial.println("Waiting 2 seconds for controllers to initialize...");
  delay(2000);
  
  // Initialize I2C
  Wire.begin();
  Wire.setClock(100000);
  
  Serial.println("\nStep 2: Scanning for controllers...\n");
  
  // Scan expected addresses based on spec
  byte expectedAddresses[] = {0x01, 0x02, 0x03, 0x04};  // 7-bit addresses
  const char* positions[] = {"First", "Second", "Third", "Fourth"};
  
  int foundCount = 0;
  
  for (int i = 0; i < 4; i++) {
    byte addr = expectedAddresses[i];
    
    Serial.print("Checking ");
    Serial.print(positions[i]);
    Serial.print(" controller (0x");
    if (addr < 16) Serial.print("0");
    Serial.print(addr, HEX);
    Serial.print(" / 8-bit: 0x");
    Serial.print(addr << 1, HEX);
    Serial.print("/0x");
    Serial.print((addr << 1) | 1, HEX);
    Serial.print("): ");
    
    Wire.beginTransmission(addr);
    byte error = Wire.endTransmission();
    
    if (error == 0) {
      Serial.println("FOUND!");
      
      // Read version to confirm it's a HiTechnic controller
      Wire.beginTransmission(addr);
      Wire.write(0x00);  // Version register
      Wire.endTransmission();
      
      Wire.requestFrom(addr, (byte)8);
      if (Wire.available() >= 8) {
        Serial.print("  Version: ");
        while (Wire.available()) {
          char c = Wire.read();
          if (c >= 32 && c <= 126) Serial.print(c);
        }
        Serial.println();
      }
      
      // Read manufacturer
      Wire.beginTransmission(addr);
      Wire.write(0x08);  // Manufacturer register
      Wire.endTransmission();
      
      Wire.requestFrom(addr, (byte)8);
      if (Wire.available() >= 8) {
        Serial.print("  Manufacturer: ");
        while (Wire.available()) {
          char c = Wire.read();
          if (c >= 32 && c <= 126) Serial.print(c);
        }
        Serial.println();
      }
      
      foundCount++;
    } else {
      Serial.println("Not found");
    }
  }
  
  Serial.print("\n=== RESULTS ===\n");
  Serial.print("Controllers found: ");
  Serial.println(foundCount);
  
  if (foundCount == 0) {
    Serial.println("\nNo controllers found at expected addresses!");
    Serial.println("\nTroubleshooting:");
    Serial.println("1. Is Pin 22 connected through 10kΩ resistor to Pin 5 of first controller?");
    Serial.println("2. Are all controllers powered?");
    Serial.println("3. Are SDA (Pin 20) and SCL (Pin 21) connected to all controllers?");
    Serial.println("4. Is GND connected?");
    Serial.println("\nLet me scan ALL addresses to find where controllers actually are...\n");
    
    // Full scan
    Serial.println("Full I2C scan (0x01-0x7F):");
    for (byte addr = 1; addr < 127; addr++) {
      Wire.beginTransmission(addr);
      if (Wire.endTransmission() == 0) {
        Serial.print("  Device found at 0x");
        if (addr < 16) Serial.print("0");
        Serial.println(addr, HEX);
      }
    }
  } else if (foundCount < 4) {
    Serial.print("Only found ");
    Serial.print(foundCount);
    Serial.println(" controller(s).");
    Serial.println("Check that all controllers are daisy-chained and powered.");
  } else {
    Serial.println("SUCCESS! All 4 controllers found at correct addresses!");
  }
}

void loop() {
  // Nothing
}
