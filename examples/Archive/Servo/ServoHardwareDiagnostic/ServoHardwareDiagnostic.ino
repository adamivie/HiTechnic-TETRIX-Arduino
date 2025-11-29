/*
  Servo Hardware Diagnostic
  
  Tests servo controller PWM output and helps diagnose why servo isn't moving.
  Tests all 6 servo ports to find if any are working.
*/

#include "HiTechnicServo.h"

#define SERVO_ADDR 0x04

HiTechnicServo servos(SERVO_ADDR);

void setup() {
  Serial.begin(9600);
  while (!Serial);
  
  Serial.println("\n=== Servo Hardware Diagnostic ===");
  Serial.println("Controller Address: 0x04");
  Serial.println();
  
  // Initialize
  servos.begin();
  delay(100);
  
  Serial.println("Reading controller info...");
  uint8_t version = servos.readVersion();
  Serial.print("Version: ");
  Serial.println(version);
  
  uint8_t status = servos.readStatus();
  Serial.print("Status: ");
  Serial.println(status);
  
  Serial.println();
  Serial.println("=== Testing ALL 6 Servo Ports ===");
  Serial.println("Watch each servo port for movement...");
  Serial.println();
  
  // Test each servo port
  for (int servo = 1; servo <= 6; servo++) {
    Serial.print("Testing Servo Port ");
    Serial.print(servo);
    Serial.println(":");
    
    // Move to 0
    Serial.println("  Position: 0");
    servos.setServoPosition(servo, 0);
    delay(1000);
    
    // Move to center
    Serial.println("  Position: 127 (center)");
    servos.setServoPosition(servo, 127);
    delay(1000);
    
    // Move to max
    Serial.println("  Position: 255");
    servos.setServoPosition(servo, 255);
    delay(1000);
    
    // Read back position
    uint8_t pos = servos.getServoPosition(servo);
    Serial.print("  Read back: ");
    Serial.println(pos);
    
    Serial.println();
    delay(500);
  }
  
  Serial.println("=== Port Test Complete ===");
  Serial.println();
  Serial.println("TROUBLESHOOTING CHECKLIST:");
  Serial.println("1. MG90D servo power:");
  Serial.println("   - MG90D requires 4.8-6V");
  Serial.println("   - HiTechnic outputs regulated 6V (check documentation)");
  Serial.println("   - Verify 12V battery is connected and ON");
  Serial.println();
  Serial.println("2. Servo wiring:");
  Serial.println("   - Brown/Black = Ground");
  Serial.println("   - Red = Power (6V)");
  Serial.println("   - Orange/Yellow = Signal (PWM)");
  Serial.println("   - Try different ports (1-6)");
  Serial.println();
  Serial.println("3. Servo type:");
  Serial.println("   - MG90D is standard PWM servo (good)");
  Serial.println("   - NOT continuous rotation");
  Serial.println("   - Try a different servo to test");
  Serial.println();
  Serial.println("4. Controller outputs:");
  Serial.println("   - PWM should be 0.75-2.25ms");
  Serial.println("   - MG90D accepts 1-2ms");
  Serial.println("   - Check with oscilloscope if available");
  Serial.println();
  
  Serial.println("=== Starting Continuous Test on Port 1 ===");
  Serial.println("Servo 1 will sweep continuously...");
  Serial.println();
}

void loop() {
  static uint8_t pos = 0;
  static int8_t dir = 1;
  static unsigned long lastMove = 0;
  
  if (millis() - lastMove > 20) {
    servos.setServoPosition(1, pos);
    
    Serial.print("Servo 1: ");
    Serial.print(pos);
    Serial.print(" (");
    Serial.print(map(pos, 0, 255, 0, 180));
    Serial.println(" degrees)");
    
    pos += dir * 5;
    
    if (pos >= 250 || pos <= 5) {
      dir = -dir;
      Serial.println("--- Direction changed ---");
    }
    
    lastMove = millis();
  }
}
