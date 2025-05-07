# 🌡️ DHT11 LCD Weather Station V6

An Arduino-based environmental monitoring system with integrated valve control. The project displays temperature and humidity on a 16x2 LCD, logs data to an SD card, and automates valve cycling based on timers with manual override functionality.

---

## 📦 Features

- 📊 **Temperature & Humidity Monitoring**
  - Uses a DHT11 sensor
  - Displays data on a 16x2 LCD screen
  - Degree symbol supported

- 💧 **Valve Control Automation**
  - Opens valve for a set duration (default: 1 min)
  - Closes valve for a user-defined duration (default: 1 min)
  - Fully cycle-controlled via timers

- 💾 **SD Card Data Logging**
  - Logs humidity, temperature, and timestamps to `data.txt`

- 🖲️ **Button Controls**
  - **Short press (0.5s)**: toggles LCD display between environment and valve info
  - **Long press (5s)**: resets valve timer (forces valve open + resets cycle)

- 🔘 **Potentiometer Interface**
  - Can be used for future expansion (currently not active in logic)

---

## ⚙️ Hardware Requirements

- Arduino Uno (or compatible)
- DHT11 sensor
- 16x2 LCD (parallel interface)
- SD card module
- Valve (controlled via digital pin A1)
- Push button (on pin 7)
- Potentiometer (on pin A0)
- 10kΩ resistor (for button pull-up, if not using `INPUT_PULLUP`)

---

## 🛠 Configuration

You can change key timings directly in the `.ino` file:

```cpp
const unsigned long valveOpenDuration = 60000;     // 1 minute
unsigned long valveCloseDuration = 600000;         // e.g., 10 minutes
```

---

## 📋 Usage

1. Upload the sketch to your Arduino.
2. Open the serial monitor to debug (optional).
3. Watch live readings on the LCD.
4. Use the button:
   - Short press to toggle display.
   - Long press to reset the valve timer.

---

## 📁 Output Example

LCD Display:
```
Temp: 24°CF
Hum: 45%
```

Valve Display:
```
Valve ON 12s
```
or
```
Next ON: 522s
```
