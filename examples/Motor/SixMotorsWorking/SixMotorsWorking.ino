/*
 * Six Independent Motors - WORKING SOLUTION
 * 
 * Controls 6 DC motors across 3 HiTechnic motor controllers
 * 
 * HARDWARE SETUP (VERIFIED WORKING):
 * - Arduino Pin 20 (SDA) → Pin 6 of all controllers (daisy chain)
 * - Arduino Pin 21 (SCL) → Pin 5 of all controllers (daisy chain)
 * - Arduino Pin 22 → 10kΩ resistor → Pin 5 of FIRST controller
 * - Arduino GND → Pin 1/2 of all controllers
 * 
 * CONTROLLERS:
 * - Controller 1 at address 0x01: Motors 1 & 2
 * - Controller 2 at address 0x02: Motors 3 & 4
 * - Controller 3 at address 0x03: Motors 5 & 6
 * 
 * KEY FIX: Setting MODE register before POWER register eliminates crosstalk
 */

#include <Wire.h>

#define ANALOG_DETECT_PIN 22

// I2C Addresses
#define CTRL1_ADDR 0x01
#define CTRL2_ADDR 0x02
#define CTRL3_ADDR 0x03

// Registers
#define REG_MOTOR1_MODE 0x44
#define REG_MOTOR1_POWER 0x45
#define REG_MOTOR2_POWER 0x46
#define REG_MOTOR2_MODE 0x47

#define MODE_POWER_CONTROL 0x00

class MotorController {
private:
  byte address;
  
  void writeRegister(byte reg, byte value) {
    Wire.beginTransmission(address);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
  }

public:
  MotorController(byte addr) : address(addr) {}
  
  void setMotor1(int8_t power) {
    power = constrain(power, -100, 100);
    writeRegister(REG_MOTOR1_MODE, MODE_POWER_CONTROL);  // MODE first!
    writeRegister(REG_MOTOR1_POWER, (byte)power);
  }
  
  void setMotor2(int8_t power) {
    power = constrain(power, -100, 100);
    writeRegister(REG_MOTOR2_MODE, MODE_POWER_CONTROL);  // MODE first!
    writeRegister(REG_MOTOR2_POWER, (byte)power);
  }
  
  void setBothMotors(int8_t power1, int8_t power2) {
    setMotor1(power1);
    setMotor2(power2);
  }
  
  void stop() {
    setMotor1(0);
    setMotor2(0);
  }
};

// Create controller objects
MotorController controller1(CTRL1_ADDR);
MotorController controller2(CTRL2_ADDR);
MotorController controller3(CTRL3_ADDR);

void setup() {
  Serial.begin(9600);
  while (!Serial) { delay(10); }
  
  // Enable 5V analog detection for daisy chain addressing
  pinMode(ANALOG_DETECT_PIN, OUTPUT);
  digitalWrite(ANALOG_DETECT_PIN, HIGH);
  
  Wire.begin();
  Wire.setClock(100000);
  
  Serial.println("\n=== Six Independent Motors - Working! ===");
  Serial.println();
  Serial.println("Controllers:");
  Serial.println("  Controller 1 (0x01): Motor 1 & Motor 2");
  Serial.println("  Controller 2 (0x02): Motor 3 & Motor 4");
  Serial.println("  Controller 3 (0x03): Motor 5 & Motor 6");
  Serial.println();
  Serial.println("Testing each motor independently at 30%...\n");
  
  delay(2000);
  
  // Test each motor individually
  Serial.println("Motor 1 (C1M1)");
  controller1.setMotor1(30);
  delay(2000);
  controller1.stop();
  delay(1000);
  
  Serial.println("Motor 2 (C1M2)");
  controller1.setMotor2(30);
  delay(2000);
  controller1.stop();
  delay(1000);
  
  Serial.println("Motor 3 (C2M1)");
  controller2.setMotor1(30);
  delay(2000);
  controller2.stop();
  delay(1000);
  
  Serial.println("Motor 4 (C2M2)");
  controller2.setMotor2(30);
  delay(2000);
  controller2.stop();
  delay(1000);
  
  Serial.println("Motor 5 (C3M1)");
  controller3.setMotor1(30);
  delay(2000);
  controller3.stop();
  delay(1000);
  
  Serial.println("Motor 6 (C3M2)");
  controller3.setMotor2(30);
  delay(2000);
  controller3.stop();
  delay(1000);
  
  Serial.println("\n=== All motors tested successfully! ===");
  Serial.println("Now running demo patterns...\n");
}

void loop() {
  // Demo 1: Sequential activation
  Serial.println("Demo: Sequential Motors");
  for (int i = 1; i <= 6; i++) {
    switch(i) {
      case 1: controller1.setMotor1(40); break;
      case 2: controller1.setMotor2(40); break;
      case 3: controller2.setMotor1(40); break;
      case 4: controller2.setMotor2(40); break;
      case 5: controller3.setMotor1(40); break;
      case 6: controller3.setMotor2(40); break;
    }
    delay(1000);
  }
  
  // Stop all
  controller1.stop();
  controller2.stop();
  controller3.stop();
  delay(2000);
  
  // Demo 2: All motors together
  Serial.println("Demo: All Motors Together (50%)");
  controller1.setBothMotors(50, 50);
  controller2.setBothMotors(50, 50);
  controller3.setBothMotors(50, 50);
  delay(3000);
  
  // Stop all
  controller1.stop();
  controller2.stop();
  controller3.stop();
  delay(2000);
  
  // Demo 3: Wave pattern
  Serial.println("Demo: Wave Pattern");
  for (int power = 0; power <= 60; power += 10) {
    controller1.setBothMotors(power, 60 - power);
    controller2.setBothMotors(60 - power, power);
    controller3.setBothMotors(power, 60 - power);
    delay(500);
  }
  
  // Stop all
  controller1.stop();
  controller2.stop();
  controller3.stop();
  
  Serial.println("\nDemo complete. Pausing 5 seconds...\n");
  delay(5000);
}
