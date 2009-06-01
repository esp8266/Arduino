#include <LiquidCrystal.h>

// LiquidCrystal display with:
// rs on pin 12
// rw on pin 11
// enable on pin 10
// d4, d5, d6, d7 on pins 5, 4, 3, 2
LiquidCrystal lcd(12, 11, 10, 5, 4, 3, 2);

void setup()
{
  // Print a message to the LCD.
  lcd.print("hello, world!");
}

void loop()
{
}
