#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 2, 7, 8, 9, 10);

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  if (Serial.available()) {
    delay(100);
    lcd.clear();
    while (Serial.available() > 0) {
      lcd.write(Serial.read());
    }
  }
}
