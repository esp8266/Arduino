/*
  Conditionals - while statement

 This example demonstrates the use of  while() statements.

 While the pushbutton is pressed, the sketch runs the calibration routine.
 The  sensor readings during the while loop define the minimum and maximum
 of expected values from the photo resistor.

 This is a variation on the calibrate example.

 The circuit:
 * photo resistor connected from +5V to analog in pin 0
 * 10K resistor connected from ground to analog in pin 0
 * LED connected from digital pin 9 to ground through 220 ohm resistor
 * pushbutton attached from pin 2 to +5V
 * 10K resistor attached from pin 2 to ground

 created 17 Jan 2009
 modified 30 Aug 2011
 by Tom Igoe

 This example code is in the public domain.

 http://arduino.cc/en/Tutorial/WhileLoop

 */


// These constants won't change:
const int sensorPin = A2;       // pin that the sensor is attached to
const int ledPin = 9;           // pin that the LED is attached to
const int indicatorLedPin = 13; // pin that the built-in LED is attached to
const int buttonPin = 2;        // pin that the button is attached to


// These variables will change:
int sensorMin = 1023;  // minimum sensor value
int sensorMax = 0;     // maximum sensor value
int sensorValue = 0;         // the sensor value


void setup() {
  // set the LED pins as outputs and the switch pin as input:
  pinMode(indicatorLedPin, OUTPUT);
  pinMode (ledPin, OUTPUT);
  pinMode (buttonPin, INPUT);
}

void loop() {
  // while the button is pressed, take calibration readings:
  while (digitalRead(buttonPin) == HIGH) {
    calibrate();
  }
  // signal the end of the calibration period
  digitalWrite(indicatorLedPin, LOW);

  // read the sensor:
  sensorValue = analogRead(sensorPin);

  // apply the calibration to the sensor reading
  sensorValue = map(sensorValue, sensorMin, sensorMax, 0, 255);

  // in case the sensor value is outside the range seen during calibration
  sensorValue = constrain(sensorValue, 0, 255);

  // fade the LED using the calibrated value:
  analogWrite(ledPin, sensorValue);
}

void calibrate() {
  // turn on the indicator LED to indicate that calibration is happening:
  digitalWrite(indicatorLedPin, HIGH);
  // read the sensor:
  sensorValue = analogRead(sensorPin);

  // record the maximum sensor value
  if (sensorValue > sensorMax) {
    sensorMax = sensorValue;
  }

  // record the minimum sensor value
  if (sensorValue < sensorMin) {
    sensorMin = sensorValue;
  }
}


