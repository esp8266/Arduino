/* Potentiometer to Processing
 * ---------------------------
 *
 * This program sends data from a potentiometer to Processing
 * over the serial port (works in PC: Windows and Linux, and MAC)
 * The code reads a potentiometer plugged to an analog
 * input and sends the data as a byte back to the computer.
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
 
int ledPin = 13;      // pin for the LED
int potPin = 0;       // analog pin for the potentiometer
int ledStatus = LOW;  // we use a variable to toggle the LEDs state
int val = 0;          // variable to store the potentiometer's reading

void setup() {
  pinMode(ledPin, OUTPUT);  // declare the ledPin as an output
  Serial.begin(9600);        // initialize the serial port
}

void loop() {
  // read the potentiometer's value and store it
  val = analogRead(potPin)/4;

  // send the value over the port
  Serial.print(val, BYTE);

  // wait a bit
  delay(100);

  // change the state of the LED (if HIGH, then LOW, and viceversa)
  ledStatus = !ledStatus;
  digitalWrite(ledPin, ledStatus);
}
