/* Read Jostick
 * ------------
 *
 * Reads two analog pins that are supposed to be
 * connected to a jostick made of two potentiometers
 *
 * We send three bytes back to the computer: one header and
 * two with data as signed bytes, this will take the form:
 *
 *		Jxy\r\n
 *
 * x and y are integers and sent in ASCII 
 * 
 * created 20 June 2005
 * copyleft 2005 DojoDave for DojoCorp <http://www.0j0.org>
 * http://arduino.berlios.de
 *
 */
 
int ledPin = 13;    // declare pin 13 for the LED
int joyPin1 = 0;    // slider variable connecetd to analog pin 0
int joyPin2 = 1;    // slider variable connecetd to analog pin 1
int value1 = 0;     // variable to read the value from the analog pin 0
int value2 = 0;     // variable to read the value from the analog pin 1

void setup() 
{
  pinMode(ledPin, OUTPUT);    // initializes digital pins 0 to 7 as outputs
  Serial.begin(9600);          // turn on the serial port
}

// function that transformes the data from a scale 0-1024 to a scale 0-9
// and answers back the ASCII value for it
int treatValue(int data)
{
  return (data * 9 / 1024) + 48;
}

void loop() 
{
  value1 = analogRead(joyPin1);   // reads the value of the variable resistor 
  delay(100);			  // this small pause is needed between reading two
				  // analog pins, otherwise we get the same value twice
  value2 = analogRead(joyPin2);   // reads the value of the variable resistor 
                                     
  digitalWrite(ledPin, HIGH);     // turn LED on        
  delay(value1/4);                // wait depending on the value read for one axis
  digitalWrite(ledPin, LOW);      // turn LED off
  delay(value2/4);                // wait depending on the value read for the other axis
  Serial.print('J');                 // write a capital 'J' to the serial port
  Serial.print(treatValue(value1), BYTE);  // send the treated value for sensor 1
  Serial.print(treatValue(value2), BYTE);  // send the treated value for sensor 2
  Serial.println();
}
