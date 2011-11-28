/*
  JoystickMouseControl
 
 Controls the mouse from a joystick on an Arduino Leonardo.
 Uses a pushbutton to turn on and off mouse control.
 
 The mouse movement is always relative. This sketch reads 
 two analog inputs that range from 0 to 1023 (or less on either end)
 and translates them into ranges of -60 to 60. 
 The sketch assumes that the joystick resting values are around the 
 middle of the range, but that they vary within a threshold.
 
 WARNING:  When you use the Mouse.move() command, the Arduino takes
 over your mouse!  Make sure you have control before you use the command.
 This sketch includes a pushbutton to toggle the mouse control state, so
 you can turn on and off mouse control.
 
 created 15 Sept 2011
 by Tom Igoe
 
 this code is in the public domain
 
 */

// set pin numbers for switch, joystick axes, and LED:
const int switchPin = 6;      // switch to turn on and off mouse control
const int xAxis = A1;         // joystick X axis  
const int yAxis = A2;         // joystick Y axis
const int ledPin = 5;         // Mouse control LED 

// parameters for reading the joystick:
int range = 12;               // output range of X or Y movement
int responseDelay = 2;       // response delay of the mouse, in ms
int threshold = range/4;      // resting threshold
int center = range/2;         // resting position value
int minima[] = { 
  1023, 1023};                // actual analogRead minima for {x, y}
int maxima[] = {
  0,0};                       // actual analogRead maxima for {x, y}
int axis[] = {
  xAxis, yAxis};              // pin numbers for {x, y}
int mouseReading[2];          // final mouse readings for {x, y}


boolean mouseIsActive = false;    // whether or not to control the mouse
int lastSwitchState = LOW;        // previous switch state

void setup() {
  pinMode(switchPin, INPUT);       // the switch pin
  pinMode(ledPin, OUTPUT);         // the LED pin  
}

void loop() {
  // read the switch:
  int switchState = digitalRead(switchPin);
  // if it's changed and it's high, toggle the mouse state:
  if (switchState != lastSwitchState) {
    if (switchState == HIGH) {
      mouseIsActive = !mouseIsActive;
  // turn on LED to indicate mouse state:
  digitalWrite(ledPin, mouseIsActive);
    } 
  }
  // save switch state for next comparison:
  lastSwitchState = switchState;

// read and scale the two axes:
  int xReading = readAxis(0);
  int yReading = readAxis(1);

// if the mouse control state is active, move the mouse:
  if (mouseIsActive) {
    Mouse.move(xReading, yReading, 0);
  }  
  delay(responseDelay);
}

/*
  reads an axis (0 or 1 for x or y) and scales the 
  analog input range to a range from 0 to <range>
*/

int readAxis(int axisNumber) {
  int distance = 0;    // distance from center of the output range
  
  // read the analog input:
  int reading = analogRead(axis[axisNumber]);

// of the current reading exceeds the max or min for this axis,
// reset the max or min:
  if (reading < minima[axisNumber]) {
    minima[axisNumber] = reading;
  }
  if (reading > maxima[axisNumber]) {
    maxima[axisNumber] = reading;
  }
  
  // map the reading from the analog input range to the output range:
  reading = map(reading, minima[axisNumber], maxima[axisNumber], 0, range);
 
 // if the output reading is outside from the
 // rest position threshold,  use it:
  if (abs(reading - center) > threshold) {
    distance = (reading - center);
  } 
  
  // the Y axis needs to be inverted in order to 
  // map the movemment correctly:
  if (axisNumber == 1) {
    distance = -distance;
  }
  
  // return the distance for this axis:
  return distance;
}
