/*
  Aggressive Servo Test - MG90D
  
  Continuously re-enables PWM and tests with extreme positions
  Designed for MG90D (4.8-6.6V, 1us deadband)
*/

#include <Wire.h>

#define SERVO_ADDR 0x04

void writeReg(uint8_t reg, uint8_t val) {
  Wire.beginTransmission(SERVO_ADDR);
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission();
}

uint8_t readReg(uint8_t reg) {
  Wire.beginTransmission(SERVO_ADDR);
  Wire.write(reg);
  Wire.endTransmission();
  Wire.requestFrom(SERVO_ADDR, (uint8_t)1);
  if (Wire.available()) return Wire.read();
  return 0;
}

void setup() {
  Serial.begin(9600);
  Serial.println("\n=== AGGRESSIVE MG90D SERVO TEST ===");
  Serial.println("This will:");
  Serial.println("- Keep PWM enabled continuously");
  Serial.println("- Use extreme position values");
  Serial.println("- Test on ALL 6 ports");
  Serial.println();
  
  Wire.begin();
  delay(200);
  
  Serial.println("Reading controller...");
  Serial.print("Version: ");
  Serial.println(readReg(0x00));
  
  Serial.println("\nEnabling PWM (0xAA = no timeout)...");
  writeReg(0x48, 0xAA);
  delay(50);
  Serial.print("PWM Enable readback: 0x");
  Serial.println(readReg(0x48), HEX);
  
  Serial.println("\nSetting step time = 0 (FASTEST)...");
  writeReg(0x41, 0);
  
  Serial.println("\n=== TESTING ALL 6 PORTS ===");
  Serial.println("Watch for ANY movement on ANY port!\n");
  
  for (int port = 1; port <= 6; port++) {
    uint8_t reg = 0x41 + port; // Servo 1=0x42, 2=0x43, etc.
    
    Serial.print("PORT ");
    Serial.print(port);
    Serial.println(":");
    
    // Extreme positions
    Serial.println("  -> 0 (0.75ms)");
    writeReg(reg, 0);
    delay(500);
    
    Serial.println("  -> 85 (~1ms)");
    writeReg(reg, 85);
    delay(500);
    
    Serial.println("  -> 127 (1.5ms CENTER)");
    writeReg(reg, 127);
    delay(500);
    
    Serial.println("  -> 170 (~2ms)");
    writeReg(reg, 170);
    delay(500);
    
    Serial.println("  -> 255 (2.25ms)");
    writeReg(reg, 255);
    delay(500);
    
    Serial.println("  -> 127 (CENTER)");
    writeReg(reg, 127);
    delay(500);
    
    Serial.println();
  }
  
  Serial.println("=== Starting Continuous Sweep on Port 1 ===");
  Serial.println("If servo not moving, likely causes:");
  Serial.println("1. Servo not connected or on wrong port");
  Serial.println("2. 12V battery not connected/on");
  Serial.println("3. Servo controller hardware fault");
  Serial.println("4. Wrong servo type (should see SOME twitch)");
  Serial.println();
}

void loop() {
  static unsigned long lastTime = 0;
  static uint8_t pos = 50;  // Start at ~1ms
  static int dir = 1;
  
  // Re-enable PWM every second to prevent timeout
  static unsigned long lastEnable = 0;
  if (millis() - lastEnable > 1000) {
    writeReg(0x48, 0xAA);
    lastEnable = millis();
    Serial.println("[PWM re-enabled]");
  }
  
  if (millis() - lastTime > 50) {
    writeReg(0x42, pos);  // Servo 1 position
    
    Serial.print("Pos: ");
    Serial.print(pos);
    Serial.print(" (");
    
    // Calculate pulse width: 0-255 maps to 0.75-2.25ms
    float pulse_ms = 0.75 + (pos / 255.0) * 1.5;
    Serial.print(pulse_ms, 2);
    Serial.print("ms) ");
    
    // Show if in MG90D range (1-2ms)
    if (pos >= 43 && pos <= 212) {
      Serial.print("✓ IN MG90D RANGE");
    }
    Serial.println();
    
    pos += dir * 2;
    
    if (pos >= 200 || pos <= 50) {
      dir = -dir;
      Serial.println("--- SWEEP DIRECTION CHANGED ---");
    }
    
    lastTime = millis();
  }
}
