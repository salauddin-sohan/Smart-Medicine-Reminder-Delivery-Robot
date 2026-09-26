# 🤖 Smart Medicine Reminder & Delivery Robot

An automated IoT-based medicine reminder system built with **ESP32**. It features NTP time synchronization, a web server interface for remote alarm scheduling, an LCD display, a servo motor for medicine dispensing, and audiovisual feedback.

---

## 📸 Overview & Features

* **🌐 Web Server & Remote Control:** Set and update alarm times dynamically via Web API parameters (`/setAlarm?hour=HH&min=MM`).
* **⏰ Real-Time Clock Sync:** Synchronizes local time with **pool.ntp.org** (Bangladesh Time, UTC+6).
* **🖥️ Smart Display Output:** Uses a 16x2 I2C LCD to display time, system status, patient/medicine info, and notifications.
* **💊 Automatic Dispenser:** Servo motor rotates to 90° to deliver medicine when the alarm triggers.
* **🚨 Alarm & Acknowledgement:** Red LED and Buzzer alert the patient. The alarm stops and confirms with a "Thank You" message when the physical push button is pressed.
* **🌐 CORS Support:** Enabled CORS handling to allow requests from external web apps/dashboards.

---

## 🛠️ Hardware Components Required

* **Microcontroller:** ESP32 Development Board
* **Display:** 16x2 LCD with I2C Adapter (Address: `0x27`)
* **Actuator:** Servo Motor (connected to GPIO 18)
* **Alerts:** 
  * Buzzer (GPIO 19)
  * Red LED (GPIO 25)
  * Green LED (GPIO 26)
* **Input:** Push Button (GPIO 27) with Internal Pull-up

---

## 🔌 Circuit Pin Mapping

| Component | ESP32 Pin |
| :--- | :--- |
| **Servo Motor** | GPIO 18 |
| **Buzzer** | GPIO 19 |
| **Red LED** | GPIO 25 |
| **Green LED** | GPIO 26 |
| **Push Button** | GPIO 27 |
| **LCD SDA** | GPIO 21 (Default I2C) |
| **LCD SCL** | GPIO 22 (Default I2C) |

---

## 📂 Project Structure

```text
Medicine-Delivery-Robot/
├── README.md               # Documentation
├── code/
│   └── medicine_robot.ino  # ESP32 Main Firmware
└── images/
    ├── robot.jpg           # Hardware photo
    └── circuit.png         # Circuit setup
