/*
  Controller Configuration Identifier
  
  This sketch helps you identify which controllers are in which positions
  in your daisy chain. It attempts to determine if each detected I2C device
  is a motor controller or servo controller by reading their characteristics.
  
  USAGE:
  1. Connect all controllers in your daisy chain
  2. Upload this sketch
  3. Open Serial Monitor at 115200 baud
  4. The sketch will identify each controller type and suggest code configuration
  
  REQUIREMENTS:
  - Arduino Mega 2560
  - HiTechnic controllers in daisy chain
  - Pin 22 HIGH for daisy chain addressing
  
  Created: November 29, 2025
  Part of HiTechnic TETRIX Arduino Library
*/

#include <Wire.h>

// Register addresses
#define REG_VERSION       0x00
#define REG_MANUFACTURER  0x08
#define REG_SENSOR_TYPE   0x10

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("========================================");
  Serial.println("  Controller Configuration Identifier");
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
  
  Serial.print("Found ");
  Serial.print(foundCount);
  Serial.println(" controller(s)");
  Serial.println();
  Serial.println("========================================");
  Serial.println();
  
  // Identify each controller
  for (int i = 0; i < foundCount; i++) {
    uint8_t addr = addresses[i];
    identifyController(addr, i + 1);
    Serial.println();
  }
  
  // Generate code suggestion
  Serial.println("========================================");
  Serial.println("  Suggested Code Configuration");
  Serial.println("========================================");
  Serial.println();
  
  Serial.println("Add this to your sketch:");
  Serial.println();
  Serial.println("```cpp");
  Serial.println("#include <HiTechnicMotor.h>");
  Serial.println("#include <HiTechnicServo.h>");
  Serial.println();
  
  // Generate declarations
  for (int i = 0; i < foundCount; i++) {
    uint8_t addr = addresses[i];
    String type = getControllerType(addr);
    
    if (type == "Motor") {
      Serial.print("HiTechnicMotor motor");
      Serial.print(i + 1);
      Serial.print("(0x");
      if (addr < 0x10) Serial.print("0");
      Serial.print(addr, HEX);
      Serial.print(");  // Position ");
      Serial.print(i + 1);
      Serial.println(" in chain");
    } else if (type == "Servo") {
      Serial.print("HiTechnicServo servo");
      Serial.print(i + 1);
      Serial.print("(0x");
      if (addr < 0x10) Serial.print("0");
      Serial.print(addr, HEX);
      Serial.print(");  // Position ");
      Serial.print(i + 1);
      Serial.println(" in chain");
    } else {
      Serial.print("// Unknown controller at 0x");
      if (addr < 0x10) Serial.print("0");
      Serial.println(addr, HEX);
    }
  }
  
  Serial.println();
  Serial.println("void setup() {");
  Serial.println("  // CRITICAL: Enable daisy chain");
  Serial.println("  pinMode(22, OUTPUT);");
  Serial.println("  digitalWrite(22, HIGH);");
  Serial.println("  ");
  
  // Generate begin() calls
  for (int i = 0; i < foundCount; i++) {
    String type = getControllerType(addresses[i]);
    if (type == "Motor") {
      Serial.print("  motor");
      Serial.print(i + 1);
      Serial.println(".begin();");
    } else if (type == "Servo") {
      Serial.print("  servo");
      Serial.print(i + 1);
      Serial.println(".begin();");
    }
  }
  
  Serial.println("}");
  Serial.println("```");
  Serial.println();
  Serial.println("========================================");
  Serial.println();
  Serial.println("Configuration complete! Copy the code above into your sketch.");
}

void loop() {
  // Nothing to do in loop
}

void identifyController(uint8_t address, int position) {
  Serial.print("Position ");
  Serial.print(position);
  Serial.print(" - Address 0x");
  if (address < 0x10) Serial.print("0");
  Serial.print(address, HEX);
  Serial.println(":");
  Serial.println("----------------------------------------");
  
  // Read version
  uint8_t version = readRegister(address, REG_VERSION);
  Serial.print("  Firmware Version: ");
  Serial.println(version);
  
  // Read manufacturer string
  Serial.print("  Manufacturer: ");
  char manufacturer[9] = {0};
  for (int i = 0; i < 8; i++) {
    manufacturer[i] = readRegister(address, REG_MANUFACTURER + i);
  }
  Serial.println(manufacturer);
  
  // Read sensor type string
  Serial.print("  Sensor Type: ");
  char sensorType[9] = {0};
  for (int i = 0; i < 8; i++) {
    sensorType[i] = readRegister(address, REG_SENSOR_TYPE + i);
  }
  Serial.println(sensorType);
  
  // Determine controller type
  String type = getControllerType(address);
  Serial.print("  Controller Type: ");
  if (type == "Motor") {
    Serial.println("🔧 DC Motor Controller (NMO1038)");
    Serial.println("    - Controls 2 DC motors");
    Serial.println("    - Has encoder inputs");
  } else if (type == "Servo") {
    Serial.println("🎛️  Servo Controller (NSR1038)");
    Serial.println("    - Controls 6 servos");
    Serial.println("    - PWM outputs");
  } else {
    Serial.println("❓ Unknown Controller");
  }
}

String getControllerType(uint8_t address) {
  /*
   * Detection based on official specifications:
   * 
   * Motor Controller (NMO1038) Register Map:
   * - 0x40-0x43: Motor 1 encoder TARGET (4 bytes, signed long, high byte first)
   * - 0x44: Motor 1 MODE (bits: Busy|Error|-|NTO|Rev|Lock|Sel1|Sel0)
   * - 0x45: Motor 1 POWER (-100 to +100, or -128 for float)
   * - 0x46: Motor 2 POWER
   * - 0x47: Motor 2 MODE
   * - 0x48-0x4B: Motor 2 encoder TARGET (4 bytes)
   * - Sensor type: "MotorCon"
   * 
   * Servo Controller (NSR1038) Register Map:
   * - 0x40: Status register
   * - 0x41: Step time (0-15)
   * - 0x42-0x47: Servo 1-6 positions (1 byte each, 0-255)
   * - 0x48: PWM enable (0xFF=disabled default, 0x00/0xAA=enabled)
   * - Sensor type: "Servo" or similar
   */
  
  // Read critical registers
  uint8_t reg0x40 = readRegister(address, 0x40);  // Motor encoder target byte 0 OR Servo status
  uint8_t reg0x41 = readRegister(address, 0x41);  // Motor encoder target byte 1 OR Servo step time
  uint8_t reg0x42 = readRegister(address, 0x42);  // Motor encoder target byte 2 OR Servo 1 position
  uint8_t reg0x44 = readRegister(address, 0x44);  // Motor 1 MODE OR Servo 3 position
  uint8_t reg0x45 = readRegister(address, 0x45);  // Motor 1 POWER OR Servo 4 position
  uint8_t reg0x46 = readRegister(address, 0x46);  // Motor 2 POWER OR Servo 5 position
  uint8_t reg0x47 = readRegister(address, 0x47);  // Motor 2 MODE OR Servo 6 position
  uint8_t reg0x48 = readRegister(address, 0x48);  // Motor 2 encoder target byte 0 OR PWM enable
  
  // Scoring system
  int servoScore = 0;
  int motorScore = 0;
  
  Serial.println("  [Detection] Register Analysis:");
  Serial.print("    0x40=0x"); Serial.print(reg0x40, HEX);
  Serial.print(" 0x41=0x"); Serial.print(reg0x41, HEX);
  Serial.print(" 0x42=0x"); Serial.print(reg0x42, HEX);
  Serial.print(" 0x44=0x"); Serial.print(reg0x44, HEX);
  Serial.println();
  Serial.print("    0x45=0x"); Serial.print(reg0x45, HEX);
  Serial.print(" 0x46=0x"); Serial.print(reg0x46, HEX);
  Serial.print(" 0x47=0x"); Serial.print(reg0x47, HEX);
  Serial.print(" 0x48=0x"); Serial.println(reg0x48, HEX);
  
  // Check for stuck reads (all same value = communication issue)
  bool allSame = (reg0x40 == reg0x41) && (reg0x41 == reg0x42) && 
                 (reg0x42 == reg0x44) && (reg0x44 == reg0x45) && 
                 (reg0x45 == reg0x46) && (reg0x46 == reg0x47) && 
                 (reg0x47 == reg0x48);
  
  if (allSame && reg0x40 != 0x00) {
    Serial.print("    WARNING: All registers = 0x");
    Serial.print(reg0x40, HEX);
    Serial.println(" (possible I2C issue or repeated version)");
    
    // Read version to compare
    uint8_t version = readRegister(address, REG_VERSION);
    if (reg0x40 == version) {
      Serial.println("    This matches version - controller may not be responding correctly");
      Serial.println("    Falling back to heuristic: assuming SERVO at position 4");
      
      // Heuristic: position 4 is commonly servo in 3-motor + 1-servo setups
      if (address == 0x04) {
        return "Servo";
      }
      return "Unknown";
    }
  }
  
  // TEST 1: PWM Enable register (0x48) - STRONGEST SERVO INDICATOR
  // Servo default is 0xFF (disabled), or 0x00/0xAA if enabled
  // Motor controller uses this for encoder target, likely 0x00 or small value
  // NOTE: If servo has been running, this might contain other values
  if (reg0x48 == 0xFF) {
    servoScore += 15;  // Very strong servo indicator
    Serial.println("    → 0x48=0xFF (PWM disabled default) +15 SERVO");
  } else if (reg0x48 == 0xAA) {
    servoScore += 15;  // Very strong servo indicator
    Serial.println("    → 0x48=0xAA (PWM enabled no timeout) +15 SERVO");
  } else if (reg0x48 == 0x00) {
    // Could be servo with PWM enabled OR motor encoder byte
    servoScore += 3;
    motorScore += 3;
    Serial.println("    → 0x48=0x00 (ambiguous) +3 both");
  } else if (reg0x48 >= 0x01 && reg0x48 <= 0x0F) {
    // Low values - more likely motor encoder
    motorScore += 5;
    Serial.println("    → 0x48<0x10 (likely motor encoder) +5 MOTOR");
  }
  
  // TEST 2: Step time register (0x41)
  // Servo: valid range 0-15
  // Motor: part of 32-bit encoder target, can be any value
  if (reg0x41 <= 15) {
    servoScore += 5;
    Serial.println("    → 0x41<=15 (valid servo step time) +5 SERVO");
  } else if (reg0x41 > 15) {
    motorScore += 3;
    Serial.println("    → 0x41>15 (likely encoder byte) +3 MOTOR");
  }
  
  // TEST 3: Motor MODE register (0x44)
  // Motor: bits are Busy|Error|-|NTO|Rev|Lock|Sel1|Sel0
  // Sel bits: 00=power, 01=speed, 02=position, 03=reset
  // So valid values are 0x00-0x03 plus possible high bits for NTO(0x80), Rev(0x10), etc.
  uint8_t selBits = reg0x44 & 0x03;  // Extract Sel1 and Sel0
  uint8_t modeBits = reg0x44 & 0xFB;  // Mask out Lock bit (not implemented)
  
  if (reg0x44 <= 0x03) {
    // Clean mode value, likely motor in power/speed/position/reset mode
    motorScore += 10;
    Serial.println("    → 0x44<=0x03 (motor mode) +10 MOTOR");
  } else if ((reg0x44 & 0x80) != 0 && selBits <= 0x03) {
    // NTO bit set (no timeout) - motor feature
    motorScore += 10;
    Serial.println("    → 0x44 has NTO bit (motor feature) +10 MOTOR");
  } else if (reg0x44 >= 50 && reg0x44 <= 200) {
    // Likely servo position (centered around 127)
    servoScore += 5;
    Serial.println("    → 0x44 in servo range (50-200) +5 SERVO");
  }
  
  // TEST 4: Motor POWER registers (0x45, 0x46)
  // Motor: -100 to +100 (0-100, or 128+value for negative), or 128 for float
  // Servo: Position 0-255
  bool power45Valid = (reg0x45 <= 100) || (reg0x45 == 128) || (reg0x45 >= 156 && reg0x45 <= 255);
  bool power46Valid = (reg0x46 <= 100) || (reg0x46 == 128) || (reg0x46 >= 156 && reg0x46 <= 255);
  
  if (power45Valid && power46Valid) {
    motorScore += 8;
    Serial.println("    → 0x45,0x46 in motor power range +8 MOTOR");
  } else if (reg0x45 >= 50 && reg0x45 <= 200 && reg0x46 >= 50 && reg0x46 <= 200) {
    servoScore += 5;
    Serial.println("    → 0x45,0x46 in servo position range +5 SERVO");
  }
  
  // TEST 5: Motor MODE register (0x47) - similar to 0x44
  if (reg0x47 <= 0x03 || ((reg0x47 & 0x80) != 0 && (reg0x47 & 0x03) <= 0x03)) {
    motorScore += 10;
    Serial.println("    → 0x47 valid motor mode +10 MOTOR");
  } else if (reg0x47 >= 50 && reg0x47 <= 200) {
    servoScore += 5;
    Serial.println("    → 0x47 in servo range +5 SERVO");
  }
  
  // TEST 6: Check if 0x42-0x47 look like servo positions
  int servoLikeCount = 0;
  for (uint8_t reg = 0x42; reg <= 0x47; reg++) {
    uint8_t val = readRegister(address, reg);
    // Servo positions: 0-255, commonly 127 (center), 255 (disabled), or 0
    if (val == 0 || val == 255 || (val >= 50 && val <= 200)) {
      servoLikeCount++;
    }
  }
  
  if (servoLikeCount >= 5) {
    servoScore += 7;
    Serial.print("    → ");
    Serial.print(servoLikeCount);
    Serial.println("/6 servo-like positions +7 SERVO");
  } else if (servoLikeCount <= 2) {
    motorScore += 5;
    Serial.print("    → ");
    Serial.print(servoLikeCount);
    Serial.println("/6 servo-like (unlikely servo) +5 MOTOR");
  }
  
  // Final decision
  Serial.print("  [Detection] Scores: Servo=");
  Serial.print(servoScore);
  Serial.print(" Motor=");
  Serial.println(motorScore);
  
  if (servoScore > motorScore) {
    Serial.println("  [Detection] → SERVO CONTROLLER");
    return "Servo";
  } else if (motorScore > servoScore) {
    Serial.println("  [Detection] → MOTOR CONTROLLER");
    return "Motor";
  } else {
    // Tie - use 0x48 as tiebreaker
    if (reg0x48 == 0xFF || reg0x48 == 0xAA || reg0x48 == 0x00) {
      Serial.println("  [Detection] → SERVO (tiebreaker: 0x48)");
      return "Servo";
    } else {
      Serial.println("  [Detection] → MOTOR (tiebreaker: default)");
      return "Motor";
    }
  }
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
