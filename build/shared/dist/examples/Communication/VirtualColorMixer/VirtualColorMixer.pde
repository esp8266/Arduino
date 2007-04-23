int redPin = 0;
int greenPin = 1;
int bluePin = 2;

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  Serial.print("R");
  Serial.println(analogRead(redPin));
  Serial.print("G");
  Serial.println(analogRead(greenPin));
  Serial.print("B");
  Serial.println(analogRead(bluePin));
  delay(100);
}
 
/* Processing code for this example

// Color Mixer
// by David A. Mellis
//
// Created 2 December 2006
//
// based on Analog In 
// by <a href="http://itp.jtnimoy.com">Josh Nimoy</a>. 
// 
// Created 8 February 2003
// Updated 2 April 2005

import processing.serial.*;

String buff = "";
int rval = 0, gval = 0, bval = 0;
int NEWLINE = 10;

Serial port;

void setup()
{
  size(200, 200);

  // Print a list in case COM1 doesn't work out
  println("Available serial ports:");
  println(Serial.list());

  //port = new Serial(this, "COM1", 9600);
  // Uses the first available port
  port = new Serial(this, Serial.list()[0], 9600);
}

void draw()
{
  while (port.available() > 0) {
    serialEvent(port.read());
  }
  background(rval, gval, bval);
}

void serialEvent(int serial) 
{ 
  // If the variable "serial" is not equal to the value for 
  // a new line, add the value to the variable "buff". If the 
  // value "serial" is equal to the value for a new line,
  //  save the value of the buffer into the variable "val".
  if(serial != NEWLINE) { 
    buff += char(serial);
  } else {
    // The first character tells us which color this value is for
    char c = buff.charAt(0);
    // Remove it from the string
    buff = buff.substring(1);
    // Discard the carriage return at the end of the buffer
    buff = buff.substring(0, buff.length()-1);
    // Parse the String into an integer
    if (c == 'R')
      rval = Integer.parseInt(buff);
    else if (c == 'G')
      gval = Integer.parseInt(buff);
    else if (c == 'B')
      bval = Integer.parseInt(buff);
    // Clear the value of "buff"
    buff = "";
  }
}
*/
