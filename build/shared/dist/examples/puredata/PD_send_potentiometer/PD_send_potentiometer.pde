/* Potentiometer to PD
 * -------------------
 *
 * This program sends data from a potentiometer to PD
 * over the serial port (works in Windows and Linux, MAC?)
 * The code reads a potentiometer plugged to an analog
 * input and sends the data as a byte back to the computer.
 *
 * On the other side there will be a PureData sketch running
 * comport2000 and will use the data to change a sound file
 * properties.
 *
 * In order to make the data transfer as simple as possible
 * we will only send a byte back to the computer, what means
 * that the data coming from the ADC will be divided by 4.
 *
 * (cleft) 2005 DojoDave for K3
 *
 * @author: David Cuartielles
 * @context: ID3 - K3 - MAH - Sweden
 */
 
int ledPin = 13; 
int potPin = 0;
int ledStatus = LOW;

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  Serial.print(analogRead(potPin)/4, BYTE);
  delay(100);
  ledStatus = !ledStatus;
  digitalWrite(ledPin, ledStatus);
}
