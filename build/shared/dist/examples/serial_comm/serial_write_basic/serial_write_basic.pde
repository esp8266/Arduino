/* Serial Write Basic
 * ------------------ 
 *
 * turns on and off a light emitting diode(LED) connected to digital  
 * pin 13. The LED will light up when pressing a button. At the same
 * time, Arduino will send two different strings over the serial
 * port depending if the button is pressed or released.
 *
 * Created 1 December 2005
 * copyleft 2005 DojoDave <http://www.0j0.org>
 * http://arduino.berlios.de
 *
 */

int ledPin = 13;   // select the pin for the LED
int buttonPin = 2; // select the pin for the button
int val = 0;       // variable to store the data from the serial port

void setup() {
  pinMode(ledPin,OUTPUT);   // declare the LED's pin as output
  pinMode(buttonPin, INPUT);   // delcare the button pin as input
  Serial.begin(9600);        // connect to the serial port
}

void loop () {
  // read the button and store the value
  val = digitalRead(buttonPin);

  // if the button is at HIGH, turn the LED on, off otherwise
  if (val == HIGH) {
    Serial.print("HIGH");
    digitalWrite(ledPin, HIGH);
  } else {
    Serial.print("LOW");
    digitalWrite(ledPin, LOW);
  }

  Serial.println();
  delay(1000);      // convenient to use delays when sending stuff back to the comp.
}
