 /*
  Loop
 	
 Lights multiple LEDs in sequence, then in reverse.  Demonstrates
 the use of a for() loop and arrays.
 
  	
 The circuit:
 * LEDs attached from pin 2 through 7 to ground
 
  
 created  23 Sep 2007
 by David A. Mellis
 modified 17 Jun 2009
 by Tom Igoe

 	
 http://www.arduino.cc/en/Tutorial/Loop
 
 */

int timer = 100;                   // The higher the number, the slower the timing.
int pins[] = { 2, 3, 4, 5, 6, 7 }; // an array of pin numbers
int pinCount = 6;                  // the number of pins (i.e. the length of the array)

void setup() {
  // the array elements are numbered from 0 to num_pins - 1:
  for (int thisPin = 0; thisPin < pinCount; i++)  { 
  	// set each pin as an output:
    pinMode(pins[thisPin], OUTPUT);      
    }
}

void loop() {
  // loop through the array:
  for (int thisPin = 0; thisPin < pinCount; thisPin++) {
    // turn on the pin with the array element's pin number
    // turning it on:
    digitalWrite(pins[thisPin], HIGH);  
    // pause: 
    delay(timer);                  
    // and turn it off:
    digitalWrite(pins[thisPin], LOW);    
  }
  
  // do the same loop in reverse:
   for (int thisPin = pinCount; thisPin >= 0; thisPin--) {
    // turn on the pin with the array element's pin number
    // turning it on:
    digitalWrite(pins[thisPin], HIGH);  
    // pause: 
    delay(timer);                  
    // and turn it off:
    digitalWrite(pins[thisPin], LOW);    
  }
}