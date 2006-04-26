/* Serial Read Basic
 * ----------------- 
 *
 * turns on and off a light emitting diode(LED) connected to digital  
 * pin 13. The LED will light up when receiving a 'H' over the serial
 * port. The LED will blink shortly.
 *
 * Created 1 December 2005
 * copyleft 2005 DojoDave <http://www.0j0.org>
 * http://arduino.berlios.de
 *
 */

int ledPin = 13;   // select the pin for the LED
int val = 0;       // variable to store the data from the serial port

void setup() {
  pinMode(ledPin,OUTPUT);   // declare the LED's pin as output
  Serial.begin(9600);        // connect to the serial port
}

void loop () {
  // read the serial port
  val = Serial.read();

  // if the input is '-1' then there is no data
  // at the input, otherwise check out if it is 'H'
  if (val != -1) {
    if (val == 'H') {
      digitalWrite(ledPin, HIGH);
      delay(200);
      digitalWrite(ledPin, LOW);
    }
  }
}
