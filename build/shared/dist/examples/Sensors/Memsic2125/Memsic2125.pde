/*
 * Memsic2125
 * 
 * Read the Memsic 2125 two-axis accelerometer.  Converts the
 * pulses output by the 2125 into milli-g's (1/1000 of earth's
 * gravity) and prints them over the serial connection to the
 * computer.
 *
 * http://www.arduino.cc/en/Tutorial/Memsic2125
 */

int xpin = 2;
int ypin = 3;

void setup()
{
  Serial.begin(9600);
  pinMode(xpin, INPUT);
  pinMode(ypin, INPUT);
}

void loop()
{
  int pulseX, pulseY;
  int accX, accY;
  
  // read pulse from x- and y-axes
  pulseX = pulseIn(xpin,HIGH);  
  pulseY = pulseIn(ypin,HIGH);
  
  // convert the pulse width into acceleration
  // accX and accY are in milli-g's: earth's gravity is 1000.
  accX = ((pulseX / 10) - 500) * 8;
  accY = ((pulseY / 10) - 500) * 8;

  // print the acceleration
  Serial.print(accX);
  Serial.print(" ");
  Serial.print(accY);
  Serial.println();

  delay(100);
}
