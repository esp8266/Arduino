/* 
 ESP8266 BlinkWithoutDelay by Simon Peter
 Blink the blue LED on the ESP-01 module
 Based on the Arduino Blink without Delay example
 This example code is in the public domain
*/

const int ledPin =  1;  // The blue LED on the ESP-01 module is connected to GPIO1 
                        // (which is also the TXD pin; so we cannot use 
                        // Serial.print() at the same time

int ledState = LOW;     

unsigned long previousMillis = 0;
const long interval = 1000;

void setup() {
  pinMode(ledPin, OUTPUT);
}

void loop()
{
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;   
    if (ledState == LOW)
      ledState = HIGH;  // Note that this switches the LED *off*
    else
      ledState = LOW;   // Note that this switches the LED *on*
    digitalWrite(ledPin, ledState);
  }
}

