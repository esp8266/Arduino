// ADXL3xx
// 
// Reads an Analog Devices ADXL3xx accelerometer and communicates the
// acceleration to the computer.  The pins used are designed to be easily
// compatible with the breakout boards from Sparkfun, available from:
// http://www.sparkfun.com/commerce/categories.php?c=80
//
// http://www.arduino.cc/en/Tutorial/ADXL3xx

// Breakout Board Pinout
// 0: self test
// 1: z-axis
// 2: y-axis
// 3: x-axis
// 4: ground
// 5: vcc

int groundpin = 18;             // analog input pin 4
int powerpin = 19;              // analog input pin 5
int xpin = 3;                   // x-axis of the accelerometer
int ypin = 2;                   // y-axis
int zpin = 1;                   // z-axis (only on 3-axis models)

void setup()
{
  Serial.begin(9600);
  
  // Provide ground and power by using the analog inputs as normal
  // digital pins.  This makes it possible to directly connect the
  // breakout board to the Arduino.  If you use the normal 5V and
  // GND pins on the Arduino, you can remove these lines.
  pinMode(groundpin, OUTPUT);
  pinMode(powerpin, OUTPUT);
  digitalWrite(groundpin, LOW); 
  digitalWrite(powerpin, HIGH);
}

void loop()
{
  Serial.print(analogRead(xpin));
  Serial.print(" ");
  Serial.print(analogRead(ypin));
  Serial.print(" ");
  Serial.print(analogRead(zpin));
  Serial.println();
  delay(1000);
}
