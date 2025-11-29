# HiTechnic Daisy Chain Addressing Explained

## How Automatic Addressing Works

According to the HiTechnic specification, the controllers use **"the legacy analog signal of 5v (not 9v) supplied through a 10.0k resistor to automatically generate daisy chain I2C addresses."**

### The Magic: Pin 5 (Analog Detection)

```
6P6C Cable Pinout:
Pin 1: GND
Pin 2: GND  
Pin 3: Not used
Pin 4: Not used
Pin 5: SCL (I2C Clock) + 5V ANALOG DETECTION
Pin 6: SDA (I2C Data)
```

## Address Assignment Logic

### NXT Brick Connection (Reference)
When connected to a LEGO NXT brick:
- NXT supplies **5V through 10kΩ resistor** on Pin 5
- First controller detects this 5V → assigns itself address **0x02/0x03** (7-bit: 0x01)
- First controller then supplies 5V to next controller in chain
- Second controller sees 5V from previous controller → assigns itself **0x04/0x05** (7-bit: 0x02)
- And so on...

### Arduino Connection (Your Setup)
When connected to Arduino Mega:
- Arduino does NOT supply 5V through 10kΩ resistor on SCL line
- Controllers cannot detect their position in chain
- **ALL controllers default to the SAME address**

## Why You Only See Address 0x05

Your Arduino is likely providing:
- Pin 20 (SDA) → Pin 6 on controllers
- Pin 21 (SCL) → Pin 5 on controllers
- GND → Pin 1 or 2

**But Pin 21 (SCL) only has I2C clock signal, NOT the 5V analog voltage!**

## The Solution

### Option 1: Add 5V Detection Circuit (Hardware Modification)

Connect a **5V signal through 10kΩ resistor** to Pin 5 of FIRST controller only:

```
Arduino Mega → First Controller:
- Pin 20 (SDA) → Pin 6 
- Pin 21 (SCL) → Pin 5 (I2C clock)
- Pin 22 (Digital Out) → 10kΩ resistor → Pin 5 (5V detection)
- GND → Pin 1

First Controller → Second Controller:
- Pin 5 has 5V from first controller → second gets address 0x04/0x05
- Continue chain...
```

**Wiring Diagram:**
```
Arduino Mega
├─ Pin 20 (SDA) ──┬─────────> [Controller 1 Pin 6] ──> [Controller 2 Pin 6] ──> [Controller 3 Pin 6]
├─ Pin 21 (SCL) ──┼─────────> [Controller 1 Pin 5] ──> [Controller 2 Pin 5] ──> [Controller 3 Pin 5]
│                  │                    │
└─ Pin 22 ───[10kΩ]┘                   │
                                        └── (Controller 1 supplies 5V to Controller 2's Pin 5)
```

### Option 2: Use Separate I2C Buses (Software Solution)

Since you can't change addresses, use **SoftwareI2C library** with separate pins for each controller:

```
Controller 1: Pins 22 (SDA), 23 (SCL) → Address 0x02
Controller 2: Pins 24 (SDA), 25 (SCL) → Address 0x02  
Controller 3: Pins 26 (SDA), 27 (SCL) → Address 0x02
```

Each controller thinks it's the first in the chain!

## Expected Addresses in Proper Daisy Chain

| Position | 8-bit Addresses | 7-bit Address (Arduino) |
|----------|----------------|-------------------------|
| First    | 0x02 / 0x03    | 0x01                   |
| Second   | 0x04 / 0x05    | 0x02                   |
| Third    | 0x06 / 0x07    | 0x03                   |
| Fourth   | 0x08 / 0x09    | 0x04                   |

## Why Address 0x05 Appears

Address 0x05 in 7-bit = 0x0A/0x0B in 8-bit

This suggests:
- Controllers may be in a **default/unconfigured state**
- Or there's a **manufacturing default address** when no analog detection occurs
- The 5V detection circuit is not active without proper NXT brick connection

## Next Steps

1. **Test the 5V detection theory**: Add 5V through 10kΩ resistor to Pin 5 of first controller
2. **Verify daisy chain propagation**: Check if subsequent controllers get different addresses
3. **Or implement Software I2C**: Use separate GPIO pins for each controller

## Key Quote from Spec

> "The servo controller will use the legacy analog signal of 5v (not 9v) supplied 
> through a 10.0k resistor to automatically generate daisy chain I2C addresses."

This is the missing piece! The Arduino doesn't provide this 5V analog signal.
