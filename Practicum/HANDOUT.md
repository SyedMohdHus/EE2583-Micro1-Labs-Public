# Micro 1 Practicum (transcribed handout)

Typed up from the 2-page paper handout. The sign-in section with student names was left out.

## Equipment (per group)
- Each student's own Viva board with a blink program of some kind working
- Laptop with working MPLAB X and XC8 loaded, SNAP programmer and micro USB cable
- Shell program set up with config words
- Required include files pre-loaded: `i2c.h` and `SSD1306.h` (the OLED display file)
- Given in class: 128x64 OLED I2C display, analog board with temp sensor, POT and photoresistor
- Some way to power the Viva board: the CP2102 soldered onto the Arduino side with cable
- Starter code in a .TXT file from Canvas

Help video: https://youtu.be/1QF9AiAjC_I

## Setup
The starter code and include files are on Canvas, but you have to set up the include files in MPLAB X first and then paste in the downloaded code. Don't type the extension: type only `i2c` and let the IDE add `.h`, otherwise you get `i2c.h.h`.

You must create the headers in the IDE (right click Header Files > New > XC8 header file), not just paste them into the project directory. If you only put them in the project folder, the IDE will not recognize them. Same for `main.c`: create it in the IDE, then paste in the starter code. Name the project "Practicum".

Include files:
```c
#include "i2c.h"
#include "SSD1306.h"
```

Main file contains:
- A UTSA 1K constant splash screen stored in flash
- A function to set up pins and clock
- A function to init the A2D converter
- A function to read the A2D input, do the conversion and return the value

## Task
- Display the splash "UTSA" screen (already done), then your group name (make one up, replace the instructor's name) for 2 seconds, then display the POT reading on the 128x64 OLED using the large 16x16 font.
- Load the starter code and the 2 include files and run it first to be sure everything is set up OK, then modify it.

The main body calls the OLED init, writes the splash screen, writes the name, waits 2 seconds and erases. Then read the POT with the provided function and display it with `Out8` or `Out16`. Add some spaces (4 or so) after the `%d` so leftover digits from bigger numbers get erased when smaller numbers are shown.

```c
void SSD1306_Out8 ( char row, char col, char *texto, char color );
void SSD1306_Out16( char row, char col, char *texto, char color );
```

- `row`, `col`, pointer to the sprintf buffer (just the array name, `Sout`), and `color`: 0 = dark text on a light background, 1 = light.
- Rows are 0 to 7, columns 0 to 15. `Out16` uses 2 rows per letter, `Out8` uses 1. Text that overflows wraps around.
- Read the pot with something like `Pot = Get_A2D_Result();`

## Grading
Minimum 70 if you at least have your Viva board with a blink program. Project correct 90, mostly 80, some good work 70, plus 10 pts for Viva board present and blinking, plus extra credit.
