// I2C Scanner - Find I2C devices on the bus
// Use this to verify your HiTechnic motor controller is connected
// and to find its I2C address

#include <Wire.h>

void setup() {
  // Setup Pin 22 for analog detection (required for daisy chain addressing)
  pinMode(22, OUTPUT);
  digitalWrite(22, HIGH);  // 5V to first controller Pin 5 via 10kΩ resistor
  
  Wire.begin();
  Serial.begin(9600);
  while (!Serial);
  Serial.println("\nI2C Scanner - HiTechnic Motor Controller Finder");
  Serial.println("Pin 22 analog detection enabled for daisy chain");
  Serial.println("Scanning...\n");
}

void loop() {
  byte error, address;
  int deviceCount = 0;

  Serial.println("Starting I2C scan...");
  
  for(address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      
      // Check if it's likely a HiTechnic controller
      if (address == 0x02) {
        Serial.print("  <- This is likely your HiTechnic Motor Controller!");
      }
      
      Serial.println();
      deviceCount++;
    }
    else if (error == 4) {
      Serial.print("Unknown error at address 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
    }    
  }
  
  if (deviceCount == 0) {
    Serial.println("No I2C devices found.\n");
    Serial.println("Check connections:");
    Serial.println("- SDA (Pin 20) connected?");
    Serial.println("- SCL (Pin 21) connected?");
    Serial.println("- Common GND connected?");
    Serial.println("- Controller powered (12V)?");
  }
  else {
    Serial.print("\nFound ");
    Serial.print(deviceCount);
    Serial.println(" device(s).");
  }
  
  Serial.println("\nScanning again in 5 seconds...\n");
  delay(5000);
}
