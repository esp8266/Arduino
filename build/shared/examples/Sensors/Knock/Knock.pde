/* Knock Sensor
 * by DojoDave <http://www.0j0.org>
 *
 * Program using a Piezo element as if it was a knock sensor.
 *
 * We have to basically listen to an analog pin and detect 
 * if the signal goes over a certain threshold. It writes
 * "knock" to the serial port if the Threshold is crossed,
 * and toggles the LED on pin 13.
 *
 * http://www.arduino.cc/en/Tutorial/Knock
 */

int ledPin = 13;      // led connected to control pin 13
int knockSensor = 0;  // the knock sensor will be plugged at analog pin 0
byte val = 0;         // variable to store the value read from the sensor pin
int statePin = LOW;   // variable used to store the last LED status, to toggle the light
int THRESHOLD = 100;  // threshold value to decide when the detected sound is a knock or not

void setup() {
 pinMode(ledPin, OUTPUT); // declare the ledPin as as OUTPUT
 Serial.begin(9600);       // use the serial port
}

void loop() {
  val = analogRead(knockSensor);    // read the sensor and store it in the variable "val"
  if (val >= THRESHOLD) {
    statePin = !statePin;           // toggle the status of the ledPin (this trick doesn't use time cycles)
    digitalWrite(ledPin, statePin); // turn the led on or off
    Serial.println("Knock!");          // send the string "Knock!" back to the computer, followed by newline
  }
  delay(100);  // we have to make a delay to avoid overloading the serial port
}

