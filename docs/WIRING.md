# Wiring Diagram - Arduino Mega to HiTechnic TETRIX Motor Controller

## I2C Connection

The HiTechnic motor controller uses I2C communication protocol. Connect as follows:

### Arduino Mega Pins:
- **SDA (Pin 20)** - I2C Data Line
- **SCL (Pin 21)** - I2C Clock Line
- **GND** - Ground

### HiTechnic Motor Controller:
- **SDA** - Connect to Arduino Pin 20
- **SCL** - Connect to Arduino Pin 21
- **GND** - Connect to Arduino GND

```
Arduino Mega                HiTechnic Motor Controller
┌─────────────┐            ┌─────────────────────────┐
│             │            │                         │
│   Pin 20────┼────────────┼─── SDA                 │
│   (SDA)     │            │                         │
│             │            │                         │
│   Pin 21────┼────────────┼─── SCL                 │
│   (SCL)     │            │                         │
│             │            │                         │
│   GND   ────┼────────────┼─── GND                 │
│             │            │                         │
└─────────────┘            └─────────────────────────┘
      │                              │
      │                              │
   USB Power                    12V TETRIX Battery
   (5V Logic)                   (Motor Power)
```

## Important Notes:

### Power Supply:
1. **Arduino Mega** - Powered via USB or external 7-12V supply to VIN pin
2. **HiTechnic Controller** - Must be powered by 12V TETRIX battery (separate power)
3. **Common Ground** - CRITICAL: Connect Arduino GND to HiTechnic GND for I2C communication

### I2C Address:
- Default I2C address: **0x02**
- Can be changed using jumpers on the HiTechnic controller
- If using multiple controllers, each must have a unique address

### Pull-up Resistors:
- Arduino Mega has internal pull-up resistors on SDA/SCL
- HiTechnic controller also has pull-ups
- External pull-ups (4.7kΩ) may help with longer wires (>30cm)

### Wire Length:
- Keep I2C wires as short as possible (< 1 meter recommended)
- Use twisted pair or shielded cable for longer runs
- Avoid running I2C wires parallel to motor power wires (EMI noise)

## Motor Connections:

```
HiTechnic Motor Controller
┌─────────────────────────┐
│                         │
│  M1+  M1-  M2+  M2-    │ ← Motor Terminal Blocks
│   │    │    │    │     │
└───┼────┼────┼────┼─────┘
    │    │    │    │
    └────┴────┘    └────┴─── Connect TETRIX DC Motors
    Motor 1         Motor 2
```

### Motor Polarity:
- **M1+/M1-** - Motor 1 terminals (positive/negative)
- **M2+/M2-** - Motor 2 terminals (positive/negative)
- If motor runs backward, swap the + and - connections

### Encoder Connections:
- If using TETRIX motors with encoders, connect encoder cables to the HiTechnic controller
- Encoder ports are typically 6-pin connectors on the controller
- Encoders provide feedback for precise position and speed control

## Complete System Diagram:

```
┌──────────────┐
│   Computer   │
└──────┬───────┘
       │ USB
       │
┌──────▼──────────────────────┐
│   Arduino Mega              │
│                             │
│  Pin 20 (SDA)──────┐       │
│  Pin 21 (SCL)──────┤       │
│  GND      ──────────┤       │
└─────────────────────┼───────┘
                      │
                      │ I2C Bus
                      │
       ┌──────────────▼──────────────────┐
       │  HiTechnic Motor Controller     │
       │  (I2C Address: 0x02)            │
       │                                 │
       │  M1+  M1-      M2+  M2-        │
       └───┬────┬────────┬────┬─────────┘
           │    │        │    │
       ┌───▼────▼──┐ ┌───▼────▼──┐
       │  TETRIX   │ │  TETRIX   │
       │  Motor 1  │ │  Motor 2  │
       │ (w/encoder)│ │ (w/encoder)│
       └───────────┘ └───────────┘
           │
       ┌───▼────────────────────┐
       │  12V TETRIX Battery    │
       │  (Powers motors only)  │
       └────────────────────────┘
```

## Troubleshooting:

### No Communication:
- Check SDA/SCL connections
- Verify common ground connection
- Confirm I2C address (use I2C scanner sketch)
- Check if motor controller is powered

### Motors Don't Move:
- Verify 12V battery is connected and charged
- Check motor connections to M1/M2 terminals
- Test with different power levels (try 100)
- Swap motor + and - if direction is wrong

### Erratic Behavior:
- Add 4.7kΩ pull-up resistors on SDA/SCL
- Shorten I2C wire length
- Separate I2C wires from motor power wires
- Check for loose connections
