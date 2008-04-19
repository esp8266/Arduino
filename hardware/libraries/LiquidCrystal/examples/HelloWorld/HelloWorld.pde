#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 2, 7, 8, 9, 10);

void setup()
{
  lcd.print("hello, world!");
}

void loop()
{
}
