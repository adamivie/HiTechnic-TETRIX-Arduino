/*
  WorkingServoControl - Confirmed working servo control for HiTechnic TETRIX Servo Controller
  
  This example demonstrates the confirmed working approach for controlling servos
  with the HiTechnic TETRIX Servo Controller (NSR1038).
  
  KEY FINDINGS:
  - PWM Enable must use 0x00 (enable with 10-second timeout) NOT 0xAA
  - PWM needs periodic refresh every 5-10 seconds to prevent timeout
  - The library now handles this automatically in begin()
  - For long-running applications, call refreshPWM() every 5-10 seconds
  
  HARDWARE:
  - HiTechnic Servo Controller at I2C address 0x04 (4th in daisy chain)
  - MG90D metal gear servos on channels 1 and 2
  - Pin 22 must be HIGH (via 10kΩ resistor to first controller Pin 5)
  
  WIRING:
  - Arduino Mega Pin 20 (SDA) → Controller SDA
  - Arduino Mega Pin 21 (SCL) → Controller SCL
  - Arduino Mega Pin 22 → First Controller Pin 5 (via 10kΩ resistor)
  - GND → GND
  - 6V Power → Servo Controller
  
  Created: November 2025
  Confirmed Working: November 29, 2025
*/

#include <HiTechnicServo.h>

// Create servo controller at address 0x04
HiTechnicServo servoController(0x04);

unsigned long lastPWMRefresh = 0;
const unsigned long PWM_REFRESH_INTERVAL = 5000; // Refresh every 5 seconds

void setup() {
  Serial.begin(9600);
  delay(1000);
  
  Serial.println("=== Working Servo Control Example ===");
  Serial.println();
  
  // CRITICAL: Enable Pin 22 for daisy chain addressing
  pinMode(22, OUTPUT);
  digitalWrite(22, HIGH);
  Serial.println("Pin 22 enabled for daisy chain");
  
  // Initialize servo controller
  // This now uses PWM enable 0x00 (with timeout) which is confirmed to work
  servoController.begin();
  Serial.println("Servo controller initialized");
  Serial.println("PWM enabled with 10-second timeout mode");
  Serial.println();
  
  delay(1000);
  
  // Center all servos
  Serial.println("Centering all servos...");
  servoController.centerAll();
  delay(2000);
  
  Serial.println("Ready! Starting servo sweep...");
  Serial.println();
}

void loop() {
  // Refresh PWM enable every 5 seconds to prevent timeout
  if (millis() - lastPWMRefresh >= PWM_REFRESH_INTERVAL) {
    servoController.refreshPWM();
    lastPWMRefresh = millis();
    Serial.println("PWM refreshed");
  }
  
  // Demonstrate servo control on channels 1 and 2
  
  // Sweep servo 1 from 0° to 180° while servo 2 does opposite
  Serial.println("Sweep: Servo 1 (0° to 180°), Servo 2 (180° to 0°)");
  for (int angle = 0; angle <= 180; angle += 5) {
    servoController.setServoAngle(1, angle);
    servoController.setServoAngle(2, 180 - angle);
    
    Serial.print("Servo 1: ");
    Serial.print(angle);
    Serial.print("°, Servo 2: ");
    Serial.print(180 - angle);
    Serial.println("°");
    
    delay(100);
  }
  
  delay(1000);
  
  // Return to center
  Serial.println("Returning to center...");
  servoController.centerServo(1);
  servoController.centerServo(2);
  
  delay(2000);
  
  // Demonstrate position control (0-255 raw values)
  Serial.println("Position control: Moving to specific positions");
  
  servoController.setServoPosition(1, 50);  // Near minimum
  servoController.setServoPosition(2, 200); // Near maximum
  Serial.println("Servo 1: Position 50, Servo 2: Position 200");
  delay(1500);
  
  servoController.setServoPosition(1, 200); // Near maximum
  servoController.setServoPosition(2, 50);  // Near minimum
  Serial.println("Servo 1: Position 200, Servo 2: Position 50");
  delay(1500);
  
  // Back to center
  servoController.centerAll();
  Serial.println("Both servos centered");
  Serial.println();
  
  delay(2000);
}
