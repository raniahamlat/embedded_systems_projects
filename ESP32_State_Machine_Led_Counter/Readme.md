# ESP32 4-Bit Binary Counter

A small ESP32 embedded-systems project implementing a 4-bit binary counter controlled by a push button.

## Objective

This project was developed to practice:

* GPIO configuration
* Binary counters
* Bit shifting and masking
* Arrays and loops
* GPIO interrupts
* Button debouncing
* Non-blocking timing with `millis()`

## System

Four LEDs represent the four bits of a modulo-16 counter.

```text
Q3 Q2 Q1 Q0
 │  │  │  │
LED LED LED LED
```

A push button connected to GPIO4 increments the counter.

```text
0000 → 0001 → 0010 → ... → 1111 → 0000
```

## Pin Configuration

| Function    |   GPIO |
| ----------- | -----: |
| Q0 / LSB    | GPIO18 |
| Q1          | GPIO19 |
| Q2          | GPIO21 |
| Q3 / MSB    | GPIO22 |
| Push button |  GPIO4 |

## Main Concepts

### Bit extraction

Each counter bit is extracted using:

```cpp
(counter >> i) & 1
```

### Non-blocking timing

Button debouncing is implemented using:

```cpp
millis()
```

instead of `delay()`.

### Interrupt

The push button uses a GPIO interrupt configured on the falling edge:

```cpp
attachInterrupt(
    digitalPinToInterrupt(buttonPin),
    buttonISR,
    FALLING
);
```

## Documentation

* [System Overview](docs/01_system_overview.md)
* [Hardware & Pin Configuration](docs/02_hardware_and_pins.md)
* [Software Design](docs/03_software_design.md)

## Technologies

* ESP32
* C/C++
* GPIO
* Interrupts
* `millis()`
* Bit manipulation
* Embedded programming
