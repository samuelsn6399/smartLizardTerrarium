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
  - Closes valve for a user-defined duration (1 min - 24 hour)
  - Fully cycle-controlled via timers

- 💾 **SD Card Data Logging**
  - Logs timestamps, humidity, temperature, and valve status to `data(I).txt`
    - '(I)' denotes the variable data file name starting at 0 and incrementing by +1

- 🖲️ **Button Controls**
  - **Short press ()**: 
    - tToggles LCD display between environment and valve info
  - **Long press (3s)**: 
    - Resets valve timer when viewing the valve timer display(forces valve open + resets cycle)
    - Start/stop data logging when viewing the enviro data display(start logging data + create new file to store data on SD card)

- 🔘 **Potentiometer Interface**
  - Sets the spray interval:
    - 1 min, 2 min, 3 min, 5 min, 10 min, 20 min, 30 min
    - 1 hr, 3 hr, 6 hr, 12 hr, 24 hr

---

## ⚙️ Hardware Requirements

- Arduino Uno (or compatible)
- DHT11 sensor
- 16x2 LCD (parallel interface)
- SD card module
- Solenoid Valve 12V (controlled via digital pin A1)
- Push button (on pin 7)
- Potentiometer (on pin A0)
- 10kΩ resistor (for button pull-up, if not using `INPUT_PULLUP`)
- TIP102 transister (to control 12 V solenoid with 5 V Arduino logic)
- 1kΩ resistor (for Arduino pin to transister)
- IN40001 diode (prevent back emf into Arduino when switching solenoid on/off)
- 12 V DC power supply

---

## 🛠 Configuration

You can change key timings directly in the `.ino` file:
```cpp
const unsigned long valveOpenDuration = 60000; // [ms]    // open valve for 1 minute
const unsigned long logInterval = 5000; // [ms]           // log data every 5 seconds
const int buttonInterval = 3000; // [ms]                  // define a long button press
```

---

## 📋 Usage

1. Upload the sketch to your Arduino.
2. Open the serial monitor to debug (optional).
3. Watch live readings on the LCD.
4. Use the button:
   - Short press to toggle display.
   - Long press (greater than 3 seconds) to reset the valve timer and start/stop data logging.
5. Use potentiometer:
   - Set the spray interval.
---

## 📁 Output Example

LCD Display:
```
T: 24°F H: 45
```
Valve Display:
```
Valve ON: 0.75 min
```
or
```
Valve OFF: 1.96 min
```
