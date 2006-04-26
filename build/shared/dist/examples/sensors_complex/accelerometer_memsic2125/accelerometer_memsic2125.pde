/* Accelerometer Sensor
 * --------------------
 *
 * Reads an 2-D accelerometer
 * attached to a couple of digital inputs and
 * sends their values over the serial port; makes
 * the monitor LED blink once sent
 *
 *
 * http://www.0j0.org
 * copyleft 2005 K3 - Malmo University - Sweden
 * @author: Marcos Yarza
 * @hardware: Marcos Yarza
 * @project: SMEE - Experiential Vehicles
 * @sponsor: Experiments in Art and Technology Sweden, 1:1 Scale
 */

int ledPin = 13;
int xaccPin = 7;
int yaccPin = 6;
int value = 0;
int accel = 0;
char sign = ' ';

int timer = 0;
int count = 0;

void setup() {
  Serial.begin(9600); // Sets the baud rate to 9600
  pinMode(ledPin, OUTPUT);
  pinMode(xaccPin, INPUT);
  pinMode(yaccPin, INPUT);
}

/* (int) Operate Acceleration
* function to calculate acceleration
* returns an integer
*/
int operateAcceleration(int time1) {
  return abs(8 * (time1 / 10 - 500));
}

/* (void) readAccelerometer
* procedure to read the sensor, calculate
* acceleration and represent the value
*/
void readAcceleration(int axe){
  timer = 0;
  count = 0;
  value = digitalRead(axe);
  while(value == HIGH) { // Loop until pin reads a low
    value = digitalRead(axe);
  }
  while(value == LOW) { // Loop until pin reads a high
    value = digitalRead(axe);
  }
  while(value == HIGH) { // Loop until pin reads a low and count
    value = digitalRead(axe);
    count = count + 1;
  }
  timer = count * 18; //calculate the teme in miliseconds

  //operate sign
  if (timer > 5000){
    sign = '+';
  }
  if (timer < 5000){
    sign = '-';
  }

  //determine the value
  accel = operateAcceleration(timer);

  //Represent acceleration over serial port
  if (axe == xaccPin){
    Serial.print('X');
  } else {
    Serial.print('Y');
  }
  Serial.print(sign);
  Serial.print(accel);
  Serial.println();
}

void loop() {
  readAcceleration(xaccPin); //reads and represents acceleration X
  readAcceleration(yaccPin); //reads and represents acceleration Y
  digitalWrite(ledPin, HIGH);
  delay(300);
  digitalWrite(ledPin, LOW);
} 
