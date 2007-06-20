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

/* Max/MSP patch for this example

#P user multiSlider 285 285 246 167 0. 1023. 1 3433 47 0 0 2 0 0 0;
#M frgb 17 15 198;
#M brgb 59 182 255;
#M rgb2 127 127 127;
#M rgb3 0 0 0;
#M rgb4 37 52 91;
#M rgb5 74 105 182;
#M rgb6 112 158 18;
#M rgb7 149 211 110;
#M rgb8 187 9 201;
#M rgb9 224 62 37;
#M rgb10 7 114 128;
#P window setfont "Sans Serif" 9.;
#P window linecount 1;
#P comment 300 110 191 196617 Click here to get a list of serial ports;
#P window linecount 2;
#P comment 166 265 112 196617 Here's the number from Arduino's analog input;
#P window linecount 1;
#P comment 353 243 215 196617 Convert ASCII to symbol;
#P comment 353 220 215 196617 Convert integer to ASCII;
#P number 285 264 35 9 0 0 0 3 0 0 0 221 221 221 222 222 222 0 0 0;
#P newex 285 243 62 196617 fromsymbol;
#B color 5;
#P newex 285 220 40 196617 itoa;
#B color 5;
#P newex 285 197 55 196617 zl group 4;
#P newex 229 159 67 196617 select 10 13;
#P toggle 229 66 15 0;
#P newex 229 88 52 196617 metro 10;
#P message 264 110 32 196617 print;
#P newex 229 135 71 196617 serial a 9600;
#P window linecount 2;
#P comment 34 88 191 196617 Read serial input buffer every 10 milliseconds;
#P window linecount 3;
#P comment 302 159 215 196617 If you get newline (ASCII 10) \, send the list. If you get return (ASCII 13) do nothing. Any other value \, add to the list;
#P window linecount 1;
#P comment 256 66 100 196617 Click to start;
#P connect 6 0 5 0;
#P connect 5 0 3 0;
#P connect 11 0 16 0;
#P connect 10 0 11 0;
#P connect 9 0 10 0;
#P connect 8 0 9 0;
#P fasten 7 0 8 0 234 188 290 188;
#P fasten 7 2 8 0 290 200 290 200;
#P connect 3 0 7 0;
#P fasten 4 0 3 0 269 130 234 130;
#P window clipboard copycount 17;
*/
