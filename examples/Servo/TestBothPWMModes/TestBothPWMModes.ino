/*
  TestBothPWMModes - Test 0xAA (no timeout) vs 0x00 (with timeout) PWM modes
  
  This example allows you to test both PWM enable modes for the HiTechnic 
  TETRIX Servo Controller to see which works better for your setup.
  
  PWM MODES:
  - 0xAA = Enable without timeout (no need for periodic refresh)
  - 0x00 = Enable with 10-second timeout (requires periodic refresh)
  
  USAGE:
  1. Upload this sketch
  2. Open Serial Monitor at 9600 baud
  3. Send 'A' to test 0xAA mode (no timeout)
  4. Send '0' to test 0x00 mode (with timeout)
  5. Servos will sweep continuously on channels 1 and 2
  
  HARDWARE:
  - HiTechnic Servo Controller at I2C address 0x04
  - Servos on channels 1 and 2
  - Pin 22 must be HIGH for daisy chain
  
  Created: November 29, 2025
*/

#include <HiTechnicServo.h>

HiTechnicServo servoController(0x04);

uint8_t currentPWMMode = 0xAA; // Default to no-timeout mode
unsigned long lastPWMRefresh = 0;
unsigned long lastPrint = 0;
bool testRunning = false;
int sweepAngle = 0;
int sweepDirection = 1;

void setup() {
  Serial.begin(9600);
  delay(1000);
  
  Serial.println("=== PWM Mode Comparison Test ===");
  Serial.println();
  Serial.println("CRITICAL: Ensure Pin 22 is HIGH for daisy chain!");
  
  // CRITICAL: Enable Pin 22 for daisy chain addressing
  pinMode(22, OUTPUT);
  digitalWrite(22, HIGH);
  Serial.println("Pin 22 enabled");
  Serial.println();
  
  Serial.println("Commands:");
  Serial.println("  'A' or 'a' = Test 0xAA mode (no timeout)");
  Serial.println("  '0'        = Test 0x00 mode (with timeout + refresh)");
  Serial.println("  'S' or 's' = Stop test");
  Serial.println();
  Serial.println("Ready! Send a command to begin...");
  Serial.println();
}

void loop() {
  // Check for serial commands
  if (Serial.available() > 0) {
    char cmd = Serial.read();
    
    if (cmd == 'A' || cmd == 'a') {
      startTest(0xAA);
    } else if (cmd == '0') {
      startTest(0x00);
    } else if (cmd == 'S' || cmd == 's') {
      stopTest();
    }
  }
  
  // Run the sweep test if active
  if (testRunning) {
    // Refresh PWM if using 0x00 mode (every 5 seconds)
    if (currentPWMMode == 0x00 && millis() - lastPWMRefresh >= 5000) {
      servoController.refreshPWM();
      lastPWMRefresh = millis();
      Serial.println("[PWM Refreshed]");
    }
    
    // Update servo positions
    servoController.setServoAngle(1, sweepAngle);
    servoController.setServoAngle(2, 180 - sweepAngle);
    
    // Print status every second
    if (millis() - lastPrint >= 1000) {
      Serial.print("Angle: ");
      Serial.print(sweepAngle);
      Serial.print("° (Servo 1: ");
      Serial.print(sweepAngle);
      Serial.print("°, Servo 2: ");
      Serial.print(180 - sweepAngle);
      Serial.println("°)");
      lastPrint = millis();
    }
    
    // Update sweep angle
    sweepAngle += sweepDirection * 5;
    if (sweepAngle >= 180) {
      sweepAngle = 180;
      sweepDirection = -1;
    } else if (sweepAngle <= 0) {
      sweepAngle = 0;
      sweepDirection = 1;
    }
    
    delay(50);
  }
}

void startTest(uint8_t pwmMode) {
  currentPWMMode = pwmMode;
  testRunning = false;
  
  Serial.println();
  Serial.println("========================================");
  Serial.print("Starting test with PWM mode: 0x");
  Serial.print(pwmMode, HEX);
  
  if (pwmMode == 0xAA) {
    Serial.println(" (No Timeout)");
    Serial.println("This mode does NOT require periodic refresh");
  } else {
    Serial.println(" (10-Second Timeout)");
    Serial.println("This mode REQUIRES refresh every 5-10 seconds");
  }
  Serial.println("========================================");
  Serial.println();
  
  // Initialize controller with selected PWM mode
  Serial.println("Initializing servo controller...");
  servoController.begin(pwmMode);
  delay(1000);
  
  Serial.println("Controller initialized!");
  Serial.println("Starting continuous sweep on channels 1 & 2...");
  Serial.println("(Send 'S' to stop)");
  Serial.println();
  
  sweepAngle = 90;
  sweepDirection = 1;
  lastPWMRefresh = millis();
  lastPrint = millis();
  testRunning = true;
}

void stopTest() {
  if (!testRunning) {
    Serial.println("No test running.");
    return;
  }
  
  testRunning = false;
  
  Serial.println();
  Serial.println("========================================");
  Serial.println("Test stopped - Centering servos...");
  servoController.centerAll();
  delay(1000);
  Serial.println("Servos centered.");
  Serial.println("========================================");
  Serial.println();
  Serial.println("Send 'A' for 0xAA mode or '0' for 0x00 mode to test again.");
  Serial.println();
}
