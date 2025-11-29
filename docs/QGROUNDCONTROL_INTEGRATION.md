# QGroundControl Integration Guide

## Overview
This guide explains how to control your HiTechnic TETRIX motors from QGroundControl through a Pixhawk autopilot.

## System Architecture

```
┌─────────────────────┐
│  QGroundControl     │  - User interface
│  (PC/Tablet)        │  - Mission planning
└──────────┬──────────┘  - Telemetry display
           │ MAVLink (USB/Radio)
           ↓
┌─────────────────────┐
│  Pixhawk Autopilot  │  - Flight controller
│  (PX4 or ArduPilot) │  - MAVLink hub
└──────────┬──────────┘  - Handles QGC commands
           │ Serial/UART (Custom Protocol)
           ↓
┌─────────────────────┐
│  Arduino Mega 2560  │  - Motor controller interface
│  (Companion Comp)   │  - HiTechnic I2C driver
└──────────┬──────────┘  - Telemetry gathering
           │ I2C
           ↓
┌─────────────────────┐
│  HiTechnic TETRIX   │  - DC motor drivers
│  Motor Controllers  │  - 3 controllers (6 motors)
└─────────────────────┘
```

## Hardware Connections

### Pixhawk to Arduino
Connect via UART (TELEM2 recommended):

| Pixhawk TELEM2 | Arduino Mega |
|----------------|--------------|
| TX (Pin 2)     | RX1 (Pin 19) |
| RX (Pin 3)     | TX1 (Pin 18) |
| GND (Pin 6)    | GND          |
| +5V (Pin 1)    | 5V (optional)|

**Note**: Use Serial1 on Arduino (pins 18/19) to keep Serial (USB) free for debugging.

### Arduino to Motor Controllers
Your existing I2C setup:
- Pin 20 (SDA) → All controllers SDA
- Pin 21 (SCL) → All controllers SCL
- Pin 22 → 10kΩ resistor → First controller Pin 5
- Common ground

## Communication Protocol

### Option A: Simple Text Protocol (Easy to Start)

**Arduino receives from Pixhawk:**
```
M1:50\n     - Motor 1 to 50% power
M2:-30\n    - Motor 2 to -30% power
M3:0\n      - Motor 3 to 0% (stop)
MALL:25\n   - All motors to 25%
STOP\n      - Emergency stop all
STATUS\n    - Request telemetry
```

**Arduino sends to Pixhawk:**
```
TELEM,M1:45,E1:1234,M2:-28,E2:5678,...\n
```

### Option B: Binary Protocol (Efficient)

**Command Packet (Pixhawk → Arduino):**
```cpp
struct MotorCommand {
  uint8_t startByte;      // 0xAA (packet sync)
  uint8_t motorMask;      // Bits 0-5 = motors 1-6
  int8_t motorPowers[6];  // -100 to 100 for each motor
  uint8_t checksum;       // XOR of all bytes
};
```

**Telemetry Packet (Arduino → Pixhawk):**
```cpp
struct MotorTelemetry {
  uint8_t startByte;      // 0xBB (packet sync)
  int8_t powers[6];       // Current power levels
  int32_t encoders[6];    // Encoder positions
  uint8_t status;         // Error flags, busy bits
  uint16_t batteryVoltage;// Battery voltage * 100
  uint8_t checksum;       // XOR of all bytes
};
```

### Option C: MAVLink Protocol (Professional)

Use MAVLink messages for native QGC integration:
- `COMMAND_LONG` for motor commands
- `NAMED_VALUE_FLOAT` for telemetry
- `STATUSTEXT` for diagnostic messages

## Arduino Firmware Implementation

### Serial Communication Handler

```cpp
// Serial1 for Pixhawk communication
#define PIXHAWK_SERIAL Serial1
#define PIXHAWK_BAUD 57600

// Command buffer
char cmdBuffer[64];
uint8_t cmdIndex = 0;

void setup() {
  Serial.begin(9600);        // Debug
  PIXHAWK_SERIAL.begin(PIXHAWK_BAUD);  // Pixhawk
  
  // Initialize motor controllers
  motor1.begin();
  motor2.begin();
  motor3.begin();
}

void loop() {
  // Check for commands from Pixhawk
  if (PIXHAWK_SERIAL.available()) {
    char c = PIXHAWK_SERIAL.read();
    
    if (c == '\n') {
      cmdBuffer[cmdIndex] = 0;  // Null terminate
      processCommand(cmdBuffer);
      cmdIndex = 0;
    } else if (cmdIndex < sizeof(cmdBuffer) - 1) {
      cmdBuffer[cmdIndex++] = c;
    }
  }
  
  // Update motor ramping
  motor1.update();
  motor2.update();
  motor3.update();
  
  // Send telemetry periodically
  static unsigned long lastTelem = 0;
  if (millis() - lastTelem > 100) {  // 10Hz telemetry
    sendTelemetry();
    lastTelem = millis();
  }
}

void processCommand(const char* cmd) {
  // Parse motor commands
  if (cmd[0] == 'M' && cmd[1] >= '1' && cmd[1] <= '6') {
    int motorNum = cmd[1] - '0';
    int power = atoi(cmd + 3);  // Skip "M1:"
    
    // Map motor number to controller and motor
    if (motorNum == 1) motor1.setMotorPowerSmooth(MOTOR_1, power, 5);
    else if (motorNum == 2) motor1.setMotorPowerSmooth(MOTOR_2, power, 5);
    else if (motorNum == 3) motor2.setMotorPowerSmooth(MOTOR_1, power, 5);
    else if (motorNum == 4) motor2.setMotorPowerSmooth(MOTOR_2, power, 5);
    else if (motorNum == 5) motor3.setMotorPowerSmooth(MOTOR_1, power, 5);
    else if (motorNum == 6) motor3.setMotorPowerSmooth(MOTOR_2, power, 5);
    
    PIXHAWK_SERIAL.println("OK");
    
  } else if (strcmp(cmd, "STOP") == 0) {
    motor1.stopAll();
    motor2.stopAll();
    motor3.stopAll();
    PIXHAWK_SERIAL.println("STOPPED");
    
  } else if (strcmp(cmd, "STATUS") == 0) {
    sendTelemetry();
  }
}

void sendTelemetry() {
  // Send compact telemetry string
  PIXHAWK_SERIAL.print("TELEM");
  
  // Motor 1 powers and encoders
  PIXHAWK_SERIAL.print(",P1:");
  PIXHAWK_SERIAL.print(motor1.getCurrentPower(MOTOR_1));
  PIXHAWK_SERIAL.print(",E1:");
  PIXHAWK_SERIAL.print(motor1.readEncoder(MOTOR_1));
  
  PIXHAWK_SERIAL.print(",P2:");
  PIXHAWK_SERIAL.print(motor1.getCurrentPower(MOTOR_2));
  PIXHAWK_SERIAL.print(",E2:");
  PIXHAWK_SERIAL.print(motor1.readEncoder(MOTOR_2));
  
  // Motor 2 controller
  PIXHAWK_SERIAL.print(",P3:");
  PIXHAWK_SERIAL.print(motor2.getCurrentPower(MOTOR_1));
  PIXHAWK_SERIAL.print(",E3:");
  PIXHAWK_SERIAL.print(motor2.readEncoder(MOTOR_1));
  
  PIXHAWK_SERIAL.print(",P4:");
  PIXHAWK_SERIAL.print(motor2.getCurrentPower(MOTOR_2));
  PIXHAWK_SERIAL.print(",E4:");
  PIXHAWK_SERIAL.print(motor2.readEncoder(MOTOR_2));
  
  // Motor 3 controller
  PIXHAWK_SERIAL.print(",P5:");
  PIXHAWK_SERIAL.print(motor3.getCurrentPower(MOTOR_1));
  PIXHAWK_SERIAL.print(",E5:");
  PIXHAWK_SERIAL.print(motor3.readEncoder(MOTOR_1));
  
  PIXHAWK_SERIAL.print(",P6:");
  PIXHAWK_SERIAL.print(motor3.getCurrentPower(MOTOR_2));
  PIXHAWK_SERIAL.print(",E6:");
  PIXHAWK_SERIAL.println(motor3.readEncoder(MOTOR_2));
}
```

## Pixhawk Configuration

### PX4 Firmware

1. **Enable Companion Computer**:
   ```bash
   # In QGC: Parameters
   SER_TEL2_BAUD = 57600
   MAV_1_MODE = Normal (or Onboard)
   ```

2. **Custom Mixer for Pass-through**:
   Create custom mixer file to route RC channels to serial commands

3. **Custom Module**:
   Write PX4 module that:
   - Reads RC input or mission commands
   - Sends motor commands via UART to Arduino
   - Receives telemetry from Arduino
   - Publishes to uORB topics for QGC display

### ArduPilot Firmware

1. **Serial Port Configuration**:
   ```
   SERIAL2_PROTOCOL = 2 (MAVLink2)
   SERIAL2_BAUD = 57
   ```

2. **Lua Script** (ArduPilot 4.0+):
   ```lua
   -- Script to control Arduino motors from RC
   function update()
     local ch1 = rc:get_pwm(1)  -- Read RC channel 1
     local power = (ch1 - 1500) / 5  -- Convert 1000-2000 to -100 to 100
     
     uart:write("M1:" .. power .. "\n")
     
     return update, 100  -- Run at 10Hz
   end
   
   return update()
   ```

## QGroundControl Setup

### 1. Custom Widgets
Create custom QGC widgets to display motor telemetry:
- Motor power levels (gauges)
- Encoder positions (numbers)
- Motor status indicators

### 2. Joystick/Gamepad Mapping
Map joystick axes to motor commands:
- Left stick Y-axis → Motors 1-3
- Right stick Y-axis → Motors 4-6
- Buttons for modes, emergency stop

### 3. Custom Actions
Add custom toolbar buttons:
- "Stop All Motors"
- "Reset Encoders"
- "Run Test Pattern"

## Testing Procedure

### Phase 1: Direct Arduino Testing
1. Connect Arduino USB
2. Open Serial Monitor
3. Send commands: `M1:50`, `STOP`, `STATUS`
4. Verify motor response and telemetry

### Phase 2: Pixhawk-Arduino Communication
1. Connect Pixhawk to Arduino via UART
2. Use MAVLink console to send commands
3. Verify Arduino receives and responds
4. Check telemetry flow

### Phase 3: QGC Integration
1. Connect QGC to Pixhawk
2. Use MAVLink console in QGC
3. Send test commands through Pixhawk
4. Verify full chain works

### Phase 4: Joystick Control
1. Configure joystick in QGC
2. Map axes to motor control functions
3. Test smooth control
4. Verify emergency stop

## Example QGC Configuration

### Custom Button in QGC
```json
{
  "QGCRoot": {
    "customButtons": [
      {
        "label": "Stop All Motors",
        "command": "vehicle.sendMavCommand(MAV_CMD_USER_1, 0, 0, 0, 0, 0, 0, 0)"
      },
      {
        "label": "Motor Test Pattern",
        "command": "vehicle.sendMavCommand(MAV_CMD_USER_2, 0, 0, 0, 0, 0, 0, 0)"
      }
    ]
  }
}
```

## Advanced Features

### 1. Position Control from Missions
Use waypoints to control motors:
- Waypoint actions trigger motor sequences
- Distance-based motor control
- Timed operations

### 2. Sensor Feedback
Integrate additional sensors:
- Current sensors for motor load monitoring
- Temperature sensors for thermal management
- Limit switches for safety

### 3. Autonomous Modes
Create custom flight modes:
- "Excavate" mode - coordinated digging pattern
- "Drive" mode - differential drive control
- "Manipulator" mode - arm control

### 4. Telemetry Logging
Log all motor data:
- QGC automatically logs MAVLink telemetry
- Arduino can log to SD card
- Analyze performance offline

## Troubleshooting

### No Communication
- Check baud rates match (57600)
- Verify TX → RX, RX → TX connections
- Check ground connection
- Test with USB-serial adapter first

### Delayed Response
- Reduce telemetry rate (slower than 10Hz)
- Use binary protocol instead of text
- Check for I2C bus delays

### Erratic Motor Behavior
- Add command validation/limits in Arduino
- Implement watchdog timer (stop if no commands for 1 second)
- Check for electrical noise on I2C bus

## Safety Features (CRITICAL)

Always implement these safety features:

```cpp
// Watchdog timer
unsigned long lastCommandTime = 0;
#define COMMAND_TIMEOUT 1000  // 1 second

void loop() {
  // Check for command timeout
  if (millis() - lastCommandTime > COMMAND_TIMEOUT) {
    // Stop all motors if no recent commands
    motor1.stopAll();
    motor2.stopAll();
    motor3.stopAll();
  }
  
  // ... rest of loop
}

void processCommand(const char* cmd) {
  lastCommandTime = millis();  // Update on any command
  // ... process command
}

// Emergency stop on serial error
void serialEvent1() {
  if (PIXHAWK_SERIAL.available() == 0) {
    // Connection lost?
    motor1.stopAll();
    motor2.stopAll();
    motor3.stopAll();
  }
}
```

## Next Steps

1. **Create Arduino sketch** with serial communication
2. **Test with PC** before connecting Pixhawk
3. **Configure Pixhawk** serial port
4. **Write Pixhawk module/script** for motor control
5. **Set up QGC** custom interface
6. **Test full system** thoroughly before live operation

## Resources

- [PX4 Serial Configuration](https://docs.px4.io/main/en/peripherals/serial_configuration.html)
- [ArduPilot Lua Scripting](https://ardupilot.org/plane/docs/common-lua-scripts.html)
- [QGC Custom Build](https://docs.qgroundcontrol.com/master/en/getting_started/custom_build.html)
- [MAVLink Protocol](https://mavlink.io/en/)

---

**Need Help?** Start with the simple text protocol in Phase 1, then gradually add complexity once basic communication works.
