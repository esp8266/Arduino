/*
  Graph
 
 A simple example of communication from the Arduino board to the computer:
 the value of analog input 0 is sent out the serial port.  We call this "serial"
 communication because the connection appears to both the Arduino and the
 computer as a serial port, even though it may actually use
 a USB cable. Bytes are sent one after another (serially) from the Arduino
 to the computer.
 
 You can use the Arduino serial monitor to view the sent data, or it can
 be read by Processing, PD, Max/MSP, or any other program capable of reading 
 data from a serial port.  The Processing code below graphs the data received 
 so you can see the value of the analog input changing over time.
 
 The circuit:
 Any analog input sensor is attached to analog in pin 0.
  
 created 2006
 by David A. Mellis
 modified 14 Apr 2009
 by Tom Igoe and Scott Fitzgerald
 
 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/Graph
 */

void setup() {
  // initialize the serial communication:
  Serial.begin(9600);
}

void loop() {
  // send the value of analog input 0:
  Serial.println(analogRead(A0));
  // wait a bit for the analog-to-digital converter 
  // to stabilize after the last reading:
  delay(10);
}

/* Processing code for this example
 
 // Graphing sketch
 
 
 // This program takes ASCII-encoded strings
 // from the serial port at 9600 baud and graphs them. It expects values in the
 // range 0 to 1023, followed by a newline, or newline and carriage return
 
 // Created 20 Apr 2005
 // Updated 18 Jan 2008
 // by Tom Igoe
 // This example code is in the public domain.
 
 import processing.serial.*;
 
 Serial myPort;        // The serial port
 int xPos = 1;         // horizontal position of the graph
 
 void setup () {
 // set the window size:
 size(400, 300);        
 
 // List all the available serial ports
 println(Serial.list());
 // I know that the first port in the serial list on my mac
 // is always my  Arduino, so I open Serial.list()[0].
 // Open whatever port is the one you're using.
 myPort = new Serial(this, Serial.list()[0], 9600);
 // don't generate a serialEvent() unless you get a newline character:
 myPort.bufferUntil('\n');
 // set inital background:
 background(0);
 }
 void draw () {
 // everything happens in the serialEvent()
 }
 
 void serialEvent (Serial myPort) {
 // get the ASCII string:
 String inString = myPort.readStringUntil('\n');
 
 if (inString != null) {
 // trim off any whitespace:
 inString = trim(inString);
 // convert to an int and map to the screen height:
 float inByte = float(inString); 
 inByte = map(inByte, 0, 1023, 0, height);
 
 // draw the line:
 stroke(127,34,255);
 line(xPos, height, xPos, height - inByte);
 
 // at the edge of the screen, go back to the beginning:
 if (xPos >= width) {
 xPos = 0;
 background(0); 
 } 
 else {
 // increment the horizontal position:
 xPos++;
 }
 }
 }
 
 */

/* Max/MSP v5 patch for this example
 {
 	"boxes" : [ 		{
 			"box" : 			{
 				"maxclass" : "comment",
 				"text" : "Graph\n\nThis patch takes a string, containing ASCII formatted number from 0 to 1023, with a carriage return and linefeed at the end.  It converts the string to an integer and graphs it.\n\ncreated 2006\nby David A. Mellis\nmodified 14 Apr 2009\nby Scott Fitzgerald and Tom Igoe",
 				"linecount" : 10,
 				"patching_rect" : [ 479.0, 6.0, 344.0, 144.0 ],
 				"numoutlets" : 0,
 				"fontsize" : 12.0,
 				"id" : "obj-32",
 				"fontname" : "Arial",
 				"numinlets" : 1
 			}
 
 		}
 , 		{
 			"box" : 			{
 				"maxclass" : "newobj",
 				"text" : "select 0 1",
 				"patching_rect" : [ 327.0, 80.0, 62.0, 20.0 ],
 				"numoutlets" : 3,
 				"fontsize" : 12.0,
 				"outlettype" : [ "bang", "bang", "" ],
 				"id" : "obj-30",
 				"fontname" : "Arial",
 				"numinlets" : 1
 			}
 
 		}
 , 		{
 			"box" : 			{
 				"maxclass" : "comment",
 				"text" : "click here to close the serial port",
 				"patching_rect" : [ 412.0, 231.0, 206.0, 20.0 ],
 				"numoutlets" : 0,
 				"fontsize" : 12.0,
 				"id" : "obj-26",
 				"fontname" : "Arial",
 				"numinlets" : 1
 			}
 
 		}
 , 		{
 			"box" : 			{
 				"maxclass" : "comment",
 				"text" : "click here to open the serial port",
 				"patching_rect" : [ 412.0, 205.0, 206.0, 20.0 ],
 				"numoutlets" : 0,
 				"fontsize" : 12.0,
 				"id" : "obj-27",
 				"fontname" : "Arial",
 				"numinlets" : 1
 			}
 
 		}
 , 		{
 			"box" : 			{
 				"maxclass" : "message",
 				"text" : "close",
 				"patching_rect" : [ 327.0, 231.0, 39.0, 18.0 ],
 				"numoutlets" : 1,
 				"fontsize" : 12.0,
 				"outlettype" : [ "" ],
 				"id" : "obj-21",
 				"fontname" : "Arial",
 				"numinlets" : 2
 			}
 
 		}
 , 		{
 			"box" : 			{
 				"maxclass" : "message",
 				"text" : "port a",
 				"patching_rect" : [ 349.0, 205.0, 41.0, 18.0 ],
 				"numoutlets" : 1,
 				"fontsize" : 12.0,
 				"outlettype" : [ "" ],
 				"id" : "obj-19",
 				"fontname" : "Arial",
 				"numinlets" : 2
 			}
 
 		}
 , 		{
 			"box" : 			{
 				"maxclass" : "multislider",
 				"candicane7" : [ 0.878431, 0.243137, 0.145098, 1.0 ],
 				"patching_rect" : [ 302.0, 450.0, 246.0, 167.0 ],
 				"contdata" : 1,
 				"numoutlets" : 2,
 				"peakcolor" : [ 0.498039, 0.498039, 0.498039, 1.0 ],
 				"slidercolor" : [ 0.066667, 0.058824, 0.776471, 1.0 ],
 				"candicane8" : [ 0.027451, 0.447059, 0.501961, 1.0 ],
 				"outlettype" : [ "", "" ],
 				"setminmax" : [ 0.0, 1023.0 ],
 				"settype" : 0,
 				"candicane6" : [ 0.733333, 0.035294, 0.788235, 1.0 ],
 				"setstyle" : 3,
 				"bgcolor" : [ 0.231373, 0.713726, 1.0, 1.0 ],
 				"id" : "obj-1",
 				"candicane4" : [ 0.439216, 0.619608, 0.070588, 1.0 ],
 				"candicane5" : [ 0.584314, 0.827451, 0.431373, 1.0 ],
 				"candicane2" : [ 0.145098, 0.203922, 0.356863, 1.0 ],
 				"candicane3" : [ 0.290196, 0.411765, 0.713726, 1.0 ],
 				"numinlets" : 1
 			}
 
 		}
 , 		{
 			"box" : 			{
 				"maxclass" : "comment",
 				"text" : "Click here to get a list of serial ports",
 				"patching_rect" : [ 412.0, 179.0, 207.0, 20.0 ],
 				"numoutlets" : 0,
 				"fontsize" : 12.0,
 				"id" : "obj-2",
 				"fontname" : "Arial",
 				"numinlets" : 1
 			}
 
 		}
 , 		{
 			"box" : 			{
 				"maxclass" : "comment",
 				"text" : "Here's the number from Arduino's analog input",
 				"linecount" : 2,
 				"patching_rect" : [ 153.0, 409.0, 138.0, 34.0 ],
 				"numoutlets" : 0,
 				"fontsize" : 12.0,
 				"id" : "obj-3",
 				"fontname" : "Arial",
 				"numinlets" : 1
 			}
 
 		}
 , 		{
 			"box" : 			{
 				"maxclass" : "comment",
 				"text" : "Convert ASCII to symbol",
 				"patching_rect" : [ 379.0, 378.0, 147.0, 20.0 ],
 				"numoutlets" : 0,
 				"fontsize" : 12.0,
 				"id" : "obj-4",
 				"fontname" : "Arial",
 				"numinlets" : 1
 			}
 
 		}
 , 		{
 			"box" : 			{
 				"maxclass" : "comment",
 				"text" : "Convert integer to ASCII",
 				"patching_rect" : [ 379.0, 355.0, 147.0, 20.0 ],
 				"numoutlets" : 0,
 				"fontsize" : 12.0,
 				"id" : "obj-5",
 				"fontname" : "Arial",
 				"numinlets" : 1
 			}
 
 		}
 , 		{
 			"box" : 			{
 				"maxclass" : "number",
 				"patching_rect" : [ 302.0, 414.0, 37.0, 20.0 ],
 				"numoutlets" : 2,
 				"fontsize" : 12.0,
 				"outlettype" : [ "int", "bang" ],
 				"bgcolor" : [ 0.866667, 0.866667, 0.866667, 1.0 ],
 				"id" : "obj-6",
 				"triscale" : 0.9,
 				"fontname" : "Arial",
 				"htextcolor" : [ 0.870588, 0.870588, 0.870588, 1.0 ],
 				"numinlets" : 1
 			}
 
 		}
 , 		{
 			"box" : 			{
 				"maxclass" : "newobj",
 				"text" : "fromsymbol",
 				"patching_rect" : [ 302.0, 378.0, 74.0, 20.0 ],
 				"numoutlets" : 1,
 				"fontsize" : 12.0,
 				"outlettype" : [ "" ],
 				"id" : "obj-7",
 				"fontname" : "Arial",
 				"color" : [ 1.0, 0.890196, 0.090196, 1.0 ],
 				"numinlets" : 1
 			}
 
 		}
 , 		{
 			"box" : 			{
 				"maxclass" : "newobj",
 				"text" : "itoa",
 				"patching_rect" : [ 302.0, 355.0, 46.0, 20.0 ],
 				"numoutlets" : 1,
 				"fontsize" : 12.0,
 				"outlettype" : [ "int" ],
 				"id" : "obj-8",
 				"fontname" : "Arial",
 				"color" : [ 1.0, 0.890196, 0.090196, 1.0 ],
 				"numinlets" : 3
 			}
 
 		}
 , 		{
 			"box" : 			{
 				"maxclass" : "newobj",
 				"text" : "zl group 4",
 				"patching_rect" : [ 302.0, 332.0, 64.0, 20.0 ],
 				"numoutlets" : 2,
 				"fontsize" : 12.0,
 				"outlettype" : [ "", "" ],
 				"id" : "obj-9",
 				"fontname" : "Arial",
 				"numinlets" : 2
 			}
 
 		}
 , 		{
 			"box" : 			{
 				"maxclass" : "newobj",
 				"text" : "select 10 13",
 				"patching_rect" : [ 244.0, 281.0, 77.0, 20.0 ],
 				"numoutlets" : 3,
 				"fontsize" : 12.0,
 				"outlettype" : [ "bang", "bang", "" ],
 				"id" : "obj-10",
 				"fontname" : "Arial",
 				"numinlets" : 1
 			}
 
 		}
 , 		{
 			"box" : 			{
 				"maxclass" : "toggle",
 				"patching_rect" : [ 244.0, 43.0, 15.0, 15.0 ],
 				"numoutlets" : 1,
 				"outlettype" : [ "int" ],
 				"id" : "obj-11",
 				"numinlets" : 1
 			}
 
 		}
 , 		{
 			"box" : 			{
 				"maxclass" : "newobj",
 				"text" : "qmetro 10",
 				"patching_rect" : [ 244.0, 80.0, 65.0, 20.0 ],
 				"numoutlets" : 1,
 				"fontsize" : 12.0,
 				"outlettype" : [ "bang" ],
 				"id" : "obj-12",
 				"fontname" : "Arial",
 				"numinlets" : 2
 			}
 
 		}
 , 		{
 			"box" : 			{
 				"maxclass" : "message",
 				"text" : "print",
 				"patching_rect" : [ 369.0, 179.0, 36.0, 18.0 ],
 				"numoutlets" : 1,
 				"fontsize" : 12.0,
 				"outlettype" : [ "" ],
 				"id" : "obj-13",
 				"fontname" : "Arial",
 				"numinlets" : 2
 			}
 
 		}
 , 		{
 			"box" : 			{
 				"maxclass" : "newobj",
 				"text" : "serial a 9600",
 				"patching_rect" : [ 244.0, 255.0, 84.0, 20.0 ],
 				"numoutlets" : 2,
 				"fontsize" : 12.0,
 				"outlettype" : [ "int", "" ],
 				"id" : "obj-14",
 				"fontname" : "Arial",
 				"numinlets" : 1
 			}
 
 		}
 , 		{
 			"box" : 			{
 				"maxclass" : "comment",
 				"text" : "Read serial input buffer every 10 milliseconds",
 				"linecount" : 2,
 				"patching_rect" : [ 53.0, 72.0, 185.0, 34.0 ],
 				"numoutlets" : 0,
 				"fontsize" : 12.0,
 				"id" : "obj-15",
 				"fontname" : "Arial",
 				"numinlets" : 1
 			}
 
 		}
 , 		{
 			"box" : 			{
 				"maxclass" : "comment",
 				"text" : "If you get newline (ASCII 10), send the list. If you get return (ASCII 13) do nothing. Any other value, add to the list",
 				"linecount" : 3,
 				"patching_rect" : [ 332.0, 269.0, 320.0, 48.0 ],
 				"numoutlets" : 0,
 				"fontsize" : 12.0,
 				"id" : "obj-16",
 				"fontname" : "Arial",
 				"numinlets" : 1
 			}
 
 		}
 , 		{
 			"box" : 			{
 				"maxclass" : "comment",
 				"text" : "Click to open/close serial port and start/stop patch",
 				"linecount" : 2,
 				"patching_rect" : [ 271.0, 32.0, 199.0, 34.0 ],
 				"numoutlets" : 0,
 				"fontsize" : 12.0,
 				"id" : "obj-17",
 				"fontname" : "Arial",
 				"numinlets" : 1
 			}
 
 		}
 ],
 	"lines" : [ 		{
 			"patchline" : 			{
 				"source" : [ "obj-6", 0 ],
 				"destination" : [ "obj-1", 0 ],
 				"hidden" : 0,
 				"midpoints" : [  ]
 			}
 
 		}
 , 		{
 			"patchline" : 			{
 				"source" : [ "obj-7", 0 ],
 				"destination" : [ "obj-6", 0 ],
 				"hidden" : 0,
 				"midpoints" : [  ]
 			}
 
 		}
 , 		{
 			"patchline" : 			{
 				"source" : [ "obj-8", 0 ],
 				"destination" : [ "obj-7", 0 ],
 				"hidden" : 0,
 				"midpoints" : [  ]
 			}
 
 		}
 , 		{
 			"patchline" : 			{
 				"source" : [ "obj-9", 0 ],
 				"destination" : [ "obj-8", 0 ],
 				"hidden" : 0,
 				"midpoints" : [  ]
 			}
 
 		}
 , 		{
 			"patchline" : 			{
 				"source" : [ "obj-10", 0 ],
 				"destination" : [ "obj-9", 0 ],
 				"hidden" : 0,
 				"midpoints" : [ 253.5, 308.0, 311.5, 308.0 ]
 			}
 
 		}
 , 		{
 			"patchline" : 			{
 				"source" : [ "obj-10", 2 ],
 				"destination" : [ "obj-9", 0 ],
 				"hidden" : 0,
 				"midpoints" : [ 311.5, 320.0, 311.5, 320.0 ]
 			}
 
 		}
 , 		{
 			"patchline" : 			{
 				"source" : [ "obj-14", 0 ],
 				"destination" : [ "obj-10", 0 ],
 				"hidden" : 0,
 				"midpoints" : [  ]
 			}
 
 		}
 , 		{
 			"patchline" : 			{
 				"source" : [ "obj-12", 0 ],
 				"destination" : [ "obj-14", 0 ],
 				"hidden" : 0,
 				"midpoints" : [  ]
 			}
 
 		}
 , 		{
 			"patchline" : 			{
 				"source" : [ "obj-11", 0 ],
 				"destination" : [ "obj-12", 0 ],
 				"hidden" : 0,
 				"midpoints" : [  ]
 			}
 
 		}
 , 		{
 			"patchline" : 			{
 				"source" : [ "obj-13", 0 ],
 				"destination" : [ "obj-14", 0 ],
 				"hidden" : 0,
 				"midpoints" : [ 378.5, 200.5, 253.5, 200.5 ]
 			}
 
 		}
 , 		{
 			"patchline" : 			{
 				"source" : [ "obj-19", 0 ],
 				"destination" : [ "obj-14", 0 ],
 				"hidden" : 0,
 				"midpoints" : [ 358.5, 228.5, 253.5, 228.5 ]
 			}
 
 		}
 , 		{
 			"patchline" : 			{
 				"source" : [ "obj-21", 0 ],
 				"destination" : [ "obj-14", 0 ],
 				"hidden" : 0,
 				"midpoints" : [ 336.5, 251.5, 253.5, 251.5 ]
 			}
 
 		}
 , 		{
 			"patchline" : 			{
 				"source" : [ "obj-30", 0 ],
 				"destination" : [ "obj-21", 0 ],
 				"hidden" : 0,
 				"midpoints" : [  ]
 			}
 
 		}
 , 		{
 			"patchline" : 			{
 				"source" : [ "obj-30", 1 ],
 				"destination" : [ "obj-19", 0 ],
 				"hidden" : 0,
 				"midpoints" : [  ]
 			}
 
 		}
 , 		{
 			"patchline" : 			{
 				"source" : [ "obj-11", 0 ],
 				"destination" : [ "obj-30", 0 ],
 				"hidden" : 0,
 				"midpoints" : [ 253.0, 71.0, 336.5, 71.0 ]
 			}
 
 		}
 ]
 }
 
 */
