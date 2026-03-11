# 🤖 Automatic Whiteboard Duster (ARM Cortex-M4F)

An embedded systems capstone project designed to automate the process of erasing a whiteboard. Built on the **Tiva-C Series (TM4C123GH6PM)** microcontroller, this project utilizes hardware timers, pulse-width modulation (PWM), and a custom state machine to achieve precise, multi-axis motor control without relying on blocking software delays.

## 🎥 Video Demonstration
*(Click the image below to view the duster in action)*

[![Automatic Whiteboard Duster Demonstration](https://img.youtube.com/vi/iQI02sH2h3Y/0.jpg)](https://youtube.com/shorts/iQI02sH2h3Y?feature=share)

---

## 🛠️ Hardware Requirements
* **Microcontroller:** Tiva-C LaunchPad 
* **Motor Driver:** H-Bridge Motor Driver (L298N)
* **Actuators:** 2x DC Motors (controlling Vertical and Horizontal axes)
* **Power Supply:** External 12V/9V supply for motors

## 🔌 Pin Mapping & Connections
The microcontroller interfaces with the motor driver using **GPIO Port A** for directional logic and **GPIO Port D** for PWM speed control.

| Tiva-C Pin | Peripheral Module | Function / Connection |
| :--- | :--- | :--- |
| **PA2** | GPIO (Digital Out) | Motor A (Vertical) - IN1 |
| **PA3** | GPIO (Digital Out) | Motor A (Vertical) - IN2 |
| **PA4** | GPIO (Digital Out) | Motor B (Horizontal) - IN3 |
| **PA5** | GPIO (Digital Out) | Motor B (Horizontal) - IN4 |
| **PD0** | PWM1 Module (Gen A) | Motor A Enable (Speed Control) |
| **PD1** | PWM1 Module (Gen B) | Motor B Enable (Speed Control) |

---

## 🧠 Software Architecture

### 1. The Core State Machine
The system operates on a non-blocking finite state machine (FSM) written in C, handling the exact cleaning path and the return-to-origin sequence.
* `STATE_MOVE_UP`: Drives the vertical motor upwards for 1.75 seconds.
* `STATE_STEP_RIGHT`: Drives the horizontal motor right for 0.15 seconds (shifting to a new uncleaned column).
* `STATE_MOVE_DOWN`: Drives the vertical motor downwards for 1.75 seconds.
* `STATE_RETURN_HOME`: A dedicated sequence triggered after 4 complete cleaning passes. It drives the mechanism back to the top-left starting coordinate.
* `STATE_HALT`: Safely disables all PWM and GPIO outputs.

### 2. Hardware PWM (Speed Control)
Instead of running both motors at maximum voltage, the system dynamically adjusts the duty cycle using the **PWM1 Module**.
* **Vertical Motion (Fast):** Uses `PWM_DUTY_7V` to quickly clear the board.
* **Horizontal Stepping (Slow):** Uses `PWM_DUTY_3V` for short, precise horizontal shifts.

### 3. Hardware Timers (Precise Delays)
Software delay loops (e.g., `for(int i=0; i<1000; i++)`) are highly inaccurate and waste CPU cycles. This project utilizes **Timer 0 in 32-bit One-Shot Mode**. 
* The timer loads a specific value (e.g., `16,000,000` ticks for 1.75s).
* The CPU polls the `TIMER0_RIS_R` flag to detect the exact moment the timeout occurs, ensuring perfectly timed motor actuations.

---

## 🚀 How to Build and Flash
1. Open the project folder in **Keil µVision4** (or a newer version).
2. Ensure the **TivaWare** library paths are correctly linked in the project options.
3. Connect the EK-TM4C123GXL board to your PC via the USB debug port.
4. Compile the code by pressing `F7` (Build Target).
5. Flash the microcontroller by pressing `F8` (Download).
6. Press the **RESET** button on the board to begin the automated cleaning sequence.

---

## 👨‍💻 Author
**Ali Maaz** * Electrical Engineering Student at UET Lahore
* Connect with me on [LinkedIn](https://www.linkedin.com/in/ali-maaz-64986b29b)
