/* DC motor
 * --------
 *
 * Switch a motor on and off making use of a transistor
 * we use of a BD137 from Fairchild. It is possible to
 * play with the motor's innertia, and use a potentiometer to
 * control the speed.
 *
 * (cleft) 2005 DojoDave for DojoCorp at Madrid Medialab - Spain
 */
 
int motorPin = 6;   // selec the pin where the motor is connected at
int value = 0;      // variable to store the reading from the potentiometer
int potPin = 0;     // analog pin where to plug the potentiometer at
 
void setup() {
  pinMode(motorPin, OUTPUT);   // declare the motor as an output
  Serial.begin(9600);           // connect to the serial port to send values back
}
 
void loop() {
  // read the potentiometer
  value = analogRead(potPin);

  // print its value back to the computer
  Serial.println(value);

  // turn the motor on
  digitalWrite(motorPin,HIGH);
  delay(50);
  digitalWrite(motorPin,LOW);

  // this will control how long the motor is off
  delay(value);
}
