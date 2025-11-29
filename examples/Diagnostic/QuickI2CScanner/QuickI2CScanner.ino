/*
  Quick I2C Scanner with Pin 22 Setup
  
  This scanner includes Pin 22 analog detection setup
  and has shorter timeouts to prevent hanging.
  
  Hardware:
  - Arduino Mega 2560
  - Pin 20 (SDA) to controllers
  - Pin 21 (SCL) to controllers
  - Pin 22 via 10kΩ resistor to first controller Pin 5
  - Common GND
*/

#include <Wire.h>

void setup() {
  Serial.begin(9600);
  
  // IMPORTANT: Setup Pin 22 for daisy chain addressing
  pinMode(22, OUTPUT);
  digitalWrite(22, HIGH);
  
  // Give controllers time to initialize
  delay(500);
  
  Wire.begin();
  Wire.setTimeout(50);  // Short timeout to prevent hanging
  
  Serial.println(F("\n=== I2C Scanner with Pin 22 Setup ==="));
  Serial.println(F("Pin 22 analog detection: ENABLED"));
  Serial.println(F("Scanning I2C bus...\n"));
  
  delay(100);
}

void loop() {
  byte error, address;
  int deviceCount = 0;
  
  Serial.println(F("Scan starting..."));
  
  for (address = 1; address < 127; address++) {
    // Try to communicate with device
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    
    if (error == 0) {
      // Device found!
      Serial.print(F("  0x"));
      if (address < 16) Serial.print(F("0"));
      Serial.print(address, HEX);
      Serial.print(F(" - "));
      
      // Identify likely device type
      if (address >= 0x01 && address <= 0x03) {
        Serial.println(F("Motor Controller (typical)"));
      } else if (address == 0x04) {
        Serial.println(F("Servo Controller (default)"));
      } else if (address >= 0x02 && address <= 0x10 && (address % 2 == 0)) {
        Serial.println(F("HiTechnic Device (possible)"));
      } else {
        Serial.println(F("Unknown device"));
      }
      
      deviceCount++;
      delay(10);  // Small delay between devices
    }
    else if (error == 4) {
      Serial.print(F("  ERROR at 0x"));
      if (address < 16) Serial.print(F("0"));
      Serial.print(address, HEX);
      Serial.println(F(" - Unknown I2C error"));
    }
  }
  
  Serial.println(F("\n--- Scan Complete ---"));
  
  if (deviceCount == 0) {
    Serial.println(F("❌ NO I2C DEVICES FOUND"));
    Serial.println(F("\nTroubleshooting:"));
    Serial.println(F("1. Check SDA (Pin 20) connection"));
    Serial.println(F("2. Check SCL (Pin 21) connection"));
    Serial.println(F("3. Check common GND"));
    Serial.println(F("4. Verify 12V power to controllers"));
    Serial.println(F("5. Pin 22 should connect via 10kΩ to first controller Pin 5"));
    Serial.println(F("6. Check for loose connections"));
  } else {
    Serial.print(F("✓ Found "));
    Serial.print(deviceCount);
    Serial.println(F(" device(s) on I2C bus"));
    
    Serial.println(F("\nExpected devices:"));
    Serial.println(F("  Motor Controllers: 0x01, 0x02, 0x03"));
    Serial.println(F("  Servo Controller:  0x04"));
  }
  
  Serial.println(F("\nNext scan in 5 seconds..."));
  Serial.println(F("========================================\n"));
  delay(5000);
}
