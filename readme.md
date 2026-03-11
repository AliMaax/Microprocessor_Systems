# Microprocessor Systems

Welcome to my Microprocessor Systems repository. This collection contains lab assignments and a final project focused on embedded systems development using ARM Cortex-M4F architecture. 

## 🛠️ Hardware & Software Setup

### Hardware Required

* **Microcontroller:** EK-TM4C123GXL - ARM Cortex-M4F Based microcontroller TM4C123G Tiva-C LaunchPad (Previously known as Stellaris Launchpad Board based on LM4F120H5QR microcontroller)
* **Peripherals:** Custom Expansion Board with various electronic components used for lab assignments.

### Software Required

* **IDE:** Keil µVision4. Used for writing, debugging, and flashing code to the Tiva-C LaunchPad.

## 📁 Repository Structure
* **Lab 1 - Lab 5:** Progressive laboratory experiments interfacing the Tiva-C LaunchPad with expansion board components.
* **Misc:** Miscellaneous scripts, tests, and supplementary files.

### 🚀 Final Project: Automatic Whiteboard Duster
The capstone project for this course is an **Automatic Whiteboard Duster** controlled by a Tiva C Series (TM4C123GH6PM) microcontroller. The system automates the erasing process by controlling DC motors via an H-Bridge driver, using hardware PWM for speed control and hardware timers for precise movement durations.

#### 🎥 Project Demonstration
*(Click to view the video demonstration of the duster in action)*
<video src="./Project/Project.mp4" controls="controls" style="max-width: 730px;">
</video>


#### 🧠 Core Logic & State Machine
The core of the system is a non-blocking state machine designed to execute a predefined cleaning path.
* **Cleaning Loop (x4 Passes):** The duster starts by moving **UP** (fast, 7V PWM), steps **RIGHT** slightly (slow, 3V PWM), moves **DOWN** (fast), and repeats this zig-zag pattern across the board.
* **Return Home Sequence:** After completing 4 cleaning passes, the system enters a "Return Home" sequence to reset its position. It drives UP to the top edge, iterates through a loop to shift **LEFT** four times (returning to the original X-coordinate), and finally drives **DOWN** to the original starting point before permanently halting.

#### ⚙️ Hardware Integration
* **Motor Control (GPIO Port A):** Pins PA2-PA5 act as the directional control signals for the motor driver (IN1-IN4), governing the Up/Down and Left/Right actuation.
* **Speed Control (PWM Module 1):** Hardware PWM is generated on Port D (PD0, PD1) to control the enable pins of the motor driver. Different duty cycles (e.g., `PWM_DUTY_7V` vs `PWM_DUTY_3V`) are dynamically applied depending on whether the motion is long (vertical) or short (horizontal stepping).
* **Precise Timing (Timer 0):** Delay loops are managed using Timer 0 in 32-bit one-shot mode (`TIMER0_TAMR_R = 0x01`). The system polls the `TIMER0_RIS_R` register for exact timing rather than relying on inaccurate software delay loops.

## 🚀 Getting Started
1. Clone this repository to your local machine.
2. Open the project workspace files in Keil µVision4.
3. Connect your TivaC TM4C123GH6PM board via USB.
4. Build the target (F7) and Download (F8) the code to the flash memory.

## 📬 Let's Connect
**Ali Maaz** *Electrical Engineering* - **GitHub:** [@AliMaax](https://github.com/AliMaax)
- **LinkedIn:** [Ali Maaz](https://www.linkedin.com/in/ali-maaz-64986b29b)
- **Email:** alimaaz7865@gmail.com