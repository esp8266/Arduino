#include <Ultrasound.h>

Ultrasound ultrasound(2);

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  Serial.print("Ping took: ");
  Serial.print(ultrasound.readMicroseconds());
  Serial.print(" us. Distance is: ");
  Serial.print(ultrasound.readCentimeters());
  Serial.print(" cm, ");
  Serial.print(ultrasound.readInches());
  Serial.print(" in.");
  Serial.println();
  
  delay(1000);
}
