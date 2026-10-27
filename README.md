# Microcontrollers 1 Labs

Lab code for an intro microcontrollers course. Board: Viva Board 5.4 with a PIC16F1829 (MPLAB X, Snap programmer) and an ATmega328P (Arduino IDE, "Arduino Uno").

| Lab | Topic | Language | Status |
|-----|-------|----------|--------|
| [Lab1](Lab1) | Blink LED | asm (pic-as) + Arduino | Tested on hardware |
| [Lab2](Lab2) | Blink LED with TMR0, indirect addressing | asm | Tested on hardware |
| [Lab3](Lab3) | Interrupts (TMR0, IOC) | asm + Arduino | Tested on hardware (ATmega side not tested) |
| [Lab4A](Lab4A) | PWM with pot | asm | Tested on hardware |
| [Lab4B](Lab4B) | Intro to C (UART) | C (XC8) | Not tested on hardware |
| [Lab5](Lab5) | PWM | C (XC8) | Not tested on hardware |
| [Lab6](Lab6) | Capacitive touch | C (XC8) | Not tested on hardware |
| [Lab7](Lab7) | UART | C (XC8) | Not tested on hardware |
| [Lab8](Lab8) | I2C LCD | C (XC8) + Arduino | Not tested on hardware |
| [Lab9](Lab9) | Sleep | C (XC8) | Not tested on hardware |

Labs 4B and 7 include `UART.h`, which isn't in this repo.

## Build notes
- Labs 1 to 4A: pic-as v3.10 toolchain. In Project Properties > Global Options > Additional options add
  `-Wl,-pMyCode=0h -Wl,--fixupoverflow=ignore`
- Labs 4B on: XC8 C compiler, no extra flags.
- Every project: Connected Hardware Tool = Snap (not Simulator), and Set as Main Project.
- RGB LED is common anode (write 0 to turn a color on). Green RA5, Blue RA2, Red RC6.
