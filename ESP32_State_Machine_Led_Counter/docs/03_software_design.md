# Software Design

## ESP32 4-Bit Counter with Button State Machine

### 1. Overview

This project implements a 4-bit counter using an ESP32, four LEDs, and one push button.

The button supports two different actions:

* **Short press** → increment the counter
* **Long press** → decrement the counter

The counter operates over a 4-bit range:

```text
0 → 1 → 2 → ... → 14 → 15 → 0
```

and in the reverse direction:

```text
15 → 14 → 13 → ... → 1 → 0 → 15
```

The four counter bits are displayed directly on four LEDs.

The software is organized around a small button state machine and separates:

1. Button acquisition
2. Debouncing
3. Press-duration measurement
4. Press classification
5. Counter modification
6. LED output

---

# 2. Software Architecture

The application follows a simple cyclic embedded architecture.

```text
                         START
                           │
                           ▼
                    Initialize GPIO
                           │
                           ▼
                 Initialize Counter
                           │
                           ▼
                  Display Counter
                           │
                           ▼
                    ┌────────────┐
                    │  MAIN LOOP │
                    └──────┬─────┘
                           │
                           ▼
                     readButton()
                           │
                           ▼
                    Read GPIO 21
                           │
                           ▼
                    Debounce Input
                           │
                           ▼
                 Button state changed?
                      /           \
                    NO             YES
                    │               │
                    │               ▼
                    │          Button pressed?
                    │            /       \
                    │          YES        NO
                    │           │          │
                    │           ▼          ▼
                    │      Record time   Calculate
                    │                    duration
                    │                       │
                    │                       ▼
                    │                Duration ≥ 1000 ms?
                    │                  /             \
                    │                YES              NO
                    │                 │                │
                    │                 ▼                ▼
                    │            LONG_PRESS       SHORT_PRESS
                    │                 │                │
                    └────────────┬────┴────────────────┘
                                 │
                                 ▼
                      updateStateMachine()
                                 │
                         ┌───────┴───────┐
                         │               │
                    SHORT_PRESS     LONG_PRESS
                         │               │
                         ▼               ▼
                      counter++       counter--
                         │               │
                         │          0 → 15 wrap
                         │
                    15 → 0 wrap
                         │               │
                         └───────┬───────┘
                                 │
                                 ▼
                         displayCounter()
                                 │
                                 ▼
                          Update 4 LEDs
                                 │
                                 ▼
                    State = NOT_PRESSED
                                 │
                                 ▼
                            MAIN LOOP
```

---

# 3. Main Program Structure

The main program consists of three primary stages:

```text
setup()
   │
   ├── Configure serial communication
   ├── Configure button
   ├── Configure LEDs
   └── Display initial counter value

loop()
   │
   ├── readButton()
   └── updateStateMachine()
```

### `setup()`

The initialization function configures the hardware and establishes the initial counter display.

### `loop()`

The main loop executes continuously:

```cpp
void loop()
{
    readButton();

    updateStateMachine();
}
```

The program therefore follows a polling-based architecture rather than using a hardware interrupt.

---

# 4. Hardware Interface

## 4.1 GPIO Assignment

| Function | GPIO | Description            |
| -------- | ---: | ---------------------- |
| Button   |   21 | Active-low push button |
| Bit 0    |   25 | RED LED                |
| Bit 1    |   33 | GREEN LED              |
| Bit 2    |   32 | YELLOW LED             |
| Bit 3    |   13 | BLUE LED               |

> **Note:** Bit 3 currently uses GPIO 13 in the implementation.

---

# 5. Button Input

The button is configured using the ESP32 internal pull-up resistor:

```cpp
pinMode(BUTTON_PIN, INPUT_PULLUP);
```

The electrical logic is therefore inverted:

| Physical condition | GPIO 21 |
| ------------------ | ------: |
| Button released    |    HIGH |
| Button pressed     |     LOW |

The software detects a press when:

```cpp
stableButtonState == LOW
```

and detects a release when:

```cpp
stableButtonState == HIGH
```

---

# 6. Button Debouncing

Mechanical push buttons do not produce a perfectly clean transition.

When pressed or released, the electrical signal can oscillate briefly:

```text
Ideal:

HIGH ─────────┐
              │
              └──────── LOW


Real button:

HIGH ─────┐ ┌─┐ ┌──────
          └─┘ └─┘
```

Without debouncing, one physical button press could be interpreted as multiple events.

The software therefore uses a **50 ms debounce period**:

```cpp
const unsigned long DEBOUNCE_TIME = 50;
```

When the raw button reading changes:

```cpp
if (reading != lastButtonReading)
{
    lastDebounceTime = millis();
}
```

the debounce timer is restarted.

The new signal is accepted only when it remains stable for more than 50 ms:

```cpp
if ((millis() - lastDebounceTime) > DEBOUNCE_TIME)
```

This produces a stable logical button state.

---

# 7. Press Duration Measurement

When the button becomes stably pressed, the software records the current time:

```cpp
buttonPressTime = millis();
```

The timer starts at the button press event.

When the button is released, the elapsed time is calculated:

```cpp
pressDuration = millis() - buttonPressTime;
```

The measured duration determines the type of button event.

---

# 8. Short and Long Press Detection

The threshold between short and long presses is:

```cpp
const unsigned long LONG_PRESS_TIME = 1000;
```

The decision is:

```text
                Button Released
                       │
                       ▼
              Calculate Duration
                       │
                       ▼
            Duration < 1000 ms?
                 /          \
               YES           NO
                │             │
                ▼             ▼
          SHORT_PRESS      LONG_PRESS
```

Therefore:

```text
< 1000 ms  → Short press
≥ 1000 ms  → Long press
```

The button state is then updated accordingly.

---

# 9. Button State Machine

The software defines four states:

```cpp
enum ButtonState
{
    NOT_PRESSED,
    PRESSED,
    SHORT_PRESS,
    LONG_PRESS
};
```

## State Description

| State         | Meaning                                      |
| ------------- | -------------------------------------------- |
| `NOT_PRESSED` | No button event is currently being processed |
| `PRESSED`     | Button has been detected as pressed          |
| `SHORT_PRESS` | Button was released after less than 1 second |
| `LONG_PRESS`  | Button was released after 1 second or more   |

### State transition concept

```text
                     ┌───────────────┐
                     │ NOT_PRESSED   │
                     └───────┬───────┘
                             │
                     Button pressed
                             │
                             ▼
                     ┌───────────────┐
                     │    PRESSED    │
                     └───────┬───────┘
                             │
                     Button released
                             │
                  ┌──────────┴──────────┐
                  │                     │
             < 1000 ms              ≥ 1000 ms
                  │                     │
                  ▼                     ▼
          ┌───────────────┐     ┌───────────────┐
          │ SHORT_PRESS   │     │  LONG_PRESS   │
          └───────┬───────┘     └───────┬───────┘
                  │                     │
                  └──────────┬──────────┘
                             │
                             ▼
                     ┌───────────────┐
                     │ NOT_PRESSED   │
                     └───────────────┘
```

The `SHORT_PRESS` and `LONG_PRESS` states are event states. After the corresponding action is executed, the state returns to `NOT_PRESSED`.

---

# 10. Counter Logic

The counter is stored as an unsigned 8-bit variable:

```cpp
uint8_t counter = 0;
```

Only the lower four bits are used.

Therefore, the valid logical range is:

```text
0000 → 1111
  0  →  15
```

---

## 10.1 Short Press — Increment

A short press executes:

```cpp
counter++;
counter = counter & 0x0F;
```

The mask:

```text
0x0F = 0000 1111
```

keeps only the four least significant bits.

This creates modulo-16 behavior:

```text
0 → 1 → 2 → 3 → ... → 14 → 15 → 0
```

Example:

```text
15 decimal
= 0000 1111

+1

16 decimal
= 0001 0000

AND 0000 1111

= 0000 0000
= 0
```

---

# 11. Long Press — Decrement

A long press decrements the counter.

The implementation explicitly handles the lower boundary:

```cpp
if (counter == 0)
{
    counter = 15;
}
else
{
    counter--;
}
```

This produces:

```text
15 → 14 → 13 → ... → 2 → 1 → 0 → 15
```

The explicit boundary check makes the intended modulo-16 behavior clear.

---

# 12. Counter-to-LED Mapping

The counter is represented by four binary bits.

```text
counter = 0b1011

Bit 3  Bit 2  Bit 1  Bit 0
  1      0      1      1
  │      │      │      │
  ▼      ▼      ▼      ▼
 BLUE  YELLOW  GREEN   RED
```

The `displayCounter()` function extracts each bit individually.

### Bit 0

```cpp
(counter >> 0) & 0x01
```

### Bit 1

```cpp
(counter >> 1) & 0x01
```

### Bit 2

```cpp
(counter >> 2) & 0x01
```

### Bit 3

```cpp
(counter >> 3) & 0x01
```

Each resulting value is either:

```text
0 → LED OFF
1 → LED ON
```

---

# 13. Bit Manipulation

The expression:

```cpp
(counter >> n) & 0x01
```

performs two operations.

### Step 1 — Shift

The required bit is moved to bit position 0.

Example:

```text
counter = 1011

counter >> 2

0010
```

### Step 2 — Mask

The least significant bit is isolated:

```text
0010
AND
0001
----
0000
```

Therefore, bit 2 is `0`.

This technique allows the software to map a multi-bit numerical value directly to individual GPIO outputs.

---

# 14. Display Function

The complete display operation is centralized in:

```cpp
void displayCounter()
```

Its responsibility is:

```text
Counter value
     │
     ▼
Extract Bit 0 ──────► LED Bit 0
Extract Bit 1 ──────► LED Bit 1
Extract Bit 2 ──────► LED Bit 2
Extract Bit 3 ──────► LED Bit 3
```

This keeps the hardware output logic separate from the button and counter logic.

---

# 15. Complete Event Flow

## Short Press

```text
Button pressed
      │
      ▼
Record press time
      │
      ▼
Button released
      │
      ▼
Calculate duration
      │
      ▼
Duration < 1000 ms
      │
      ▼
SHORT_PRESS
      │
      ▼
counter++
      │
      ▼
Keep lower 4 bits
      │
      ▼
displayCounter()
      │
      ▼
Update LEDs
      │
      ▼
NOT_PRESSED
      │
      ▼
Main Loop
```

---

## Long Press

```text
Button pressed
      │
      ▼
Record press time
      │
      ▼
Button held
      │
      ▼
Button released
      │
      ▼
Calculate duration
      │
      ▼
Duration ≥ 1000 ms
      │
      ▼
LONG_PRESS
      │
      ▼
counter--
      │
      ▼
If counter == 0 → 15
      │
      ▼
displayCounter()
      │
      ▼
Update LEDs
      │
      ▼
NOT_PRESSED
      │
      ▼
Main Loop
```

---

# 16. Main Software Data Flow

```text
             ┌─────────────────┐
             │   GPIO 21       │
             │     BUTTON      │
             └────────┬────────┘
                      │
                      ▼
              readButton()
                      │
                      ├── Debounce
                      │
                      ├── Press timing
                      │
                      └── Event classification
                             │
                   ┌─────────┴─────────┐
                   │                   │
                   ▼                   ▼
              SHORT_PRESS          LONG_PRESS
                   │                   │
                   ▼                   ▼
                counter++           counter--
                   │                   │
                   └─────────┬─────────┘
                             │
                             ▼
                       counter value
                             │
                             ▼
                    displayCounter()
                             │
                  ┌──────────┼──────────┐
                  │          │          │
                  ▼          ▼          ▼
                Bit 0      Bit 1      Bit 2      Bit 3
                  │          │          │          │
                  ▼          ▼          ▼          ▼
               GPIO 25    GPIO 33    GPIO 32    GPIO 13
                  │          │          │          │
                  ▼          ▼          ▼          ▼
                 RED       GREEN      YELLOW      BLUE
```

---

# 17. Timing Parameters

| Parameter         |   Value | Purpose                           |
| ----------------- | ------: | --------------------------------- |
| `DEBOUNCE_TIME`   |   50 ms | Reject mechanical button bouncing |
| `LONG_PRESS_TIME` | 1000 ms | Separate short and long presses   |
| Serial baud rate  |  115200 | Debug output                      |

These parameters are centralized as constants so they can be modified without changing the main state-machine logic.

---

# 18. Design Decisions

### Polling instead of interrupts

The project uses a continuously running main loop:

```cpp
void loop()
{
    readButton();
    updateStateMachine();
}
```

This approach is sufficient for this application because the button timing requirements are relatively slow compared with the ESP32 processing speed.

It also keeps the implementation simple and avoids placing application logic inside an interrupt service routine.

### Separation of responsibilities

The software separates the main responsibilities into functions:

```text
readButton()
    ↓
Input acquisition
Debouncing
Timing
Event classification

updateStateMachine()
    ↓
State processing
Counter modification

displayCounter()
    ↓
Bit extraction
GPIO output
```

This makes the code easier to test, debug, and extend.

---

# 19. Functional Requirements

The software shall:

1. Configure GPIO 21 as an active-low button input.
2. Configure four GPIOs as LED outputs.
3. Debounce the button using a 50 ms stability period.
4. Measure the button press duration using `millis()`.
5. Detect a short press when the duration is less than 1000 ms.
6. Detect a long press when the duration is at least 1000 ms.
7. Increment the counter after a short press.
8. Decrement the counter after a long press.
9. Maintain the counter within the 4-bit range `0–15`.
10. Wrap from `15 → 0` when incrementing.
11. Wrap from `0 → 15` when decrementing.
12. Display the four counter bits using four LEDs.
13. Return to the `NOT_PRESSED` state after processing each button event.

---

# 20. Example Behavior

Starting from:

```text
Counter = 0
LEDs    = 0000
```

### Short press

```text
0 → 1
```

LED representation:

```text
0001
```

### Short press

```text
1 → 2
```

LED representation:

```text
0010
```

### Long press

```text
2 → 1
```

LED representation:

```text
0001
```

### Long press from zero

```text
0 → 15
```

LED representation:

```text
1111
```

### Short press from 15

```text
15 → 0
```

LED representation:

```text
0000
```

---

# 21. Software Design Summary

The project implements a small event-driven state machine on top of a cyclic embedded main loop.

The complete processing chain is:

```text
GPIO Input
    ↓
Debouncing
    ↓
Press Duration Measurement
    ↓
Short / Long Press Classification
    ↓
State Machine
    ↓
Counter Increment / Decrement
    ↓
4-Bit Representation
    ↓
Bit Extraction
    ↓
GPIO LED Output
```

The design demonstrates several fundamental embedded-systems concepts:

* GPIO configuration
* Active-low digital input
* Software debouncing
* Non-blocking time measurement with `millis()`
* Finite state machine design
* Event-based button handling
* Counter wraparound
* Bit masking
* Bit shifting
* Binary-to-GPIO mapping
* Modular software organization
