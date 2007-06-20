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

/* Max/MSP patch for this example

max v2;
#N vpatcher 0 44 587 638;
#P window setfont "Sans Serif" 9.;
#P comment 137 34 100 196617 Read the serial port every two milliseconds;
#P window linecount 1;
#P newex 156 276 27 196617 / 4;
#P number 156 254 35 9 0 1023 3 3 0 0 0 221 221 221 222 222 222 0 0 0;
#P newex 106 276 27 196617 / 4;
#P number 106 254 35 9 0 1023 3 3 0 0 0 221 221 221 222 222 222 0 0 0;
#P newex 55 276 27 196617 / 4;
#P number 55 254 35 9 0 1023 3 3 0 0 0 221 221 221 222 222 222 0 0 0;
#P hidden newex 55 303 112 196617 bgcolor 220 120 192;
#P user multiSlider 167 145 125 91 0. 1023. 1 3449 15 0 0 2 0 0 0;
#M frgb 0 0 0;
#M brgb 255 255 255;
#M rgb2 127 127 127;
#M rgb3 0 0 0;
#M rgb4 37 52 91;
#M rgb5 74 105 182;
#M rgb6 112 158 18;
#M rgb7 149 211 110;
#M rgb8 187 9 201;
#M rgb9 224 62 37;
#M rgb10 7 114 128;
#P user multiSlider 438 145 125 91 0. 1023. 1 3449 15 0 0 2 0 0 0;
#M frgb 0 0 0;
#M brgb 255 255 255;
#M rgb2 127 127 127;
#M rgb3 0 0 0;
#M rgb4 37 52 91;
#M rgb5 74 105 182;
#M rgb6 112 158 18;
#M rgb7 149 211 110;
#M rgb8 187 9 201;
#M rgb9 224 62 37;
#M rgb10 7 114 128;
#P user multiSlider 301 144 125 91 0. 1023. 1 3449 15 0 0 2 0 0 0;
#M frgb 0 0 0;
#M brgb 255 255 255;
#M rgb2 127 127 127;
#M rgb3 0 0 0;
#M rgb4 37 52 91;
#M rgb5 74 105 182;
#M rgb6 112 158 18;
#M rgb7 149 211 110;
#M rgb8 187 9 201;
#M rgb9 224 62 37;
#M rgb10 7 114 128;
#N vpatcher 20 74 303 387;
#P window setfont "Sans Serif" 9.;
#P newex 92 248 62 196617 fromsymbol;
#B color 5;
#P message 134 231 73 196617 
;
#P newex 134 211 62 196617 prepend set;
#B color 5;
#P newex 92 211 40 196617 itoa;
#B color 5;
#P newex 92 191 55 196617 zl group 4;
#P newex 50 158 52 196617 select 10;
#P message 130 86 14 196617 1;
#P message 112 86 14 196617 0;
#P newex 50 132 27 196617 gate;
#P newex 130 64 52 196617 select 82;
#P comment 151 45 25 196617 "R";
#P inlet 130 44 15 0;
#P outlet 92 270 15 0;
#P fasten 5 0 4 0 117 111 55 111;
#P fasten 6 0 4 0 135 111 55 111;
#P connect 4 0 7 0;
#P fasten 3 1 4 1 177 128 72 128;
#P connect 7 0 8 0;
#P connect 7 1 8 0;
#P connect 8 0 9 0;
#P connect 9 0 12 0;
#P connect 12 0 0 0;
#P connect 7 0 5 0;
#P connect 1 0 3 0;
#P connect 3 0 6 0;
#P connect 9 0 10 0;
#P connect 10 0 11 0;
#P pop;
#P newobj 55 96 49 196617 p parseR;
#N vpatcher 20 74 620 474;
#P window setfont "Sans Serif" 9.;
#P newex 92 250 62 196617 fromsymbol;
#B color 5;
#P message 134 233 73 196617 501
;
#P newex 134 213 62 196617 prepend set;
#B color 5;
#P newex 92 213 40 196617 itoa;
#B color 5;
#P newex 92 193 55 196617 zl group 4;
#P newex 50 160 52 196617 select 10;
#P message 130 88 14 196617 1;
#P message 112 88 14 196617 0;
#P newex 50 134 27 196617 gate;
#P newex 130 66 52 196617 select 66;
#P comment 169 50 25 196617 "B";
#P inlet 130 46 15 0;
#P outlet 92 272 15 0;
#P fasten 5 0 4 0 117 113 55 113;
#P fasten 6 0 4 0 135 113 55 113;
#P connect 4 0 7 0;
#P fasten 3 1 4 1 177 130 72 130;
#P connect 7 0 8 0;
#P connect 7 1 8 0;
#P connect 8 0 9 0;
#P connect 9 0 12 0;
#P connect 12 0 0 0;
#P connect 7 0 5 0;
#P connect 1 0 3 0;
#P connect 3 0 6 0;
#P connect 9 0 10 0;
#P connect 10 0 11 0;
#P pop;
#P newobj 156 96 49 196617 p parseB;
#N vpatcher 589 252 861 563;
#P window setfont "Sans Serif" 9.;
#P comment 153 47 25 196617 "G";
#P newex 92 250 62 196617 fromsymbol;
#B color 5;
#P message 134 233 73 196617 718
;
#P newex 134 213 62 196617 prepend set;
#B color 5;
#P newex 92 213 40 196617 itoa;
#B color 5;
#P newex 92 193 55 196617 zl group 4;
#P newex 50 160 52 196617 select 10;
#P message 130 88 14 196617 1;
#P message 112 88 14 196617 0;
#P newex 50 134 27 196617 gate;
#P newex 130 66 52 196617 select 71;
#P inlet 130 46 15 0;
#P outlet 92 272 15 0;
#P fasten 5 0 3 0 135 113 55 113;
#P fasten 4 0 3 0 117 113 55 113;
#P connect 3 0 6 0;
#P fasten 2 1 3 1 177 130 72 130;
#P connect 6 0 7 0;
#P connect 6 1 7 0;
#P connect 7 0 8 0;
#P connect 8 0 11 0;
#P connect 11 0 0 0;
#P connect 6 0 4 0;
#P connect 1 0 2 0;
#P connect 2 0 5 0;
#P connect 8 0 9 0;
#P connect 9 0 10 0;
#P pop;
#P newobj 106 96 49 196617 p parseG;
#P toggle 55 34 15 0;
#P hidden newex 55 52 46 196617 metro 2;
#P newex 55 72 71 196617 serial a 9600;
#B color 5;
#P window linecount 3;
#P comment 319 50 100 196617 Virtual Color Fader Most of the code by Jamie Allen;
#P connect 3 0 2 0;
#P connect 2 0 1 0;
#P connect 1 0 6 0;
#P connect 6 0 11 0;
#P connect 11 0 12 0;
#P fasten 12 0 10 0 60 298 60 298;
#P fasten 1 0 4 0 60 92 111 92;
#P fasten 4 0 13 0 111 243 111 243;
#P connect 13 0 14 0;
#P connect 14 0 10 1;
#P fasten 1 0 5 0 60 92 161 92;
#P fasten 5 0 15 0 161 243 161 243;
#P connect 15 0 16 0;
#P fasten 16 0 10 2 161 298 162 298;
#P fasten 6 0 9 0 60 129 172 129;
#P fasten 4 0 7 0 111 129 306 129;
#P fasten 5 0 8 0 161 129 443 129;
#P pop;
*/
