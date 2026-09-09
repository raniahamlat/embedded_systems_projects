# 🔧 Embedded Systems Projects

Hands-on embedded projects built to develop practical skills in **C/C++, ESP32, Arduino, STM32, sensors, IoT, automation, and real-time systems**.

## 🚀 Project Path

`GPIO` → `Timers` → `Bit Manipulation` → `ADC` → `State Machines` → `Sensors` → `I²C` → `UART` → `SPI` → `STM32` → `FreeRTOS` → `CAN`

## 📂 Projects

Each project has its **own README** with the objectives, hardware, concepts, implementation, testing, and results.

| #  | Project | Platform | Status |
| -- | ------- | -------- | ------ |
| 01 | 🔢 LED Counter | ESP32 | ✅ |
| 02 | 🎛️ ADC Potentiometer | ESP32 | ✅ |
| 03 | 🧠 State Machine + 4-bit Counter | ESP32 | ✅ |
| 04 | 🎛️ Push-Button Control & Debouncing | ESP32 | ✅ |
| 05 | 📡 MPU6050 Vibration Monitor | ESP32 | 🚧 |
| 06 | 💧 Smart Irrigation | ESP32 / IoT | 🔄 |
| 07 | 🏠 Smart Home | ESP32 / IoT | 🔄 |
| 08 | 📡 Sensor & Communication Projects | ESP32 | 📌 |
| 09 | ⚙️ STM32 Projects | STM32 | 📌 |
| 10 | ⏱️ FreeRTOS Projects | ESP32 / STM32 | 📌 |
| 11 | 🔗 CAN Bus Projects | STM32 | 📌 |

**Legend:** ✅ Completed · 🔄 will Be documented · 🚧 In progress · 📌 Planned

---

## 🧠 Skills Developed

### Embedded C/C++
- GPIO configuration and control
- Bit manipulation, masking, and shifting
- `uint8_t`, `uint16_t`, `uint32_t` and fixed-width types
- Pointers and memory concepts
- `struct`, `enum`, and `typedef`
- Modular C/C++ source organization
- Header/source file separation

### ESP32
- GPIO
- Digital inputs and outputs
- Push-button handling
- Software debouncing
- `millis()`-based timing
- Hardware pin limitations
- ADC
- I²C
- Sensor interfacing
- State machines

### Embedded Design
- Finite State Machines
- Fault detection
- Fault persistence
- Threshold-based monitoring
- Sensor calibration
- Digital filtering
- Modular firmware architecture
- Hardware/software debugging

### Engineering Workflow
- Git & GitHub
- Version control
- Structured project documentation
- Hardware testing
- Test-driven experimentation
- Architecture diagrams
- Test result documentation

---

## 📈 Current Progress

### Completed

- ✅ GPIO fundamentals
- ✅ ESP32 LED control
- ✅ 4-bit binary counter
- ✅ Bit masking and bit shifting
- ✅ Push-button input
- ✅ Short/long press detection
- ✅ `millis()` non-blocking timing
- ✅ Finite State Machine implementation
- ✅ ADC potentiometer project
- ✅ I²C device detection
- ✅ MPU6050 raw sensor acquisition
- ✅ Sensor calibration
- ✅ Digital filtering
- ✅ Vibration threshold detection
- ✅ Alarm/warning state handling
- ✅ Fault persistence testing
- ✅ Modular firmware architecture
- ✅ Project documentation and diagrams

### Currently Developing

- 🚧 MPU6050-based vibration monitoring system
- 🚧 Improving sensor filtering and fault detection
- 🚧 Expanding embedded testing methodology

### Next

- 🔜 UART communication
- 🔜 SPI
- 🔜 More ESP32 sensor projects
- 🔜 STM32 bare-metal development
- 🔜 Interrupt-driven firmware
- 🔜 FreeRTOS
- 🔜 CAN bus

---
## 📁 Repository Structure

```text
embedded_systems_projects/
│
├── README.md
├── LICENSE
│
└── ESP32_MPU6050_Vibration_Monitor/
    └── README.md
te_machine/
    ├── 07_fault_persistence/
    └── README.md

