/*
  PixhawkMotorControl - Control HiTechnic TETRIX motors from Pixhawk/QGroundControl
  
  This sketch enables control of 6 motors (3 HiTechnic controllers) from a Pixhawk
  autopilot running PX4 or ArduPilot firmware. QGroundControl sends commands to
  Pixhawk, which forwards them to this Arduino via serial.
  
  Hardware Setup:
  - Arduino Mega 2560
  - Pixhawk TELEM2 → Arduino Serial1 (pins 18/19)
  - 3x HiTechnic TETRIX Motor Controllers at addresses 0x01, 0x02, 0x03
  - 10kΩ resistor from Pin 22 to first controller Pin 5
  - I2C: Pin 20 (SDA), Pin 21 (SCL)
  
  Communication Protocol:
  Commands from Pixhawk (text format):
    M1:50\n       - Set motor 1 to 50% power
    M2:-75\n      - Set motor 2 to -75% power (reverse)
    STOP\n        - Emergency stop all motors
    STATUS\n      - Request telemetry update
    RESET_ENC\n   - Reset all encoders
  
  Telemetry to Pixhawk (10Hz):
    TELEM,P1:45,E1:1234,P2:-30,E2:5678,...\n
    
  Safety Features:
  - Watchdog timer (stops motors if no command for 1 second)
  - Emergency stop command
  - Power limiting (configurable max power)
  - Serial error detection
*/

#include <HiTechnicMotor.h>

// Serial configuration
#define PIXHAWK_SERIAL Serial1  // TELEM2 on Pixhawk
#define DEBUG_SERIAL Serial      // USB for debugging
#define PIXHAWK_BAUD 57600
#define DEBUG_BAUD 9600

// Safety configuration
#define COMMAND_TIMEOUT 1000     // Stop motors if no command for 1 second
#define MAX_MOTOR_POWER 100      // Maximum allowed power (0-100)
#define TELEMETRY_RATE 100       // Send telemetry every 100ms (10Hz)
#define ACCEL_RATE 5             // Smooth acceleration rate (1-100)

// Motor controllers at I2C addresses 0x01, 0x02, 0x03
HiTechnicMotor controller1(0x01);  // Motors 1 & 2
HiTechnicMotor controller2(0x02);  // Motors 3 & 4
HiTechnicMotor controller3(0x03);  // Motors 5 & 6

// Command buffer
char cmdBuffer[64];
uint8_t cmdIndex = 0;

// Timing variables
unsigned long lastCommandTime = 0;
unsigned long lastTelemetryTime = 0;

// Motor power tracking for telemetry
int8_t motorPowers[6] = {0, 0, 0, 0, 0, 0};

void setup() {
  // Initialize debug serial
  DEBUG_SERIAL.begin(DEBUG_BAUD);
  DEBUG_SERIAL.println(F("HiTechnic TETRIX - Pixhawk Motor Control"));
  DEBUG_SERIAL.println(F("=========================================="));
  
  // Initialize Pixhawk serial
  PIXHAWK_SERIAL.begin(PIXHAWK_BAUD);
  PIXHAWK_SERIAL.println(F("ARDUINO_READY"));
  
  // Setup Pin 22 for analog detection (required for daisy chain)
  pinMode(22, OUTPUT);
  digitalWrite(22, HIGH);  // 5V to first controller Pin 5 via 10kΩ resistor
  
  DEBUG_SERIAL.println(F("Pin 22 analog detection enabled"));
  
  // Initialize motor controllers
  DEBUG_SERIAL.print(F("Initializing controllers..."));
  controller1.begin();
  controller2.begin();
  controller3.begin();
  DEBUG_SERIAL.println(F(" Done"));
  
  // Read firmware versions
  DEBUG_SERIAL.print(F("Controller 1 FW: 0x"));
  DEBUG_SERIAL.println(controller1.readVersion(), HEX);
  DEBUG_SERIAL.print(F("Controller 2 FW: 0x"));
  DEBUG_SERIAL.println(controller2.readVersion(), HEX);
  DEBUG_SERIAL.print(F("Controller 3 FW: 0x"));
  DEBUG_SERIAL.println(controller3.readVersion(), HEX);
  
  DEBUG_SERIAL.println(F("\nWaiting for commands from Pixhawk..."));
  DEBUG_SERIAL.println(F("Send 'M1:50' to test motor 1 at 50% power"));
  
  lastCommandTime = millis();
}

void loop() {
  // Check for commands from Pixhawk
  while (PIXHAWK_SERIAL.available()) {
    char c = PIXHAWK_SERIAL.read();
    
    if (c == '\n' || c == '\r') {
      if (cmdIndex > 0) {
        cmdBuffer[cmdIndex] = 0;  // Null terminate
        processCommand(cmdBuffer);
        cmdIndex = 0;
      }
    } else if (cmdIndex < sizeof(cmdBuffer) - 1) {
      cmdBuffer[cmdIndex++] = c;
    }
  }
  
  // Safety: Check for command timeout (watchdog)
  if (millis() - lastCommandTime > COMMAND_TIMEOUT) {
    emergencyStop();
    lastCommandTime = millis();  // Reset to avoid spamming
  }
  
  // Update motor smooth ramping (call frequently)
  controller1.update();
  controller2.update();
  controller3.update();
  
  // Send telemetry periodically
  if (millis() - lastTelemetryTime >= TELEMETRY_RATE) {
    sendTelemetry();
    lastTelemetryTime = millis();
  }
}

void processCommand(const char* cmd) {
  lastCommandTime = millis();  // Reset watchdog timer
  
  DEBUG_SERIAL.print(F("CMD: "));
  DEBUG_SERIAL.println(cmd);
  
  // Parse motor commands (M1-M6)
  if (cmd[0] == 'M' && cmd[1] >= '1' && cmd[1] <= '6' && cmd[2] == ':') {
    int motorNum = cmd[1] - '0';
    int power = atoi(cmd + 3);  // Parse power value after "M1:"
    
    // Limit power to safety maximum
    power = constrain(power, -MAX_MOTOR_POWER, MAX_MOTOR_POWER);
    
    setMotorPower(motorNum, power);
    
    PIXHAWK_SERIAL.print(F("OK,M"));
    PIXHAWK_SERIAL.print(motorNum);
    PIXHAWK_SERIAL.print(F(":"));
    PIXHAWK_SERIAL.println(power);
    
  // Emergency stop
  } else if (strcmp(cmd, "STOP") == 0) {
    emergencyStop();
    PIXHAWK_SERIAL.println(F("STOPPED"));
    DEBUG_SERIAL.println(F("EMERGENCY STOP"));
    
  // Status request
  } else if (strcmp(cmd, "STATUS") == 0) {
    sendTelemetry();
    
  // Reset encoders
  } else if (strcmp(cmd, "RESET_ENC") == 0) {
    controller1.resetAllEncoders();
    controller2.resetAllEncoders();
    controller3.resetAllEncoders();
    PIXHAWK_SERIAL.println(F("ENCODERS_RESET"));
    DEBUG_SERIAL.println(F("Encoders reset"));
    
  // Set all motors to same power
  } else if (strncmp(cmd, "MALL:", 5) == 0) {
    int power = atoi(cmd + 5);
    power = constrain(power, -MAX_MOTOR_POWER, MAX_MOTOR_POWER);
    
    for (int i = 1; i <= 6; i++) {
      setMotorPower(i, power);
    }
    
    PIXHAWK_SERIAL.print(F("OK,MALL:"));
    PIXHAWK_SERIAL.println(power);
    
  // Unknown command
  } else {
    PIXHAWK_SERIAL.print(F("ERROR,UNKNOWN:"));
    PIXHAWK_SERIAL.println(cmd);
    DEBUG_SERIAL.print(F("Unknown command: "));
    DEBUG_SERIAL.println(cmd);
  }
}

void setMotorPower(uint8_t motorNum, int8_t power) {
  // Map motor number (1-6) to controller and motor
  switch (motorNum) {
    case 1:
      controller1.setMotorPowerSmooth(MOTOR_1, power, ACCEL_RATE);
      motorPowers[0] = power;
      break;
    case 2:
      controller1.setMotorPowerSmooth(MOTOR_2, power, ACCEL_RATE);
      motorPowers[1] = power;
      break;
    case 3:
      controller2.setMotorPowerSmooth(MOTOR_1, power, ACCEL_RATE);
      motorPowers[2] = power;
      break;
    case 4:
      controller2.setMotorPowerSmooth(MOTOR_2, power, ACCEL_RATE);
      motorPowers[3] = power;
      break;
    case 5:
      controller3.setMotorPowerSmooth(MOTOR_1, power, ACCEL_RATE);
      motorPowers[4] = power;
      break;
    case 6:
      controller3.setMotorPowerSmooth(MOTOR_2, power, ACCEL_RATE);
      motorPowers[5] = power;
      break;
  }
  
  DEBUG_SERIAL.print(F("Motor "));
  DEBUG_SERIAL.print(motorNum);
  DEBUG_SERIAL.print(F(" → "));
  DEBUG_SERIAL.print(power);
  DEBUG_SERIAL.println(F("%"));
}

void emergencyStop() {
  controller1.stopAll();
  controller2.stopAll();
  controller3.stopAll();
  
  for (int i = 0; i < 6; i++) {
    motorPowers[i] = 0;
  }
}

void sendTelemetry() {
  // Format: TELEM,P1:val,E1:val,P2:val,E2:val,...
  PIXHAWK_SERIAL.print(F("TELEM"));
  
  // Controller 1 (Motors 1 & 2)
  PIXHAWK_SERIAL.print(F(",P1:"));
  PIXHAWK_SERIAL.print(controller1.getCurrentPower(MOTOR_1));
  PIXHAWK_SERIAL.print(F(",E1:"));
  PIXHAWK_SERIAL.print(controller1.readEncoder(MOTOR_1));
  
  PIXHAWK_SERIAL.print(F(",P2:"));
  PIXHAWK_SERIAL.print(controller1.getCurrentPower(MOTOR_2));
  PIXHAWK_SERIAL.print(F(",E2:"));
  PIXHAWK_SERIAL.print(controller1.readEncoder(MOTOR_2));
  
  // Controller 2 (Motors 3 & 4)
  PIXHAWK_SERIAL.print(F(",P3:"));
  PIXHAWK_SERIAL.print(controller2.getCurrentPower(MOTOR_1));
  PIXHAWK_SERIAL.print(F(",E3:"));
  PIXHAWK_SERIAL.print(controller2.readEncoder(MOTOR_1));
  
  PIXHAWK_SERIAL.print(F(",P4:"));
  PIXHAWK_SERIAL.print(controller2.getCurrentPower(MOTOR_2));
  PIXHAWK_SERIAL.print(F(",E4:"));
  PIXHAWK_SERIAL.print(controller2.readEncoder(MOTOR_2));
  
  // Controller 3 (Motors 5 & 6)
  PIXHAWK_SERIAL.print(F(",P5:"));
  PIXHAWK_SERIAL.print(controller3.getCurrentPower(MOTOR_1));
  PIXHAWK_SERIAL.print(F(",E5:"));
  PIXHAWK_SERIAL.print(controller3.readEncoder(MOTOR_1));
  
  PIXHAWK_SERIAL.print(F(",P6:"));
  PIXHAWK_SERIAL.print(controller3.getCurrentPower(MOTOR_2));
  PIXHAWK_SERIAL.print(F(",E6:"));
  PIXHAWK_SERIAL.println(controller3.readEncoder(MOTOR_2));
}

// Test function - can be called from DEBUG_SERIAL
void runMotorTest() {
  DEBUG_SERIAL.println(F("\n=== Running Motor Test ==="));
  
  for (int motor = 1; motor <= 6; motor++) {
    DEBUG_SERIAL.print(F("Testing Motor "));
    DEBUG_SERIAL.println(motor);
    
    setMotorPower(motor, 30);
    
    for (int i = 0; i < 20; i++) {
      controller1.update();
      controller2.update();
      controller3.update();
      delay(50);
    }
    
    setMotorPower(motor, 0);
    delay(500);
  }
  
  DEBUG_SERIAL.println(F("=== Test Complete ===\n"));
}
