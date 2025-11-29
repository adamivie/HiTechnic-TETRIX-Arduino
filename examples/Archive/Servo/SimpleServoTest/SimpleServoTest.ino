/*
  SIMPLE SERVO TEST - Channels 1 & 2
  
  Absolute simplest test for servos on channels 1 and 2.
  Based directly on HiTechnic specification.
*/

#include <Wire.h>

#define ADDR 0x04

void setup() {
  Serial.begin(9600);
  Serial.println("HiTechnic Servo Test - Ch1 & Ch2");
  
  Wire.begin();
  delay(100);
  
  // CRITICAL: Enable PWM (spec says default is 0xFF=disabled)
  Serial.println("Enabling PWM...");
  Wire.beginTransmission(ADDR);
  Wire.write(0x48);  // PWM enable register
  Wire.write(0x00);  // 0x00 = enable with 10sec timeout
  Wire.endTransmission();
  delay(50);
  
  // Set step time = 0 (fastest)
  Wire.beginTransmission(ADDR);
  Wire.write(0x41);  // Step time register  
  Wire.write(0);     // 0 = full speed
  Wire.endTransmission();
  delay(10);
  
  Serial.println("Moving servos...");
}

void loop() {
  static unsigned long lastTime = 0;
  static int step = 0;
  
  if (millis() - lastTime > 2000) {
    // Re-enable PWM every cycle (in case of timeout)
    Wire.beginTransmission(ADDR);
    Wire.write(0x48);
    Wire.write(0x00);
    Wire.endTransmission();
    
    switch(step) {
      case 0:
        Serial.println("Ch1=0, Ch2=255");
        // Servo 1 to 0
        Wire.beginTransmission(ADDR);
        Wire.write(0x42);
        Wire.write(0);
        Wire.endTransmission();
        // Servo 2 to 255
        Wire.beginTransmission(ADDR);
        Wire.write(0x43);
        Wire.write(255);
        Wire.endTransmission();
        break;
        
      case 1:
        Serial.println("Ch1=127, Ch2=127");
        // Both to center
        Wire.beginTransmission(ADDR);
        Wire.write(0x42);
        Wire.write(127);
        Wire.endTransmission();
        Wire.beginTransmission(ADDR);
        Wire.write(0x43);
        Wire.write(127);
        Wire.endTransmission();
        break;
        
      case 2:
        Serial.println("Ch1=255, Ch2=0");
        // Servo 1 to 255
        Wire.beginTransmission(ADDR);
        Wire.write(0x42);
        Wire.write(255);
        Wire.endTransmission();
        // Servo 2 to 0
        Wire.beginTransmission(ADDR);
        Wire.write(0x43);
        Wire.write(0);
        Wire.endTransmission();
        break;
    }
    
    step = (step + 1) % 3;
    lastTime = millis();
  }
}
