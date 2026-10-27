// Lab 8 Challenge 2: ATmega328P (Arduino Uno) writes to the same I2C LCD
// Lab 8 ATmega side
// Library: Tools > Manage Libraries > search "LCD-I2C" > install "LCD-I2C" by Frank Hafele
// Wiring if the board doesn't already share the bus: Uno A4 = SDA, A5 = SCL, plus 5V and GND
// The LCD's PCF8574 is at address 0x27 (same as the PIC code uses).

// If setup() gives a compile error, copy the setup lines from
// File > Examples > LCD-I2C > Hello World (library versions differ a little).

#include <LCD-I2C.h>

LCD_I2C lcd(0x27, 16, 2);   // address, columns, rows

void setup() {
  lcd.begin();
  lcd.display();
  lcd.backlight();
}

void loop() {
  // Part A: Hello World with a flashing backlight (same idea as the library example)
  lcd.clear();
  lcd.print("Hello World");
  lcd.setCursor(0, 1);       // column 0, bottom line
  lcd.print("from ATmega");
  for (int i = 0; i < 3; i++) {
    lcd.noBacklight();
    delay(300);
    lcd.backlight();
    delay(300);
  }
  delay(2000);

  // Part B: some math
  int a = 100;
  int b = 50;
  int sum = a + b;
  lcd.clear();
  lcd.print(a);
  lcd.print(" + ");
  lcd.print(b);
  lcd.print(" = ");
  lcd.print(sum);
  delay(3000);
}
