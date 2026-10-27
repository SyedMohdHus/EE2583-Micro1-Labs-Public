# Practicum: OLED pot display

Splash the UTSA logo, show a group name for 2 s, then loop showing the POT value (`Pot=xxxx`) on a 128x64 SSD1306 OLED in the 16x16 font. PIC16F1829, XC8, 4 MHz internal clock.

- `main.c` is my solution. Group name is set by `GROUP_NAME` at the top.
- `i2c.h` and `SSD1306.h` are the course-provided headers, unchanged.
- `starter/` has the original starter code. `HANDOUT.md` is the handout typed up.

## Hookup
- OLED on the I2C header (RB4 SDA, RB6 SCL). Address 0x78 (8-bit).
- Analog board plugged in, POT on RB5/AN11. Pull any other wire on RB5 first.

## Build
- New XC8 project named Practicum, Snap as hardware tool, Set as Main Project.
- Create `i2c.h` and `SSD1306.h` from inside the IDE (Header Files > New > xc8_header.h, type the name without `.h`) and paste the contents in. Same for `main.c` under Source Files. Files only dropped in the folder are not picked up.
- Config has `LVP = ON`. If programming fails, set Snap's program speed to Low.
