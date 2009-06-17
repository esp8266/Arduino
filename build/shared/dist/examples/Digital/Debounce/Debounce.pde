/* 
 Debounce
 
 Each time the input pin goes from LOW to HIGH (e.g. because of a push-button
 press), the output pin is toggled from LOW to HIGH or HIGH to LOW.  There's
 a minimum delay between toggles to debounce the circuit (i.e. to ignore
 noise).  
 
 The circuit:
 * LED attached from pin 13 to ground
 * pushbutton attached from pin 7 to +5V
 * 10K resistor attached from pin 7 to ground
 
 * Note: On most Arduino boards, there is already an LED on the board
 connected to pin 13, so you don't need any extra components for this example.
 
 
 created 21 November 2006
 by David A. Mellis
 modified 17 Jun 2009
 by Tom Igoe
 
 
 http://www.arduino.cc/en/Tutorial/Debounce
 */

// constants won't change. They're used here to 
// set pin numbers:
const int buttonPin = 7;     // the number of the pushbutton pin
const int ledPin =  13;      // the number of the LED pin

// Variables will change:
int ledState = HIGH;         // the current state of the output pin
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin

// the follow variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 200;   // the debounce time, increase if the output flickers

void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  buttonState = digitalRead(buttonPin);

  // check to see if you just pressed the button 
  // (i.e. the input went from LOW to HIGH),  and you've waited 
  // long enough since the last press to ignore any noise:  
  if ((buttonState == HIGH) && 
    (lastButtonState == LOW) && 
    (millis() - lastDebounceTime) > debounceDelay) {
    // toggle the output
    if (ledState == HIGH) {
      ledState = LOW;
    } else {
      ledState = HIGH;
    }
    // ... and store the time of the last button press
    // in a variable:
    lastDebounceTime = millis();    
  }

  // set the LED using the ledState variable:
  digitalWrite(ledPin, ledState);

  // save the buttonState.  Next time through the loop,
  // it'll be the lastButtonState:
  lastButtonState = buttonState;
}
