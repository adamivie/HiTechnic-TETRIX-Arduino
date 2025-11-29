/*
  Simple Servo Controller Test
  Tests basic I2C communication with servo controller at 0x04
*/

#include <Wire.h>

#define SERVO_ADDR 0x04

void setup() {
  Serial.begin(9600);
  while (!Serial);
  
  Wire.begin();
  delay(200);
  
  Serial.println("=== Servo Controller I2C Test ===");
  Serial.println("Address: 0x04");
  Serial.println();
  
  // Test 1: Read version (reg 0x00)
  Serial.print("Reading version (0x00): ");
  Wire.beginTransmission(SERVO_ADDR);
  Wire.write(0x00);
  byte error = Wire.endTransmission();
  Serial.print("endTransmission = ");
  Serial.println(error);
  
  if (error == 0) {
    Wire.requestFrom(SERVO_ADDR, 1);
    if (Wire.available()) {
      Serial.print("Version: ");
      Serial.println(Wire.read());
    }
  }
  
  delay(100);
  
  // Test 2: Read manufacturer string (0x08-0x0F)
  Serial.print("\nReading manufacturer (0x08-0x0F): ");
  Wire.beginTransmission(SERVO_ADDR);
  Wire.write(0x08);
  error = Wire.endTransmission();
  Serial.print("endTransmission = ");
  Serial.println(error);
  
  if (error == 0) {
    Wire.requestFrom(SERVO_ADDR, 8);
    Serial.print("Manufacturer: ");
    while (Wire.available()) {
      char c = Wire.read();
      if (c >= 32 && c < 127) Serial.print(c);
    }
    Serial.println();
  }
  
  delay(100);
  
  // Test 3: Read sensor type (0x10-0x17)
  Serial.print("\nReading sensor type (0x10-0x17): ");
  Wire.beginTransmission(SERVO_ADDR);
  Wire.write(0x10);
  error = Wire.endTransmission();
  Serial.print("endTransmission = ");
  Serial.println(error);
  
  if (error == 0) {
    Wire.requestFrom(SERVO_ADDR, 8);
    Serial.print("Sensor Type: ");
    while (Wire.available()) {
      char c = Wire.read();
      if (c >= 32 && c < 127) Serial.print(c);
    }
    Serial.println();
  }
  
  delay(100);
  
  // Test 4: Write to PWM enable (0x48)
  Serial.print("\nWriting 0xAA to PWM enable (0x48): ");
  Wire.beginTransmission(SERVO_ADDR);
  Wire.write(0x48);
  Wire.write(0xAA);
  error = Wire.endTransmission();
  Serial.print("endTransmission = ");
  Serial.println(error);
  
  if (error == 0) {
    Serial.println("Write successful!");
    
    // Read back
    delay(10);
    Wire.beginTransmission(SERVO_ADDR);
    Wire.write(0x48);
    Wire.endTransmission();
    Wire.requestFrom(SERVO_ADDR, 1);
    if (Wire.available()) {
      Serial.print("Read back: 0x");
      Serial.println(Wire.read(), HEX);
    }
  } else {
    Serial.print("Write failed! Error: ");
    Serial.println(error);
  }
  
  delay(100);
  
  // Test 5: Write servo position (0x42)
  Serial.print("\nWriting 127 to Servo 1 position (0x42): ");
  Wire.beginTransmission(SERVO_ADDR);
  Wire.write(0x42);
  Wire.write(127);
  error = Wire.endTransmission();
  Serial.print("endTransmission = ");
  Serial.println(error);
  
  if (error == 0) {
    Serial.println("Write successful!");
  } else {
    Serial.print("Write failed! Error: ");
    Serial.println(error);
  }
  
  Serial.println("\n=== Test Complete ===");
}

void loop() {
  // Nothing
}
