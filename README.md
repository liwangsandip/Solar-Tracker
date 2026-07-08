# Autonomous Solar Tracking System

**SolarTrack** — An autonomous light-tracking system that uses two light sensors to detect the brightest direction and automatically rotates a servo motor to align with it, without any manual input — conceptually mirroring how solar tracking systems orient panels toward the sun.

---

## 1. Objective

Companies working in solar energy equipment such as precision laser structuring systems for solar cell manufacturing rely on automated, sensor-driven positioning rather than manual adjustment. This project implements a small-scale demonstration of that same underlying principle: **continuous sensing, comparison, and automatic correction**, applied here to light tracking.

Two photoresistors (LDRs) continuously report ambient light levels from two directions. The system compares these readings and rotates a servo motor toward whichever side is currently brighter, holding position when light levels are roughly balanced.

---

## 2. System Architecture

```
   [LDR - Left]              [LDR - Right]
         |                         |
   Voltage Divider           Voltage Divider
   (with 10k resistor)       (with 10k resistor)
         |                         |
    Analog Input (A0)        Analog Input (A1)
         |                         |
         +-----------+-------------+
                     v
              [Arduino Uno]
        (reads both sensors, compares
         values, computes servo angle)
                     |
                     v
             [Servo Motor (Pin 9)]
            (rotates toward brighter side)
```

**Control flow:**
1. Read both LDR voltage divider outputs (A0, A1).
2. Compare the two values against a defined threshold.
3. If one side is significantly brighter, incrementally step the servo angle toward that side.
4. If both readings are roughly equal, hold the current position.
5. Clamp the output angle within the servo's physical 0°–180° range.
6. Repeat continuously.

---

## 3. Components Used

| Component | Quantity | Purpose |
|---|---|---|
| Arduino Uno R3 | 1 | Microcontroller — reads inputs, runs control logic |
| Servo Motor (SG90) | 1 | Actuator — physical positioning output (0°–180°) |
| Photoresistor (LDR) | 2 | Analog sensors — detect ambient light level from two directions |
| Resistor (10kΩ) | 2 | Forms voltage divider with each LDR |
| Breadboard + Jumper Wires | — | Circuit assembly |

---

## 4. Working Principle

### Voltage Divider Sensing
Each LDR's resistance changes with light (lower resistance in bright light, higher in darkness). Since a microcontroller can only measure voltage — not resistance directly — each LDR is paired with a fixed 10kΩ resistor to form a voltage divider:

```
5V → LDR → [node, tapped by analog pin] → Resistor → GND
```

The analog pin (A0 or A1) taps the junction between the LDR and resistor, reading a voltage that varies proportionally with the LDR's changing resistance — and therefore with ambient light level.

### Comparison and Threshold Logic
Rather than reacting to any tiny difference between the two sensors (which would cause constant, jittery micro-movements from sensor noise), the system only adjusts the servo when the difference between readings exceeds a defined **threshold**. This creates stable, deliberate tracking behavior rather than erratic correction.

### Incremental Movement
Instead of jumping directly to a calculated angle, the servo angle is adjusted in small fixed steps each loop cycle, producing smooth, gradual tracking motion rather than abrupt jumps.

### Clamping
Since the SG90 servo is mechanically limited to a 0°–180° range, the computed angle is clamped within these bounds every cycle to prevent invalid commands from being sent to the servo.

---

## 5. Development Approach: Python Prototyping Before Hardware

Before writing embedded C++, the control logic was first prototyped and validated in Python, using simulated sensor values with gradual random drift (rather than fully random jumps) to approximate realistic, continuously changing light conditions. This allowed the comparison, threshold, and clamping logic to be tested and debugged independently of the hardware, before translating it into Arduino C++.

This same logic was later reworked into an object-oriented Python version using dedicated `LightSensor` and `ServoController` classes, to practice cleaner state management and separation of concerns — demonstrating the same control logic in a more structured, reusable form.

---

## 6. Challenges & Solutions

| Challenge | Diagnosis | Solution |
|---|---|---|
| One LDR circuit gave a constant, unresponsive reading regardless of light level | The analog pin for that sensor was wired to the same node as GND (the far end of the resistor), rather than the junction between the LDR and resistor | Rewired the analog input to tap the correct node — directly between the LDR and its resistor — matching the structure of the correctly-wired second sensor |
| Simulated sensor readings jumped erratically in the Python prototype, causing unrealistic servo jitter during logic testing | Initial simulation used a fully random value each cycle with no relationship to the previous reading | Reworked the simulated sensor functions to apply a small random *nudge* to the previous value each cycle (with clamping), producing gradual, realistic-looking drift for logic testing |
| Needed to avoid constant micro-adjustments from sensor noise | Any difference at all between sensors, even negligible ones, would otherwise trigger movement | Introduced a threshold value so the servo only reacts when the difference between readings is significant enough to represent a genuine change in light direction |

---

## 7. Code

See [`Solar_Tracker.ino`](./Solar_Tracker.ino) for the full Arduino source.

```cpp
#include <Servo.h>

Servo myServo;

const int leftPin = A0;
const int rightPin = A1;

int angle = 90;
const int stepSize = 2;
const int threshold = 50;

void setup() {
  myServo.attach(9);
  myServo.write(angle);
  Serial.begin(9600);
}

void loop() {
  int leftValue = analogRead(leftPin);
  int rightValue = analogRead(rightPin);

  if (leftValue > rightValue + threshold) {
    angle -= stepSize;
  } else if (rightValue > leftValue + threshold) {
    angle += stepSize;
  }

  if (angle < 0) {
    angle = 0;
  } else if (angle > 180) {
    angle = 180;
  }

  myServo.write(angle);

  Serial.print("Left: ");
  Serial.print(leftValue);
  Serial.print("  Right: ");
  Serial.print(rightValue);
  Serial.print("  Angle: ");
  Serial.println(angle);

  delay(500);
}
```

---

## 8. Results

- The system successfully tracks the brighter of two light sources, rotating the servo toward it in real time.
- Threshold-based comparison prevents jittery, noise-driven micro-movements.
- Servo angle remains correctly clamped within its 0°–180° mechanical range at all times.
- Behavior verified in Tinkercad simulation by adjusting each LDR's simulated brightness independently and observing correct servo response.

---

---

## 9. Relevance

This project demonstrates a fully autonomous sense-compare-act control loop, directly reflecting the core principles behind automated solar tracking and precision positioning systems used in solar cell manufacturing equipment — systems that continuously sense conditions and adjust position without requiring manual control.
