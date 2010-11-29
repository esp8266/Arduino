/*
  This example reads three analog sensors (potentiometers are easiest)
 and sends their values serially. The Processing and Max/MSP programs at the bottom
 take those three values and use them to change the background color of the screen.
 
 The circuit:
 * potentiometers attached to analog inputs 0, 1, and 2
 
 http://www.arduino.cc/en/Tutorial/VirtualColorMixer
 
 created 2 Dec 2006
 by David A. Mellis
 modified 4 Sep 2010
 by Tom Igoe and Scott Fitzgerald
 
  This example code is in the public domain.
 */

const int redPin = A0;		// sensor to control red color
const int greenPin = A1;	// sensor to control green color
const int bluePin = A2;		// sensor to control blue color

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  Serial.print(analogRead(redPin));
  Serial.print(",");
  Serial.print(analogRead(greenPin));
  Serial.print(",");
  Serial.println(analogRead(bluePin));
}

/* Processing code for this example
 
//  This example code is in the public domain.
 
 import processing.serial.*;
 
 float redValue = 0;        // red value
 float greenValue = 0;      // green value
 float blueValue = 0;       // blue value
 
 Serial myPort;
 
 void setup() {
 size(200, 200);
 
 // List all the available serial ports
 println(Serial.list());
 // I know that the first port in the serial list on my mac
 // is always my  Arduino, so I open Serial.list()[0].
 // Open whatever port is the one you're using.
 myPort = new Serial(this, Serial.list()[0], 9600);
 // don't generate a serialEvent() unless you get a newline character:
 myPort.bufferUntil('\n');
 }
 
 void draw() {
 // set the background color with the color values:
 background(redValue, greenValue, blueValue);
 }
 
 void serialEvent(Serial myPort) { 
 // get the ASCII string:
 String inString = myPort.readStringUntil('\n');
 
 if (inString != null) {
 // trim off any whitespace:
 inString = trim(inString);
 // split the string on the commas and convert the 
 // resulting substrings into an integer array:
 float[] colors = float(split(inString, ","));
 // if the array has at least three elements, you know
 // you got the whole thing.  Put the numbers in the
 // color variables:
 if (colors.length >=3) {
 // map them to the range 0-255:
 redValue = map(colors[0], 0, 1023, 0, 255);
 greenValue = map(colors[1], 0, 1023, 0, 255);
 blueValue = map(colors[2], 0, 1023, 0, 255);
 }
 }
 }
 */

/* Max/MSP patch for this example
 {
 	"boxes" : [ 		{
 			"box" : 			{
 				"maxclass" : "newobj",
 				"text" : "/ 4",
 				"patching_rect" : [ 448.0, 502.0, 32.5, 20.0 ],
 				"numoutlets" : 1,
 				"fontsize" : 12.0,
 				"outlettype" : [ "int" ],
 				"id" : "obj-25",
 				"fontname" : "Arial",
 				"numinlets" : 2
 			}
 
 		}
 , 		{
 			"box" : 			{
 				"maxclass" : "newobj",
 				"text" : "/ 4",
 				"patching_rect" : [ 398.0, 502.0, 32.5, 20.0 ],
 				"numoutlets" : 1,
 				"fontsize" : 12.0,
 				"outlettype" : [ "int" ],
 				"id" : "obj-24",
 				"fontname" : "Arial",
 				"numinlets" : 2
 			}
 
 		}
 , 		{
 			"box" : 			{
 				"maxclass" : "newobj",
 				"text" : "/ 4",
 				"patching_rect" : [ 348.0, 502.0, 32.5, 20.0 ],
 				"numoutlets" : 1,
 				"fontsize" : 12.0,
 				"outlettype" : [ "int" ],
 				"id" : "obj-23",
 				"fontname" : "Arial",
 				"numinlets" : 2
 			}
 
 		}
 , 		{
 			"box" : 			{
 				"maxclass" : "comment",
 				"text" : "Virtual color mixer\n\nThis patch takes a string, containing three comma-separated ASCII formatted numbers from 0 to 1023, with a carriage return and linefeed at the end.  It converts the string to three integers and uses them to set the background color.\n\n created 2 Dec 2006\n by David A. Mellis\nmodified 14 Apr 2009\nby Scott Fitzgerald and Tom Igoe",
 				"linecount" : 11,
 				"patching_rect" : [ 524.0, 51.0, 398.0, 158.0 ],
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
 				"patching_rect" : [ 372.0, 125.0, 62.0, 20.0 ],
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
 				"patching_rect" : [ 457.0, 276.0, 206.0, 20.0 ],
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
 				"patching_rect" : [ 457.0, 250.0, 206.0, 20.0 ],
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
 				"patching_rect" : [ 372.0, 276.0, 39.0, 18.0 ],
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
 				"patching_rect" : [ 394.0, 250.0, 41.0, 18.0 ],
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
 				"maxclass" : "comment",
 				"text" : "Click here to get a list of serial ports",
 				"patching_rect" : [ 457.0, 224.0, 207.0, 20.0 ],
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
 				"text" : "Convert ASCII to symbol",
 				"patching_rect" : [ 424.0, 423.0, 147.0, 20.0 ],
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
 				"patching_rect" : [ 424.0, 400.0, 147.0, 20.0 ],
 				"numoutlets" : 0,
 				"fontsize" : 12.0,
 				"id" : "obj-5",
 				"fontname" : "Arial",
 				"numinlets" : 1
 			}
 
 		}
 , 		{
 			"box" : 			{
 				"maxclass" : "newobj",
 				"text" : "fromsymbol",
 				"patching_rect" : [ 347.0, 423.0, 74.0, 20.0 ],
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
 				"patching_rect" : [ 347.0, 400.0, 46.0, 20.0 ],
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
 				"text" : "zl group",
 				"patching_rect" : [ 347.0, 377.0, 53.0, 20.0 ],
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
 				"patching_rect" : [ 289.0, 326.0, 77.0, 20.0 ],
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
 				"patching_rect" : [ 289.0, 88.0, 15.0, 15.0 ],
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
 				"patching_rect" : [ 289.0, 125.0, 65.0, 20.0 ],
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
 				"patching_rect" : [ 414.0, 224.0, 36.0, 18.0 ],
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
 				"patching_rect" : [ 289.0, 300.0, 84.0, 20.0 ],
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
 				"patching_rect" : [ 98.0, 117.0, 185.0, 34.0 ],
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
 				"patching_rect" : [ 377.0, 314.0, 320.0, 48.0 ],
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
 				"patching_rect" : [ 316.0, 77.0, 199.0, 34.0 ],
 				"numoutlets" : 0,
 				"fontsize" : 12.0,
 				"id" : "obj-17",
 				"fontname" : "Arial",
 				"numinlets" : 1
 			}
 
 		}
 , 		{
 			"box" : 			{
 				"maxclass" : "newobj",
 				"text" : "bgcolor 0 0 0",
 				"patching_rect" : [ 348.0, 585.0, 169.0, 19.0 ],
 				"numoutlets" : 0,
 				"fontsize" : 10.0,
 				"id" : "obj-6",
 				"fontname" : "Verdana",
 				"numinlets" : 4
 			}
 
 		}
 , 		{
 			"box" : 			{
 				"maxclass" : "newobj",
 				"text" : "unpack 0 0 0 0 0",
 				"patching_rect" : [ 347.0, 470.0, 119.0, 19.0 ],
 				"numoutlets" : 5,
 				"fontsize" : 10.0,
 				"outlettype" : [ "int", "int", "int", "int", "int" ],
 				"id" : "obj-20",
 				"fontname" : "Verdana",
 				"numinlets" : 1
 			}
 
 		}
 , 		{
 			"box" : 			{
 				"maxclass" : "number",
 				"patching_rect" : [ 448.0, 535.0, 50.0, 19.0 ],
 				"numoutlets" : 2,
 				"fontsize" : 10.0,
 				"outlettype" : [ "int", "bang" ],
 				"id" : "obj-18",
 				"fontname" : "Verdana",
 				"numinlets" : 1
 			}
 
 		}
 , 		{
 			"box" : 			{
 				"maxclass" : "number",
 				"patching_rect" : [ 398.0, 535.0, 50.0, 19.0 ],
 				"numoutlets" : 2,
 				"fontsize" : 10.0,
 				"outlettype" : [ "int", "bang" ],
 				"id" : "obj-1",
 				"fontname" : "Verdana",
 				"numinlets" : 1
 			}
 
 		}
 , 		{
 			"box" : 			{
 				"maxclass" : "number",
 				"patching_rect" : [ 348.0, 535.0, 50.0, 19.0 ],
 				"numoutlets" : 2,
 				"fontsize" : 10.0,
 				"outlettype" : [ "int", "bang" ],
 				"id" : "obj-22",
 				"fontname" : "Verdana",
 				"numinlets" : 1
 			}
 
 		}
 , 		{
 			"box" : 			{
 				"maxclass" : "comment",
 				"text" : "Here's the numbers from Arduino's analog input",
 				"linecount" : 3,
 				"patching_rect" : [ 198.0, 484.0, 138.0, 48.0 ],
 				"numoutlets" : 0,
 				"fontsize" : 12.0,
 				"id" : "obj-3",
 				"fontname" : "Arial",
 				"numinlets" : 1
 			}
 
 		}
 ],
 	"lines" : [ 		{
 			"patchline" : 			{
 				"source" : [ "obj-18", 0 ],
 				"destination" : [ "obj-6", 2 ],
 				"hidden" : 0,
 				"midpoints" : [  ]
 			}
 
 		}
 , 		{
 			"patchline" : 			{
 				"source" : [ "obj-1", 0 ],
 				"destination" : [ "obj-6", 1 ],
 				"hidden" : 0,
 				"midpoints" : [  ]
 			}
 
 		}
 , 		{
 			"patchline" : 			{
 				"source" : [ "obj-22", 0 ],
 				"destination" : [ "obj-6", 0 ],
 				"hidden" : 0,
 				"midpoints" : [  ]
 			}
 
 		}
 , 		{
 			"patchline" : 			{
 				"source" : [ "obj-25", 0 ],
 				"destination" : [ "obj-18", 0 ],
 				"hidden" : 0,
 				"midpoints" : [  ]
 			}
 
 		}
 , 		{
 			"patchline" : 			{
 				"source" : [ "obj-20", 4 ],
 				"destination" : [ "obj-25", 0 ],
 				"hidden" : 0,
 				"midpoints" : [  ]
 			}
 
 		}
 , 		{
 			"patchline" : 			{
 				"source" : [ "obj-20", 2 ],
 				"destination" : [ "obj-24", 0 ],
 				"hidden" : 0,
 				"midpoints" : [  ]
 			}
 
 		}
 , 		{
 			"patchline" : 			{
 				"source" : [ "obj-24", 0 ],
 				"destination" : [ "obj-1", 0 ],
 				"hidden" : 0,
 				"midpoints" : [  ]
 			}
 
 		}
 , 		{
 			"patchline" : 			{
 				"source" : [ "obj-23", 0 ],
 				"destination" : [ "obj-22", 0 ],
 				"hidden" : 0,
 				"midpoints" : [  ]
 			}
 
 		}
 , 		{
 			"patchline" : 			{
 				"source" : [ "obj-20", 0 ],
 				"destination" : [ "obj-23", 0 ],
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
 				"midpoints" : [ 423.5, 245.5, 298.5, 245.5 ]
 			}
 
 		}
 , 		{
 			"patchline" : 			{
 				"source" : [ "obj-19", 0 ],
 				"destination" : [ "obj-14", 0 ],
 				"hidden" : 0,
 				"midpoints" : [ 403.5, 273.5, 298.5, 273.5 ]
 			}
 
 		}
 , 		{
 			"patchline" : 			{
 				"source" : [ "obj-21", 0 ],
 				"destination" : [ "obj-14", 0 ],
 				"hidden" : 0,
 				"midpoints" : [ 381.5, 296.5, 298.5, 296.5 ]
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
 				"midpoints" : [ 298.0, 116.0, 381.5, 116.0 ]
 			}
 
 		}
 , 		{
 			"patchline" : 			{
 				"source" : [ "obj-7", 0 ],
 				"destination" : [ "obj-20", 0 ],
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
 				"midpoints" : [ 298.5, 353.0, 356.5, 353.0 ]
 			}
 
 		}
 , 		{
 			"patchline" : 			{
 				"source" : [ "obj-10", 2 ],
 				"destination" : [ "obj-9", 0 ],
 				"hidden" : 0,
 				"midpoints" : [ 356.5, 365.0, 356.5, 365.0 ]
 			}
 
 		}
 ]
 }
 
 */
