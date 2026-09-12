# 🌡️ Arduino Digital Weather Station

> A minimal-wire, no-breadboard weather station built with an **Arduino Uno**, **DHT11** sensor, and a **1602 I²C LCD Display** — displaying live temperature and humidity with zero external components.

---

<div align="center">

![Live Display](https://github.com/user-attachments/assets/ca0c6beb-20fd-4b6f-bfa6-2c14c38b366f)

*Live reading: 31.0 °C · 63.0 % RH — captured during development*

</div>

---

## 📋 Table of Contents

- [Overview](#-overview)
- [Hardware](#-hardware)
- [Wiring Diagram](#-wiring-diagram)
- [Pinout Reference](#-pinout-reference)
- [Libraries](#-libraries)
- [Installation](#-installation)
- [How It Works](#-how-it-works)
- [Project Photos](#-project-photos)
- [Troubleshooting](#-troubleshooting)
- [License](#-license)

---

## 🔍 Overview

This project turns an Arduino Uno into a live indoor weather monitor using only **7 jumper wires** and **no additional resistors, potentiometers, or breadboard** required. The I²C protocol reduces LCD wiring from 6+ data lines down to just 2 (SDA + SCL), keeping the build clean and compact.

**Features:**
- ✅ Live temperature in °C with custom `°` pixel symbol
- ✅ Live relative humidity in %
- ✅ Flicker-free 2-second refresh (no `lcd.clear()` during runtime)
- ✅ Sensor error detection with on-screen retry message
- ✅ Serial Monitor output for debugging
- ✅ Startup splash screen with 1.5 s sensor warm-up delay

---

## 🛒 Hardware

| # | Component | Notes |
|---|-----------|-------|
| 1 | Arduino Uno R3 | Any USB-programmable Uno will work |
| 2 | DHT11 Temperature & Humidity Sensor | 3-pin module version (has onboard resistor) |
| 3 | 1602 LCD Display with I²C Adapter | Backpack pre-soldered; blue or green backlight |
| 4 | Jumper wires (×7) | Male-to-female recommended |
| 5 | USB Type-B cable | For power & programming |

> **Important:** Use the **3-pin DHT11 module** (with the small PCB), not the bare 4-pin component. The module has a built-in pull-up resistor, so no external resistor is needed.

---

## 🔌 Wiring Diagram

```
                    ARDUINO UNO
                   ┌───────────────────────────────┐
                   │                               │
    ┌──────────┐   │  5V  ──────────────────┐      │
    │  DHT11   │   │                        │      │
    │  MODULE  │   │  GND ────────────┐     │      │
    │          │   │                  │     │      │
    │  VCC ────┼───┼──────────────────┘     │      │
    │  GND ────┼───┼──────────────────┘     │      │
    │  DAT ────┼───┼── D2                   │      │
    └──────────┘   │                        │      │
                   │                        │      │
    ┌──────────┐   │  A4 (SDA) ─────┐       │      │
    │ 1602 LCD │   │  A5 (SCL) ──┐  │       │      │
    │  + I²C   │   │             │  │       │      │
    │  ADAPTER │   │  5V ────────┼──┼───────┘      │
    │          │   │  GND ───────┼──┼───────┘      │
    │  GND ────┼───┼─────────────┘  │              │
    │  VCC ────┼───┼────────────────┘              │
    │  SDA ────┼───┼── A4                          │
    │  SCL ────┼───┼── A5                          │
    └──────────┘   │                               │
                   └───────────────────────────────┘
```

### Wire Colour Guide *(from the build photos)*

| Wire Colour | Signal |
|-------------|--------|
| 🔴 Red | 5V Power |
| ⚫ Black / Brown | GND |
| 🟡 Yellow | SDA (A4) |
| 🟠 Orange | SCL (A5) |
| ⚪ White | DHT11 Data (D2) |

---

## 📌 Pinout Reference

### I²C LCD (4 pins on the adapter)

| LCD Pin | Connect To |
|---------|-----------|
| GND | Arduino GND |
| VCC | Arduino 5V |
| SDA | Arduino **A4** |
| SCL | Arduino **A5** |

### DHT11 Module (3 pins)

| DHT11 Pin | Connect To |
|-----------|-----------|
| VCC | Arduino 5V |
| GND | Arduino GND |
| Signal / DAT | Arduino **D2** |

> ⚠️ **I²C Address:** The sketch defaults to `0x27`. If your LCD shows nothing after upload, change `#define LCD_ADDR 0x27` to `#define LCD_ADDR 0x3F` and re-upload. Blue-PCB adapters are usually `0x27`; green PCBs are usually `0x3F`.

---

## 📦 Libraries

Install both via **Arduino IDE → Sketch → Include Library → Manage Libraries**:

| Library | Author | Search Term |
|---------|--------|-------------|
| DHT sensor library | Adafruit | `DHT sensor library` |
| Adafruit Unified Sensor | Adafruit | *(auto-installed as dependency)* |
| LiquidCrystal I2C | Frank de Brabander | `LiquidCrystal I2C` |

---

## 🚀 Installation

1. **Clone this repository**
   ```bash
   git clone https://github.com/YOUR_USERNAME/arduino-weather-station.git
   ```

2. **Open the sketch**
   ```
   File → Open → WeatherStation.ino
   ```

3. **Install libraries** (see table above)

4. **Select your board and port**
   ```
   Tools → Board → Arduino Uno
   Tools → Port → COMx (Windows) or /dev/ttyUSB0 (Linux/Mac)
   ```

5. **Upload**
   ```
   Sketch → Upload  (Ctrl+U)
   ```

6. **Done.** The LCD will show a splash screen for ~1.5 s, then display live readings.

---

## ⚙️ How It Works

### Flicker-Free Display Strategy
The sketch tracks the **previous temperature and humidity values**. It only rewrites a line on the LCD when the value has actually changed — and never calls `lcd.clear()` during normal operation (which is what causes the visible flicker). Trailing spaces in each print statement erase any leftover characters if a value becomes shorter.

```cpp
if (temp != lastTemp) {
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    printPadded(temp, 5);   // fixed-width: " 31.0"
    lcd.write(byte(0));     // custom ° glyph
    lcd.print("C  ");
    lastTemp = temp;
}
```

### Custom Degree Symbol (°)
The `°` character is drawn as a 5×8 pixel bitmap and stored in LCD CGRAM slot 0 at startup:

```cpp
byte degreeSymbol[8] = {
  0b00110,
  0b01001,
  0b01001,
  0b00110,
  0b00000, 0b00000, 0b00000, 0b00000
};
```

### Non-Blocking Refresh
Instead of `delay(2000)`, the sketch uses `millis()` so the microcontroller stays responsive:

```cpp
if (now - lastRefresh < REFRESH_MS) return;
lastRefresh = now;
```

---

## 📸 Project Photos

| Arduino Uno (powered up) | Full setup with LCD | LCD close-up |
|:---:|:---:|:---:|
| ![Arduino Uno Board](https://github.com/user-attachments/assets/859a2883-9706-41b3-89f3-ac60ce9fb158) | ![Setup](photos/full-setup.jpg) | ![LCD](photos/lcd-closeup.jpg) |

*Readings shown: 30.8 °C / 64.0 % RH and 31.0 °C / 63.0 % RH — captured live during testing.*

---

## 🛠️ Troubleshooting

| Symptom | Likely Cause | Fix |
|---------|-------------|-----|
| LCD is on but shows nothing | Wrong I²C address | Change `LCD_ADDR` from `0x27` to `0x3F` |
| LCD is completely off | No power or bad SDA/SCL | Check 5V and GND; confirm A4=SDA, A5=SCL |
| Display shows `Sensor Error!` | DHT11 bad read | Check D2 wiring; ensure 3-pin module, not bare component |
| Readings seem stuck | Normal — DHT11 updates ~1×/sec | Sketch refreshes every 2 s by design |
| Garbled characters on LCD | Contrast too low | Adjust the blue potentiometer on the I²C backpack |

---

## 📁 Repository Structure

```
arduino-weather-station/
├── WeatherStation.ino      # Main Arduino sketch
├── README.md               # This file
└── photos/
    ├── arduino-board.jpg   # Arduino Uno powered up
    ├── full-setup.jpg      # Full build with LCD
    └── lcd-closeup.jpg     # LCD close-up showing live readings
```

---

## 🧰 Specifications

| Parameter | Value |
|-----------|-------|
| Microcontroller | ATmega328P (Arduino Uno) |
| Temperature range | 0 – 50 °C (±2 °C accuracy) |
| Humidity range | 20 – 90 % RH (±5 % accuracy) |
| Refresh rate | Every 2 seconds |
| Display | 16 columns × 2 rows, I²C @ 0x27 |
| Power | 5V via USB |
| Wire count | 7 jumper wires total |

---

## 📄 License

This project is released under the **MIT License** — free to use, modify, and distribute with attribution.

---

<div align="center">

Built with ☕ and jumper wires · *No breadboard was harmed in the making of this project*

</div>
