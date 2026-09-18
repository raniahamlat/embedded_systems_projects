# Hardware and Pin Configuration

## 1. Hardware
My project was tested using Velxio online simulator

The system uses:

* ESP32 development board
* 4 LEDs
* 4 current-limiting resistors
* 1 push button
* Jumper wires
* Breadboard


---

## 2. GPIO Configuration

| Component       | ESP32 GPIO | Direction    | Function                              |
|-----------------|-----------:|--------------|---------------------------------------|
| Q0 Yellow LED   | GPIO25     | OUTPUT       | Counter bit 0 — LSB                   |
| Q1 Green LED    | GPIO33     | OUTPUT       | Counter bit 1                         |
| Q2 Red LED      | GPIO32     | OUTPUT       | Counter bit 2                         |
| Q3 Blue LED     | GPIO13     | OUTPUT       | Counter bit 3 — MSB                   |
| Push Button     | GPIO21     | INPUT_PULLUP | Short press: +1 / Long press: −1      |


---

## 3. LED Bit Mapping

The LEDs represent the four bits of the counter.

The mapping is:

```text
GPIO25 → Q0 → 2^0 → 1
GPIO33 → Q1 → 2^1 → 2
GPIO25 → Q2 → 2^2 → 4
GPIO13 → Q3 → 2^3 → 8
```

Therefore:

```text
Q3 Q2 Q1 Q0
 │  │  │  │
 8  4  2  1
```

---

## 4. Example: Displaying Decimal 13

Decimal 13 is:

```text
13 = 1101₂
```

Therefore:

```text
Q3 = 1
Q2 = 1
Q1 = 0
Q0 = 1
```

GPIO states:

| GPIO   | Bit | State |
| ------ | --- | ----- |
| GPIO32 | Q0  | HIGH  |
| GPIO33 | Q1  | LOW   |
| GPIO25 | Q2  | HIGH  |
| GPIO13 | Q3  | HIGH  |

The LEDs therefore display:

```text
Q3 Q2 Q1 Q0
 1  1  0  1
```

---

## 5. Push Button Configuration

The push button is connected between GPIO4 and GND.

```text
        ESP32
          │
       GPIO21
          │
       BUTTON
          │
         GND
```

The GPIO is configured as:

```cpp
pinMode(buttonPin, INPUT_PULLUP);
```

The internal pull-up means that no external pull-up resistor is required.

### GPIO states

When the button is released:

```text
GPIO21 = HIGH
```

When the button is pressed:

```text
GPIO21 = LOW
```

The interrupt is therefore configured to detect the falling edge:

```cpp
attachInterrupt(
    digitalPinToInterrupt(buttonPin),
    buttonISR,
    FALLING
);
```

---

## 6. LED Connection

Each LED should have a current-limiting resistor.

Typical configuration:

```text
ESP32 GPIO ─── resistor ───► LED ───► GND
```

The resistor limits the current through the LED and protects both the LED and the ESP32 GPIO.

---

## 7. Pin Configuration in Code

The LED pins are stored in an array:

```cpp
const int pins[4] = {32, 33, 25, 13};
```

The array index corresponds to the binary bit:

```text
pins[0] → GPIO32 → Q0
pins[1] → GPIO33 → Q1
pins[2] → GPIO25 → Q2
pins[3] → GPIO13 → Q3
```

This allows the four outputs to be configured and updated using a `for` loop instead of repeating the same code four times.

---

## 8. Design Note

The GPIO selection is part of the project design and can be changed if required.

If the pins are changed, the primary configuration that needs to be updated is:

```cpp
const int pins[4] = {32, 33, 25, 13};
```

The logical relationship between the array index and counter bit should remain:

```text
index 0 → bit 0
index 1 → bit 1
index 2 → bit 2
index 3 → bit 3
```
