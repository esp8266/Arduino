// Fading LED 
// by BARRAGAN <http://people.interaction-ivrea.it/h.barragan> 

int value = 0;                            // variable to keep the actual value 
int ledpin = 9;                           // light connected to digital pin 9
 
void setup() 
{ 
  // nothing for setup 
} 
 
void loop() 
{ 
  for(value = 0 ; value <= 255; value+=5) // fade in (from min to max) 
  { 
    analogWrite(ledpin, value);           // sets the value (range from 0 to 255) 
    delay(30);                            // waits for 30 milli seconds to see the dimming effect 
  } 
  for(value = 255; value >=0; value-=5)   // fade out (from max to min) 
  { 
    analogWrite(ledpin, value); 
    delay(30); 
  }  
} 
