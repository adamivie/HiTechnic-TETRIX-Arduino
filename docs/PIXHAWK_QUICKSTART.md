# Quick Start: Testing Pixhawk Integration

## Step 1: Test Arduino Alone (USB)

Before connecting to Pixhawk, test the communication:

1. **Upload the sketch**:
   ```powershell
   cd "c:\tetrix motor controllers"
   & "$env:LOCALAPPDATA\Programs\Arduino CLI\arduino-cli.exe" compile --fqbn arduino:avr:mega ".\examples\Motor\PixhawkMotorControl"
   & "$env:LOCALAPPDATA\Programs\Arduino CLI\arduino-cli.exe" upload -p COM12 --fqbn arduino:avr:mega ".\examples\Motor\PixhawkMotorControl"
   ```

2. **Open Serial Monitor** (9600 baud)

3. **Send test commands** via Serial Monitor:
   ```
   M1:50      ← Motor 1 to 50%
   M2:-30     ← Motor 2 to -30% (reverse)
   STOP       ← Emergency stop
   STATUS     ← Get telemetry
   RESET_ENC  ← Reset encoders
   MALL:25    ← All motors to 25%
   ```

4. **Verify responses**:
   - You should see `OK,M1:50` after commands
   - Telemetry should appear every 100ms
   - Motors should respond smoothly

## Step 2: Connect to Pixhawk

### Hardware Connection

**Pixhawk TELEM2 Port (6-pin JST-GH):**
```
Pin 1: +5V     → Arduino 5V (optional - can power from USB)
Pin 2: TX      → Arduino Pin 19 (RX1)
Pin 3: RX      → Arduino Pin 18 (TX1)
Pin 6: GND     → Arduino GND
```

**Important**: 
- Cross connections: Pixhawk TX → Arduino RX, Pixhawk RX → Arduino TX
- Common ground is essential
- Keep Arduino USB connected for debugging initially

### Pixhawk Configuration (PX4)

1. **Connect QGC to Pixhawk**
2. **Go to Parameters**:
   ```
   SER_TEL2_BAUD = 57600
   MAV_1_CONFIG = TELEM2
   MAV_1_MODE = Normal
   ```
3. **Reboot Pixhawk**

### Pixhawk Configuration (ArduPilot)

1. **Connect QGC to Pixhawk**
2. **Go to Parameters**:
   ```
   SERIAL2_PROTOCOL = 2 (MAVLink2) or 0 (None) for direct pass-through
   SERIAL2_BAUD = 57
   ```
3. **Reboot Pixhawk**

## Step 3: Test Through Pixhawk

### Using MAVLink Console in QGC

1. **Open QGC**
2. **Go to Analyze Tools → MAVLink Console**
3. **Send commands**:
   ```
   nsh> echo "M1:50" > /dev/ttyS1
   nsh> echo "STATUS" > /dev/ttyS1
   ```

4. **Read responses**:
   ```
   nsh> cat /dev/ttyS1
   ```

### Using Custom PX4 Module (Advanced)

Create a simple module that forwards RC input:

```cpp
// In PX4-Autopilot/src/modules/motor_control/
#include <px4_platform_common/module.h>
#include <drivers/drv_hrt.h>
#include <uORB/topics/manual_control_setpoint.h>

int manual_control_fd = orb_subscribe(ORB_ID(manual_control_setpoint));

while (true) {
    struct manual_control_setpoint_s manual;
    orb_copy(ORB_ID(manual_control_setpoint), manual_control_fd, &manual);
    
    // Map stick to motor command
    int power = (int)(manual.y * 100.0f);
    
    // Send to Arduino via UART
    dprintf(uart_fd, "M1:%d\n", power);
    
    usleep(100000);  // 10Hz
}
```

## Step 4: QGroundControl Custom Actions

### Create Custom Buttons

Add to `CustomCommandWidget.qml`:

```qml
CustomButton {
    text: "Stop All Motors"
    onClicked: {
        activeVehicle.sendMavCommandInt(
            MAV_CMD_USER_1,  // Custom command
            true,            // Show error
            0, 0, 0, 0, 0, 0 // Parameters
        )
    }
}

CustomButton {
    text: "Motor Test"
    onClicked: {
        activeVehicle.sendMavCommandInt(MAV_CMD_USER_2, true, 0, 0, 0, 0, 0, 0)
    }
}
```

### Handle Custom Commands in PX4

```cpp
// Intercept custom MAV_CMD in commander module
if (cmd.command == MAV_CMD_USER_1) {
    // Send STOP to Arduino
    dprintf(uart_fd, "STOP\n");
    return vehicle_command_ack_s::VEHICLE_CMD_RESULT_ACCEPTED;
}
```

## Step 5: Joystick Control

### Configure Joystick in QGC

1. **Vehicle Setup → Joystick**
2. **Calibrate your joystick/gamepad**
3. **Map channels**:
   - Channel 1 (Left Y) → Roll (will map to motors later)
   - Channel 2 (Right Y) → Pitch
   - Button 1 → Emergency Stop

### Forward Joystick to Arduino

**Option A: Via PX4 Lua/Custom Module**
Read joystick → Send motor commands via serial

**Option B: Via RC Override**
QGC sends RC_OVERRIDE → PX4 reads → Forwards to Arduino

**Option C: Direct from QGC**
Write QGC plugin to send commands directly via MAVLink

## Common Commands

### Testing Commands
```bash
# Single motor test
M1:50       # Motor 1 forward at 50%
M1:-50      # Motor 1 reverse at 50%
M1:0        # Motor 1 stop

# Multiple motors
M1:50
M2:50
M3:50       # Run first 3 motors

# All motors
MALL:30     # All motors to 30%

# Safety
STOP        # Emergency stop all

# Status
STATUS      # Get full telemetry
RESET_ENC   # Reset encoder positions
```

### Expected Telemetry Response
```
TELEM,P1:48,E1:1234,P2:0,E2:0,P3:0,E3:0,P4:0,E4:0,P5:0,E5:0,P6:0,E6:0
```

Fields:
- `P1-P6`: Current power levels (-100 to 100)
- `E1-E6`: Encoder positions (counts)

## Troubleshooting

### No Response from Arduino
1. Check baud rates (57600 on both)
2. Verify TX/RX cross-connection
3. Check common ground
4. Test with USB serial adapter first

### Motors Don't Move
1. Check motor power supply
2. Verify I2C connections (Pin 22 setup)
3. Check motor controller addresses (0x01, 0x02, 0x03)
4. Watch debug serial for error messages

### Watchdog Stopping Motors
- Send commands faster than 1 second interval
- Or increase `COMMAND_TIMEOUT` in sketch

### Telemetry Not Updating
- Check `TELEMETRY_RATE` setting
- Verify serial receive on Pixhawk side
- Use USB debug serial to see actual output

## Next Steps

Once basic control works:

1. **Implement joystick mapping** for real-time control
2. **Add custom QGC widgets** for motor visualization
3. **Create mission support** for automated sequences
4. **Add sensor integration** (current, temperature, etc.)
5. **Implement ROS2 bridge** for advanced autonomy

## Safety Reminders

- ⚠️ **Always test with motors unloaded first**
- ⚠️ **Watchdog timer will stop motors after 1 second of no commands**
- ⚠️ **Emergency stop command should be easily accessible**
- ⚠️ **Set `MAX_MOTOR_POWER` lower for initial testing**
- ⚠️ **Keep USB connected for debugging until system proven stable**

---

**Questions?** Start with Step 1 (USB testing) before connecting Pixhawk. This isolates Arduino/motor issues from communication issues.
