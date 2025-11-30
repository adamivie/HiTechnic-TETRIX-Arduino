/*
  Controller Configuration Helper - Interactive Version
  
  This sketch helps you identify and configure your daisy chain setup
  by asking YOU to tell it what each controller is, rather than trying
  to auto-detect (which can be unreliable due to I2C communication issues).
  
  USAGE:
  1. Connect all controllers in your daisy chain
  2. Upload this sketch
  3. Open Serial Monitor at 115200 baud
  4. Answer the questions about each controller
  5. Copy the generated code into your sketch
  
  This is more reliable than auto-detection!
  
  Created: November 29, 2025
  Part of HiTechnic TETRIX Arduino Library
*/

#include <Wire.h>

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("========================================");
  Serial.println("  Controller Configuration Helper");
  Serial.println("  Interactive Setup");
  Serial.println("========================================");
  Serial.println();
  
  // CRITICAL: Enable Pin 22 for daisy chain addressing
  pinMode(22, OUTPUT);
  digitalWrite(22, HIGH);
  Serial.println("[✓] Pin 22 enabled for daisy chain");
  Serial.println();
  
  Wire.begin();
  delay(100);
  
  Serial.println("Scanning I2C bus for controllers...");
  Serial.println();
  delay(500);
  
  // Scan for controllers at addresses 0x01 to 0x04
  int foundCount = 0;
  uint8_t addresses[4] = {0};
  
  for (uint8_t addr = 0x01; addr <= 0x04; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      addresses[foundCount] = addr;
      foundCount++;
    }
  }
  
  if (foundCount == 0) {
    Serial.println("❌ ERROR: No controllers found!");
    Serial.println();
    Serial.println("Troubleshooting:");
    Serial.println("  1. Check Pin 22 has 10kΩ resistor to first controller Pin 5");
    Serial.println("  2. Verify all controllers are powered");
    Serial.println("  3. Check I2C wiring (SDA=Pin20, SCL=Pin21)");
    Serial.println("  4. Ensure GND is connected");
    return;
  }
  
  Serial.print("✓ Found ");
  Serial.print(foundCount);
  Serial.println(" controller(s)");
  Serial.println();
  
  // Show addresses found
  Serial.println("Detected I2C Addresses:");
  for (int i = 0; i < foundCount; i++) {
    Serial.print("  Position ");
    Serial.print(i + 1);
    Serial.print(": 0x");
    if (addresses[i] < 0x10) Serial.print("0");
    Serial.print(addresses[i], HEX);
    
    // Show version if readable
    uint8_t version = readRegister(addresses[i], 0x00);
    Serial.print(" (Version: ");
    Serial.print(version);
    Serial.println(")");
  }
  
  Serial.println();
  Serial.println("========================================");
  Serial.println();
  Serial.println("Now, I need YOUR help to identify each controller!");
  Serial.println();
  Serial.println("Please look at your physical setup and answer these questions:");
  Serial.println();
  
  // Interactive identification
  String types[4];
  for (int i = 0; i < foundCount; i++) {
    Serial.println("----------------------------------------");
    Serial.print("Position ");
    Serial.print(i + 1);
    Serial.print(" (Address 0x");
    if (addresses[i] < 0x10) Serial.print("0");
    Serial.print(addresses[i], HEX);
    Serial.println("):");
    Serial.println();
    Serial.println("What type of controller is this?");
    Serial.println("  Type 'M' for Motor Controller (NMO1038)");
    Serial.println("  Type 'S' for Servo Controller (NSR1038)");
    Serial.println();
    Serial.print("Enter M or S: ");
    
    // Wait for user input
    while (Serial.available() > 0) {
      Serial.read(); // Clear buffer
    }
    
    char input = ' ';
    while (input != 'M' && input != 'm' && input != 'S' && input != 's') {
      while (Serial.available() == 0) {
        delay(100);
      }
      input = Serial.read();
    }
    
    if (input == 'M' || input == 'm') {
      types[i] = "Motor";
      Serial.println("M");
      Serial.println("  → Motor Controller");
    } else {
      types[i] = "Servo";
      Serial.println("S");
      Serial.println("  → Servo Controller");
    }
    Serial.println();
  }
  
  // Generate configuration code
  Serial.println();
  Serial.println("========================================");
  Serial.println("  Your Configuration Summary");
  Serial.println("========================================");
  Serial.println();
  
  for (int i = 0; i < foundCount; i++) {
    Serial.print("Position ");
    Serial.print(i + 1);
    Serial.print(" (0x");
    if (addresses[i] < 0x10) Serial.print("0");
    Serial.print(addresses[i], HEX);
    Serial.print("): ");
    Serial.print(types[i]);
    Serial.println(" Controller");
  }
  
  Serial.println();
  Serial.println("========================================");
  Serial.println("  Generated Code for Your Sketch");
  Serial.println("========================================");
  Serial.println();
  Serial.println("Copy and paste this into your Arduino sketch:");
  Serial.println();
  Serial.println("```cpp");
  Serial.println("#include <HiTechnicMotor.h>");
  Serial.println("#include <HiTechnicServo.h>");
  Serial.println();
  
  // Count each type
  int motorCount = 0;
  int servoCount = 0;
  
  // Generate declarations
  for (int i = 0; i < foundCount; i++) {
    if (types[i] == "Motor") {
      motorCount++;
      Serial.print("HiTechnicMotor motor");
      Serial.print(motorCount);
      Serial.print("(0x");
      if (addresses[i] < 0x10) Serial.print("0");
      Serial.print(addresses[i], HEX);
      Serial.print(");  // Position ");
      Serial.print(i + 1);
      Serial.println(" in chain");
    } else {
      servoCount++;
      Serial.print("HiTechnicServo servo");
      Serial.print(servoCount);
      Serial.print("(0x");
      if (addresses[i] < 0x10) Serial.print("0");
      Serial.print(addresses[i], HEX);
      Serial.print(");  // Position ");
      Serial.print(i + 1);
      Serial.println(" in chain");
    }
  }
  
  Serial.println();
  Serial.println("void setup() {");
  Serial.println("  // CRITICAL: Enable daisy chain");
  Serial.println("  pinMode(22, OUTPUT);");
  Serial.println("  digitalWrite(22, HIGH);");
  Serial.println("  ");
  Serial.println("  Wire.begin();");
  Serial.println("  ");
  
  // Generate begin() calls
  motorCount = 0;
  servoCount = 0;
  for (int i = 0; i < foundCount; i++) {
    if (types[i] == "Motor") {
      motorCount++;
      Serial.print("  motor");
      Serial.print(motorCount);
      Serial.println(".begin();");
    } else {
      servoCount++;
      Serial.print("  servo");
      Serial.print(servoCount);
      Serial.println(".begin();");
    }
  }
  
  Serial.println("}");
  Serial.println();
  Serial.println("void loop() {");
  Serial.println("  // Your code here");
  
  // Add some example code
  if (motorCount > 0) {
    Serial.println("  ");
    Serial.println("  // Example: Run motors at 50% power");
    for (int i = 1; i <= motorCount; i++) {
      Serial.print("  motor");
      Serial.print(i);
      Serial.println(".setMotorPower(1, 50);  // Motor 1 on controller");
    }
  }
  
  if (servoCount > 0) {
    Serial.println("  ");
    Serial.println("  // Example: Move servos to 90 degrees");
    for (int i = 1; i <= servoCount; i++) {
      Serial.print("  servo");
      Serial.print(i);
      Serial.println(".setServoAngle(1, 90);  // Servo 1 on controller");
    }
  }
  
  Serial.println("}");
  Serial.println("```");
  Serial.println();
  Serial.println("========================================");
  Serial.println();
  Serial.println("Configuration complete!");
  Serial.println("Copy the code above into your sketch.");
  Serial.println();
}

void loop() {
  // Nothing to do
}

uint8_t readRegister(uint8_t address, uint8_t reg) {
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.endTransmission();
  
  Wire.requestFrom(address, (uint8_t)1);
  if (Wire.available()) {
    return Wire.read();
  }
  return 0;
}
