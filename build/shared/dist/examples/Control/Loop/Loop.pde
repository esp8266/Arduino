/*
 * Loop
 * by David A. Mellis
 *
 * Lights multiple LEDs in sequence, then in reverse.  Demonstrates
 * the use of a for() loop and arrays.
 *
 * http://www.arduino.cc/en/Tutorial/Loop
 */

int timer = 100;                   // The higher the number, the slower the timing.
int pins[] = { 2, 3, 4, 5, 6, 7 }; // an array of pin numbers
int num_pins = 6;                  // the number of pins (i.e. the length of the array)

void setup()
{
  int i;

  for (i = 0; i < num_pins; i++)   // the array elements are numbered from 0 to num_pins - 1
    pinMode(pins[i], OUTPUT);      // set each pin as an output
}

void loop()
{
  int i;
  
  for (i = 0; i < num_pins; i++) { // loop through each pin...
    digitalWrite(pins[i], HIGH);   // turning it on,
    delay(timer);                  // pausing,
    digitalWrite(pins[i], LOW);    // and turning it off.
  }
  for (i = num_pins - 1; i >= 0; i--) { 
    digitalWrite(pins[i], HIGH);
    delay(timer);
    digitalWrite(pins[i], LOW);
  }
}
