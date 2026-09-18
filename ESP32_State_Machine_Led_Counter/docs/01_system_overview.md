# System Overview

## 1. Project Overview

The **ESP32 State Machine LED Counter** is a digital counter system based on an ESP32 microcontroller.

The system uses:

* One push button as the user input
* Four LEDs to display a 4-bit binary value
* A 4-bit counter with values from `0` to `15`
* Short and long button presses to control the counting direction

The project demonstrates the implementation of a small embedded system combining digital inputs, digital outputs, binary representation, user interaction, and real-time counter control.

---

## 2. System Objective

The objective of this project is to design and implement a simple embedded counter capable of:

* Displaying a 4-bit binary value using four LEDs
* Incrementing the counter with a short button press
* Decrementing the counter with a long button press
* Handling counter wraparound between `0` and `15`
* Providing a clear hardware representation of the counter state

The project is designed as a practical demonstration of fundamental embedded-system concepts using the ESP32.

---

## 3. System Function

The system operates as follows:

1. The user presses the push button.
2. The system determines whether the press is short or long.
3. A short press increments the counter by one.
4. A long press decrements the counter by one.
5. The resulting 4-bit value is displayed using the four LEDs.
6. The system waits for the next button interaction.

The counter operates continuously within the range:

```text
0 → 1 → 2 → ... → 14 → 15 → 0
```

For decrementing:

```text
15 → 14 → 13 → ... → 1 → 0 → 15
```

---

## 4. System Block Diagram

```text
                         ┌────────────────────────┐
                         │         ESP32          │
                         │   Counter Controller   │
                         └───────────┬────────────┘
                                     │
                   ┌─────────────────┴─────────────────┐
                   │                                   │
                   ▼                                   ▼
          ┌─────────────────┐                 ┌──────────────────┐
          │   USER INPUT    │                 │    LED OUTPUT    │
          │                 │                 │                  │
          │ Push Button     │                 │ Q0 → GPIO25      │
          │ GPIO21          │                 │ Q1 → GPIO33      │
          │                 │                 │ Q2 → GPIO32      │
          └─────────────────┘                 │ Q3 → GPIO13      │
                                              └──────────────────┘
```

The push button provides the user input, while the four LEDs represent the four binary bits of the counter.

---

## 5. Hardware Components

| Component               | Quantity | Function               |
| ----------------------- | -------: | ---------------------- |
| ESP32 development board |        1 | Main microcontroller   |
| Push button             |        1 | User input             |
| Yellow LED              |        1 | Counter bit 0 — LSB    |
| Green LED               |        1 | Counter bit 1          |
| Red LED                 |        1 | Counter bit 2          |
| Blue LED                |        1 | Counter bit 3 — MSB    |
| Resistors               |        4 | LED current limiting   |
| Breadboard / simulator  |        1 | Circuit implementation |

---

## 6. GPIO Assignment

| Component     | ESP32 GPIO | Direction    | Function            |
| ------------- | ---------: | ------------ | ------------------- |
| Q0 Yellow LED |     GPIO25 | OUTPUT       | Counter bit 0 — LSB |
| Q1 Green LED  |     GPIO33 | OUTPUT       | Counter bit 1       |
| Q2 Red LED    |     GPIO32 | OUTPUT       | Counter bit 2       |
| Q3 Blue LED   |     GPIO13 | OUTPUT       | Counter bit 3 — MSB |
| Push Button   |     GPIO21 | INPUT_PULLUP | User input          |

### Input Configuration

The push button is connected to **GPIO21** and uses the ESP32 internal pull-up resistor.

```text
Button released → HIGH
Button pressed  → LOW
```

---

## 7. Binary Representation

The four LEDs represent a 4-bit binary number.

```text
Q3   Q2   Q1   Q0
│    │    │    │
MSB             LSB
```

The binary value is formed as:

```text
Counter = Q3 Q2 Q1 Q0
```

For example:

| Decimal | Binary |  Q3 |  Q2 |  Q1 |  Q0 |
| ------: | -----: | --: | --: | --: | --: |
|       0 | `0000` | OFF | OFF | OFF | OFF |
|       1 | `0001` | OFF | OFF | OFF |  ON |
|       2 | `0010` | OFF | OFF |  ON | OFF |
|       3 | `0011` | OFF | OFF |  ON |  ON |
|       4 | `0100` | OFF |  ON | OFF | OFF |
|       5 | `0101` | OFF |  ON | OFF |  ON |
|       6 | `0110` | OFF |  ON |  ON | OFF |
|       7 | `0111` | OFF |  ON |  ON |  ON |
|       8 | `1000` |  ON | OFF | OFF | OFF |
|       9 | `1001` |  ON | OFF | OFF |  ON |
|      10 | `1010` |  ON | OFF |  ON | OFF |
|      11 | `1011` |  ON | OFF |  ON |  ON |
|      12 | `1100` |  ON |  ON | OFF | OFF |
|      13 | `1101` |  ON |  ON | OFF |  ON |
|      14 | `1110` |  ON |  ON |  ON | OFF |
|      15 | `1111` |  ON |  ON |  ON |  ON |

---

## 8. Counter Range

The system uses four binary bits, giving:

```text
2⁴ = 16 possible states
```

Therefore, the counter can represent:

```text
0 to 15
```

The counter wraps around when reaching either limit.

### Increment

```text
15 + 1 → 0
```

### Decrement

```text
0 - 1 → 15
```

This creates a circular 4-bit counting range.

---

## 9. User Interaction

The system provides two different operations depending on the duration of the button press.

| User Action | System Operation  |
| ----------- | ----------------- |
| Short press | Increment counter |
| Long press  | Decrement counter |

### Short Press

```text
Button press
     │
     ▼
Short duration
     │
     ▼
Counter + 1
     │
     ▼
Update LED display
```

### Long Press

```text
Button press
     │
     ▼
Long duration
     │
     ▼
Counter - 1
     │
     ▼
Update LED display
```

---

## 10. Counter Behavior

### Increment Example

Starting from `5`:

```text
5  →  6  →  7  →  8  →  9
```

Binary representation:

```text
0101 → 0110 → 0111 → 1000 → 1001
```

### Decrement Example

Starting from `9`:

```text
9  →  8  →  7  →  6  →  5
```

Binary representation:

```text
1001 → 1000 → 0111 → 0110 → 0101
```

### Wraparound Example

Increment:

```text
14 → 15 → 0 → 1
```

Decrement:

```text
1 → 0 → 15 → 14
```

---

## 11. System Operation Example

Assume the counter initially starts at:

```text
0000
```

The user performs the following actions:

```text
Short press
     ↓
0001

Short press
     ↓
0010

Short press
     ↓
0011

Long press
     ↓
0010

Long press
     ↓
0001
```

The LEDs therefore provide a direct visual representation of the current counter value.

---

## 12. System Characteristics

| Characteristic       | Description               |
| -------------------- | ------------------------- |
| Microcontroller      | ESP32                     |
| Counter width        | 4 bits                    |
| Counter range        | 0–15                      |
| User input           | Push button               |
| Number of outputs    | 4 LEDs                    |
| Counting up          | Short press               |
| Counting down        | Long press                |
| Maximum states       | 16                        |
| Display              | Binary LED representation |
| Button configuration | Internal pull-up          |
| Counter operation    | Circular / wraparound     |

---

## 13. Overall System Concept

The project implements a compact embedded control system in which a single digital input controls a multi-bit digital output.

The ESP32 receives a user command through the push button, determines the requested counting operation, updates the internal counter value, and represents the result through four LEDs.

The architecture can be summarized as:

```text
          USER INPUT
              │
              ▼
       ┌──────────────┐
       │ Push Button  │
       └──────┬───────┘
              │
              ▼
       ┌──────────────┐
       │    ESP32     │
       │ Counter Core │
       └──────┬───────┘
              │
              ▼
       ┌──────────────┐
       │ 4-Bit Value  │
       └──────┬───────┘
              │
              ▼
       ┌──────────────┐
       │ 4 LED Output │
       └──────────────┘
              │
              ▼
       BINARY DISPLAY
```

The project demonstrates the complete path from **user input → embedded processing → digital state → hardware output**, providing a foundation for more advanced embedded control systems.
