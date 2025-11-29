/*
  PixhawkMotorServoControl - Control motors AND servos from Pixhawk/QGroundControl
  
  This sketch enables control of 6 motors + 6 servos from a Pixhawk autopilot.
  Perfect for robots with drive motors and manipulator servos.
  
  Hardware Setup:
  - Arduino Mega 2560
  - Pixhawk TELEM2 → Arduino Serial1 (pins 18/19)
  - 3x HiTechnic TETRIX Motor Controllers at 0x01, 0x02, 0x03
  - 1x HiTechnic TETRIX Servo Controller at 0x04
  - 10kΩ resistor from Pin 22 to first controller Pin 5
  - I2C: Pin 20 (SDA), Pin 21 (SCL)
  
  Commands from Pixhawk:
    Motor Commands:
      M1:50       - Motor 1 to 50% power
      M2:-75      - Motor 2 reverse at 75%
      STOP        - Stop all motors
    
    Servo Commands:
      S1:90       - Servo 1 to 90 degrees
      S2:45       - Servo 2 to 45 degrees
      SCENTER     - Center all servos
      
    Combined Commands:
      RESET_ALL   - Reset encoders and center servos
      STATUS      - Get full telemetry (motors + servos)
  
  Telemetry (10Hz):
    TELEM,M1:P:45,E:1234,M2:P:-30,E:5678,...,S1:90,S2:45,...
    
  Use Cases:
  - Mobile robot with manipulator arm
  - Excavator with drive and bucket servos
  - Rover with camera gimbal
  - Robot with gripper and drive
*/

#include <HiTechnicMotor.h>
#include <HiTechnicServo.h>

// Serial configuration
#define PIXHAWK_SERIAL Serial1
#define DEBUG_SERIAL Serial
#define PIXHAWK_BAUD 57600
#define DEBUG_BAUD 9600

// Safety configuration
#define COMMAND_TIMEOUT 1000
#define MAX_MOTOR_POWER 100
#define TELEMETRY_RATE 100
#define ACCEL_RATE 5
#define SERVO_SPEED 5

// Motor controllers at I2C addresses 0x01, 0x02, 0x03
HiTechnicMotor controller1(0x01);  // Motors 1 & 2
HiTechnicMotor controller2(0x02);  // Motors 3 & 4
HiTechnicMotor controller3(0x03);  // Motors 5 & 6

// Servo controller at I2C address 0x04
HiTechnicServo servos(0x04);       // Servos 1-6

// Command buffer
char cmdBuffer[64];
uint8_t cmdIndex = 0;

// Timing
unsigned long lastCommandTime = 0;
unsigned long lastTelemetryTime = 0;

void setup() {
  // Initialize debug serial
  DEBUG_SERIAL.begin(DEBUG_BAUD);
  DEBUG_SERIAL.println(F("HiTechnic TETRIX - Motors + Servos Control"));
  DEBUG_SERIAL.println(F("==========================================="));
  
  // Initialize Pixhawk serial
  PIXHAWK_SERIAL.begin(PIXHAWK_BAUD);
  PIXHAWK_SERIAL.println(F("ARDUINO_READY"));
  
  // Setup Pin 22 for analog detection
  pinMode(22, OUTPUT);
  digitalWrite(22, HIGH);
  DEBUG_SERIAL.println(F("Pin 22 analog detection enabled"));
  
  // Initialize motor controllers
  DEBUG_SERIAL.print(F("Initializing motor controllers..."));
  controller1.begin();
  controller2.begin();
  controller3.begin();
  DEBUG_SERIAL.println(F(" Done"));
  
  // Initialize servo controller
  DEBUG_SERIAL.print(F("Initializing servo controller..."));
  servos.begin();
  servos.setStepTime(SERVO_SPEED);
  servos.centerAll();  // Start with servos centered
  DEBUG_SERIAL.println(F(" Done"));
  
  // Read firmware versions
  DEBUG_SERIAL.print(F("Motor Controller 1 FW: 0x"));
  DEBUG_SERIAL.println(controller1.readVersion(), HEX);
  DEBUG_SERIAL.print(F("Motor Controller 2 FW: 0x"));
  DEBUG_SERIAL.println(controller2.readVersion(), HEX);
  DEBUG_SERIAL.print(F("Motor Controller 3 FW: 0x"));
  DEBUG_SERIAL.println(controller3.readVersion(), HEX);
  DEBUG_SERIAL.print(F("Servo Controller FW: 0x"));
  DEBUG_SERIAL.println(servos.readVersion(), HEX);
  
  DEBUG_SERIAL.println(F("\nWaiting for commands from Pixhawk..."));
  DEBUG_SERIAL.println(F("Motor: M1:50, Servo: S1:90, Stop: STOP"));
  
  lastCommandTime = millis();
}

void loop() {
  // Check for commands from Pixhawk
  while (PIXHAWK_SERIAL.available()) {
    char c = PIXHAWK_SERIAL.read();
    
    if (c == '\n' || c == '\r') {
      if (cmdIndex > 0) {
        cmdBuffer[cmdIndex] = 0;
        processCommand(cmdBuffer);
        cmdIndex = 0;
      }
    } else if (cmdIndex < sizeof(cmdBuffer) - 1) {
      cmdBuffer[cmdIndex++] = c;
    }
  }
  
  // Safety watchdog
  if (millis() - lastCommandTime > COMMAND_TIMEOUT) {
    emergencyStop();
    lastCommandTime = millis();
  }
  
  // Update motor ramping
  controller1.update();
  controller2.update();
  controller3.update();
  
  // Send telemetry
  if (millis() - lastTelemetryTime >= TELEMETRY_RATE) {
    sendTelemetry();
    lastTelemetryTime = millis();
  }
}

void processCommand(const char* cmd) {
  lastCommandTime = millis();
  
  DEBUG_SERIAL.print(F("CMD: "));
  DEBUG_SERIAL.println(cmd);
  
  // Motor commands (M1-M6)
  if (cmd[0] == 'M' && cmd[1] >= '1' && cmd[1] <= '6' && cmd[2] == ':') {
    int motorNum = cmd[1] - '0';
    int power = atoi(cmd + 3);
    power = constrain(power, -MAX_MOTOR_POWER, MAX_MOTOR_POWER);
    
    setMotorPower(motorNum, power);
    
    PIXHAWK_SERIAL.print(F("OK,M"));
    PIXHAWK_SERIAL.print(motorNum);
    PIXHAWK_SERIAL.print(F(":"));
    PIXHAWK_SERIAL.println(power);
    
  // Servo commands (S1-S6)
  } else if (cmd[0] == 'S' && cmd[1] >= '1' && cmd[1] <= '6' && cmd[2] == ':') {
    int servoNum = cmd[1] - '0';
    int angle = atoi(cmd + 3);
    angle = constrain(angle, 0, 180);
    
    servos.setServoAngle(servoNum, angle);
    
    PIXHAWK_SERIAL.print(F("OK,S"));
    PIXHAWK_SERIAL.print(servoNum);
    PIXHAWK_SERIAL.print(F(":"));
    PIXHAWK_SERIAL.println(angle);
    
    DEBUG_SERIAL.print(F("Servo "));
    DEBUG_SERIAL.print(servoNum);
    DEBUG_SERIAL.print(F(" → "));
    DEBUG_SERIAL.print(angle);
    DEBUG_SERIAL.println(F("°"));
    
  // Emergency stop motors
  } else if (strcmp(cmd, "STOP") == 0) {
    emergencyStop();
    PIXHAWK_SERIAL.println(F("STOPPED"));
    DEBUG_SERIAL.println(F("EMERGENCY STOP"));
    
  // Center all servos
  } else if (strcmp(cmd, "SCENTER") == 0) {
    servos.centerAll();
    PIXHAWK_SERIAL.println(F("SERVOS_CENTERED"));
    DEBUG_SERIAL.println(F("All servos centered"));
    
  // Disable all servos
  } else if (strcmp(cmd, "SDISABLE") == 0) {
    for (int i = 1; i <= 6; i++) {
      servos.disableServo(i);
    }
    PIXHAWK_SERIAL.println(F("SERVOS_DISABLED"));
    DEBUG_SERIAL.println(F("All servos disabled"));
    
  // Status request
  } else if (strcmp(cmd, "STATUS") == 0) {
    sendTelemetry();
    
  // Reset encoders
  } else if (strcmp(cmd, "RESET_ENC") == 0) {
    controller1.resetAllEncoders();
    controller2.resetAllEncoders();
    controller3.resetAllEncoders();
    PIXHAWK_SERIAL.println(F("ENCODERS_RESET"));
    
  // Reset everything
  } else if (strcmp(cmd, "RESET_ALL") == 0) {
    controller1.resetAllEncoders();
    controller2.resetAllEncoders();
    controller3.resetAllEncoders();
    servos.centerAll();
    PIXHAWK_SERIAL.println(F("ALL_RESET"));
    DEBUG_SERIAL.println(F("Encoders reset, servos centered"));
    
  // All motors same power
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
    DEBUG_SERIAL.print(F("Unknown: "));
    DEBUG_SERIAL.println(cmd);
  }
}

void setMotorPower(uint8_t motorNum, int8_t power) {
  switch (motorNum) {
    case 1:
      controller1.setMotorPowerSmooth(MOTOR_1, power, ACCEL_RATE);
      break;
    case 2:
      controller1.setMotorPowerSmooth(MOTOR_2, power, ACCEL_RATE);
      break;
    case 3:
      controller2.setMotorPowerSmooth(MOTOR_1, power, ACCEL_RATE);
      break;
    case 4:
      controller2.setMotorPowerSmooth(MOTOR_2, power, ACCEL_RATE);
      break;
    case 5:
      controller3.setMotorPowerSmooth(MOTOR_1, power, ACCEL_RATE);
      break;
    case 6:
      controller3.setMotorPowerSmooth(MOTOR_2, power, ACCEL_RATE);
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
}

void sendTelemetry() {
  PIXHAWK_SERIAL.print(F("TELEM"));
  
  // Motor telemetry
  PIXHAWK_SERIAL.print(F(",M1:P:"));
  PIXHAWK_SERIAL.print(controller1.getCurrentPower(MOTOR_1));
  PIXHAWK_SERIAL.print(F(",E:"));
  PIXHAWK_SERIAL.print(controller1.readEncoder(MOTOR_1));
  
  PIXHAWK_SERIAL.print(F(",M2:P:"));
  PIXHAWK_SERIAL.print(controller1.getCurrentPower(MOTOR_2));
  PIXHAWK_SERIAL.print(F(",E:"));
  PIXHAWK_SERIAL.print(controller1.readEncoder(MOTOR_2));
  
  PIXHAWK_SERIAL.print(F(",M3:P:"));
  PIXHAWK_SERIAL.print(controller2.getCurrentPower(MOTOR_1));
  PIXHAWK_SERIAL.print(F(",E:"));
  PIXHAWK_SERIAL.print(controller2.readEncoder(MOTOR_1));
  
  PIXHAWK_SERIAL.print(F(",M4:P:"));
  PIXHAWK_SERIAL.print(controller2.getCurrentPower(MOTOR_2));
  PIXHAWK_SERIAL.print(F(",E:"));
  PIXHAWK_SERIAL.print(controller2.readEncoder(MOTOR_2));
  
  PIXHAWK_SERIAL.print(F(",M5:P:"));
  PIXHAWK_SERIAL.print(controller3.getCurrentPower(MOTOR_1));
  PIXHAWK_SERIAL.print(F(",E:"));
  PIXHAWK_SERIAL.print(controller3.readEncoder(MOTOR_1));
  
  PIXHAWK_SERIAL.print(F(",M6:P:"));
  PIXHAWK_SERIAL.print(controller3.getCurrentPower(MOTOR_2));
  PIXHAWK_SERIAL.print(F(",E:"));
  PIXHAWK_SERIAL.print(controller3.readEncoder(MOTOR_2));
  
  // Servo telemetry (positions in degrees)
  for (int i = 1; i <= 6; i++) {
    uint8_t pos = servos.getServoPosition(i);
    uint8_t angle = map(pos, 0, 255, 0, 180);
    
    PIXHAWK_SERIAL.print(F(",S"));
    PIXHAWK_SERIAL.print(i);
    PIXHAWK_SERIAL.print(F(":"));
    PIXHAWK_SERIAL.print(angle);
  }
  
  PIXHAWK_SERIAL.println();
}
