# ESP32 4-Bit Binary Counter

## 1. Project Objective

The objective of this project is to implement a 4-bit binary counter using an ESP32 and four digital outputs.

The project is designed as a practical exercise to review fundamental embedded-systems concepts:

* ESP32 GPIO configuration
* Binary representation
* Bit shifting and bit masking
* Arrays and `for` loops
* Non-blocking timing using `millis()`
* GPIO interrupts
* Push-button debouncing
* Basic separation between interrupt handling and application logic

The counter represents values from **0 to 15** using four LEDs.

---

## 2. System Function

The ESP32 maintains a 4-bit counter stored in a `uint8_t` variable.

Each bit of the counter is connected to one GPIO output:

```text
Q3  Q2  Q1  Q0
 │   │   │   │
LED LED LED LED
```

The four LEDs display the binary representation of the current counter value.

For example:

```text
Decimal    Binary

0          0000
1          0001
2          0010
3          0011
4          0100
5          0101
...
14         1110
15         1111
```

When the counter reaches `15`, the next increment returns it to `0`.

---

## 3. User Input

A push button connected to GPIO4 is used to increment the counter.

The button is configured with the ESP32 internal pull-up resistor.

Therefore:

```text
Button released → HIGH
Button pressed  → LOW
```

The transition from HIGH to LOW generates a GPIO interrupt.

---

## 4. Interrupt Handling

The button interrupt does not directly perform the counter operation.

Instead, the Interrupt Service Routine (ISR) sets a flag:

```cpp
buttonPressed = true;
```

The main `loop()` then detects the flag and performs:

1. Button debounce
2. Counter increment
3. Binary output update

This keeps the ISR short and separates interrupt handling from the main application logic.

---

## 5. Bit Manipulation

The four LED states are extracted from the counter using:

```cpp
(counter >> i) & 1
```

For each value of `i`, one bit of the counter is extracted.

Example:

```text
counter = 10

Binary:
0000 1010

i = 0 → bit 0 → 0
i = 1 → bit 1 → 1
i = 2 → bit 2 → 0
i = 3 → bit 3 → 1
```

The extracted bit is then written to the corresponding GPIO.

---

## 6. Timing and Debouncing

Mechanical push buttons can generate several rapid electrical transitions when pressed. This phenomenon is known as switch bounce.

A 50 ms debounce interval is used to prevent one physical press from being interpreted as multiple presses.

The debounce mechanism uses:

```cpp
millis()
```

rather than `delay()`.

This allows the main program to remain non-blocking.

---

## 7. Expected Behavior

After startup:

```text
Counter = 0
LEDs    = 0000
```

Each valid button press increments the counter:

```text
0000 → 0001 → 0010 → 0011 → ...
```

After:

```text
1111
```

the next press produces:

```text
0000
```

The project therefore implements a complete **4-bit modulo-16 binary counter** controlled by a push button.
