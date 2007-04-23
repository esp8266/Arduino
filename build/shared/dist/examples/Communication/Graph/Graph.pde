/*
 * Graph
 * 
 * A simple example of communication from the Arduino board to the computer:
 * the value of analog input 0 is printed.  We call this "serial"
 * communication because the connection appears to both the Arduino and the
 * computer as an old-fashioned serial port, even though it may actually use
 * a USB cable.
 *
 * You can use the Arduino serial monitor to view the sent data, or it can
 * be read by Processing, Flash, PD, Max/MSP, etc.  The Processing code
 * below graphs the data received so you can see the value of the analog
 * input changing over time.
 * 
 * http://www.arduino.cc/en/Tutorial/Graph
 */

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  Serial.println(analogRead(0));
  delay(100);
}

/* Processing code for this example

// Graph
// by David A. Mellis
//
// based on Analog In 
// by <a href="http://itp.jtnimoy.com">Josh Nimoy</a>. 

import processing.serial.*;

Serial port;
String buff = "";
int NEWLINE = 10;

// Store the last 64 values received so we can graph them.
int[] values = new int[64];

void setup()
{
  size(512, 256);
  
  println("Available serial ports:");
  println(Serial.list());
  
  // Uses the first port in this list (number 0).  Change this to
  // select the port corresponding to your Arduino board.  The last
  // parameter (e.g. 9600) is the speed of the communication.  It
  // has to correspond to the value passed to Serial.begin() in your
  // Arduino sketch.
  port = new Serial(this, Serial.list()[0], 9600);  
  
  // If you know the name of the port used by the Arduino board, you
  // can specify it directly like this.
  //port = new Serial(this, "COM1", 9600);
}

void draw()
{
  background(53);
  stroke(255);
  
  // Graph the stored values by drawing a lines between them.
  for (int i = 0; i < 63; i++)
    line(i * 8, 255 - values[i], (i + 1) * 8, 255 - values[i + 1]);
    
  while (port.available() > 0)
    serialEvent(port.read());
}

void serialEvent(int serial)
{
  if (serial != NEWLINE) {
    // Store all the characters on the line.
    buff += char(serial);
  } else {
    // The end of each line is marked by two characters, a carriage
    // return and a newline.  We're here because we've gotten a newline,
    // but we still need to strip off the carriage return.
    buff = buff.substring(0, buff.length()-1);
    
    // Parse the String into an integer.  We divide by 4 because
    // analog inputs go from 0 to 1023 while colors in Processing
    // only go from 0 to 255.
    int val = Integer.parseInt(buff)/4;
    
    // Clear the value of "buff"
    buff = "";
    
    // Shift over the existing values to make room for the new one.
    for (int i = 0; i < 63; i++)
      values[i] = values[i + 1];
    
    // Add the received value to the array.
    values[63] = val;
  }
}
*/
