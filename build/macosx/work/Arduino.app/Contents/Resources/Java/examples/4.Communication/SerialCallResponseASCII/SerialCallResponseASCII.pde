/*
  Serial Call and Response in ASCII
 Language: Wiring/Arduino
 
 This program sends an ASCII A (byte of value 65) on startup
 and repeats that until it gets some data in.
 Then it waits for a byte in the serial port, and 
 sends three ASCII-encoded, comma-separated sensor values, 
 truncated by a linefeed and carriage return, 
 whenever it gets a byte in.
 
 Thanks to Greg Shakar and Scott Fitzgerald for the improvements
 
  The circuit:
 * potentiometers attached to analog inputs 0 and 1 
 * pushbutton attached to digital I/O 2
 
 
 
 Created 26 Sept. 2005
 by Tom Igoe
 Modified 4 Sep 2010
 by Tom Igoe and Scott Fitzgerald
 
 This example code is in the public domain.
 
 http://www.arduino.cc/en/Tutorial/SerialCallResponseASCII
 
 */

int firstSensor = 0;    // first analog sensor
int secondSensor = 0;   // second analog sensor
int thirdSensor = 0;    // digital sensor
int inByte = 0;         // incoming serial byte

void setup()
{
  // start serial port at 9600 bps:
  Serial.begin(9600);
  pinMode(2, INPUT);   // digital sensor is on digital pin 2
  establishContact();  // send a byte to establish contact until receiver responds 
}

void loop()
{
  // if we get a valid byte, read analog ins:
  if (Serial.available() > 0) {
    // get incoming byte:
    inByte = Serial.read();
    // read first analog input, divide by 4 to make the range 0-255:
    firstSensor = analogRead(A0)/4;
    // delay 10ms to let the ADC recover:
    delay(10);
    // read second analog input, divide by 4 to make the range 0-255:
    secondSensor = analogRead(1)/4;
    // read  switch, map it to 0 or 255L
    thirdSensor = map(digitalRead(2), 0, 1, 0, 255);  
    // send sensor values:
    Serial.print(firstSensor, DEC);
    Serial.print(",");
    Serial.print(secondSensor, DEC);
    Serial.print(",");
    Serial.println(thirdSensor, DEC);               
  }
}

void establishContact() {
  while (Serial.available() <= 0) {
    Serial.println("0,0,0");   // send an initial string
    delay(300);
  }
}


/*
Processing code to run with this example:

// This example code is in the public domain.

import processing.serial.*;     // import the Processing serial library
Serial myPort;                  // The serial port

float bgcolor;			// Background color
float fgcolor;			// Fill color
float xpos, ypos;	        // Starting position of the ball

void setup() {
  size(640,480);

  // List all the available serial ports
  println(Serial.list());

  // I know that the first port in the serial list on my mac
  // is always my  Arduino module, so I open Serial.list()[0].
  // Change the 0 to the appropriate number of the serial port
  // that your microcontroller is attached to.
  myPort = new Serial(this, Serial.list()[0], 9600);

  // read bytes into a buffer until you get a linefeed (ASCII 10):
  myPort.bufferUntil('\n');
  
  // draw with smooth edges:
  smooth();
}

void draw() {
  background(bgcolor);
  fill(fgcolor);
  // Draw the shape
  ellipse(xpos, ypos, 20, 20);
}

// serialEvent  method is run automatically by the Processing applet
// whenever the buffer reaches the  byte value set in the bufferUntil() 
// method in the setup():

void serialEvent(Serial myPort) { 
  // read the serial buffer:
  String myString = myPort.readStringUntil('\n');
  // if you got any bytes other than the linefeed:
    myString = trim(myString);
 
    // split the string at the commas
    // and convert the sections into integers:
    int sensors[] = int(split(myString, ','));

    // print out the values you got:
    for (int sensorNum = 0; sensorNum < sensors.length; sensorNum++) {
      print("Sensor " + sensorNum + ": " + sensors[sensorNum] + "\t"); 
    }
    // add a linefeed after all the sensor values are printed:
    println();
    if (sensors.length > 1) {
      xpos = map(sensors[0], 0,1023,0,width);
      ypos = map(sensors[1], 0,1023,0,height);
      fgcolor = sensors[2];
    }
    // send a byte to ask for more data:
    myPort.write("A");
  }

*/

/*
{
	"boxes" : [ 		{
			"box" : 			{
				"maxclass" : "newobj",
				"text" : "fromsymbol",
				"patching_rect" : [ 265.0, 585.0, 74.0, 20.0 ],
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
				"patching_rect" : [ 265.0, 562.0, 46.0, 20.0 ],
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
				"patching_rect" : [ 265.0, 539.0, 53.0, 20.0 ],
				"numoutlets" : 2,
				"fontsize" : 12.0,
				"outlettype" : [ "", "" ],
				"id" : "obj-4",
				"fontname" : "Arial",
				"numinlets" : 2
			}

		}
, 		{
			"box" : 			{
				"maxclass" : "newobj",
				"text" : "select 10 13",
				"patching_rect" : [ 209.0, 501.0, 75.0, 20.0 ],
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
				"maxclass" : "message",
				"text" : "65",
				"patching_rect" : [ 354.0, 481.0, 32.5, 18.0 ],
				"numoutlets" : 1,
				"fontsize" : 12.0,
				"outlettype" : [ "" ],
				"id" : "obj-9",
				"fontname" : "Arial",
				"numinlets" : 2
			}

		}
, 		{
			"box" : 			{
				"maxclass" : "newobj",
				"text" : "sel 1",
				"patching_rect" : [ 354.0, 452.0, 36.0, 20.0 ],
				"numoutlets" : 2,
				"fontsize" : 12.0,
				"outlettype" : [ "bang", "" ],
				"id" : "obj-6",
				"fontname" : "Arial",
				"numinlets" : 2
			}

		}
, 		{
			"box" : 			{
				"maxclass" : "comment",
				"text" : "Serial Call-Response ASCII \n\nSends a byte out the serial port, and reads 3 ASCII enoded, comma separated  in, truncated by a linefeed. It then sets foregound color, xpos, and ypos of a circle using the values returned from the serial port.  \n\nNote: This patch assumes that the device on the other end of the serial port is going to send a single byte of value 65 (ASCII A) on startup. The sketch waits for that byte, then sends an ASCII A whenever it wants more data. \n\ncreated 14 Apr 2009\nby Scott Fitzgerald and Tom Igoe",
				"linecount" : 12,
				"patching_rect" : [ 401.0, 67.0, 540.0, 172.0 ],
				"numoutlets" : 0,
				"fontsize" : 12.0,
				"id" : "obj-5",
				"fontname" : "Arial",
				"numinlets" : 1
			}

		}
, 		{
			"box" : 			{
				"maxclass" : "comment",
				"text" : "trigger (or [t]) forces right-left conventions.  All the drawing and processing will happen before Max requests new values. When this trigger fires, it sends an ASCII A to ask Arduino for new values.",
				"linecount" : 3,
				"patching_rect" : [ 254.0, 625.0, 425.0, 48.0 ],
				"numoutlets" : 0,
				"fontsize" : 12.0,
				"id" : "obj-65",
				"fontname" : "Arial",
				"numinlets" : 1
			}

		}
, 		{
			"box" : 			{
				"maxclass" : "comment",
				"text" : "reinitializes the gates when turned on and off",
				"linecount" : 2,
				"patching_rect" : [ 185.0, 385.0, 135.0, 34.0 ],
				"numoutlets" : 0,
				"fontsize" : 12.0,
				"id" : "obj-64",
				"fontname" : "Arial",
				"numinlets" : 1
			}

		}
, 		{
			"box" : 			{
				"maxclass" : "comment",
				"text" : "checks for the ascii value of newline to begin communication.  After initial communication is made, this block shuts down.",
				"linecount" : 3,
				"patching_rect" : [ 475.0, 370.0, 252.0, 48.0 ],
				"numoutlets" : 0,
				"fontsize" : 12.0,
				"id" : "obj-63",
				"fontname" : "Arial",
				"numinlets" : 1
			}

		}
, 		{
			"box" : 			{
				"maxclass" : "newobj",
				"text" : "p \"draw the circle\"",
				"patching_rect" : [ 232.0, 765.0, 269.0, 19.0 ],
				"numoutlets" : 1,
				"fontsize" : 10.0,
				"outlettype" : [ "" ],
				"id" : "obj-62",
				"fontname" : "Verdana",
				"numinlets" : 3,
				"patcher" : 				{
					"fileversion" : 1,
					"rect" : [ 54.0, 94.0, 640.0, 480.0 ],
					"bglocked" : 0,
					"defrect" : [ 54.0, 94.0, 640.0, 480.0 ],
					"openrect" : [ 0.0, 0.0, 0.0, 0.0 ],
					"openinpresentation" : 0,
					"default_fontsize" : 10.0,
					"default_fontface" : 0,
					"default_fontname" : "Verdana",
					"gridonopen" : 0,
					"gridsize" : [ 25.0, 25.0 ],
					"gridsnaponopen" : 0,
					"toolbarvisible" : 1,
					"boxanimatetime" : 200,
					"imprint" : 0,
					"boxes" : [ 						{
							"box" : 							{
								"maxclass" : "message",
								"text" : "frgb 255 255 255",
								"patching_rect" : [ 375.0, 150.0, 98.0, 18.0 ],
								"numoutlets" : 1,
								"fontsize" : 11.595187,
								"outlettype" : [ "" ],
								"id" : "obj-47",
								"fontname" : "Arial",
								"numinlets" : 2
							}

						}
, 						{
							"box" : 							{
								"maxclass" : "message",
								"text" : "frgb 0 0 0",
								"patching_rect" : [ 275.0, 125.0, 59.0, 18.0 ],
								"numoutlets" : 1,
								"fontsize" : 11.595187,
								"outlettype" : [ "" ],
								"id" : "obj-46",
								"fontname" : "Arial",
								"numinlets" : 2
							}

						}
, 						{
							"box" : 							{
								"maxclass" : "newobj",
								"text" : "sel 255 0",
								"patching_rect" : [ 300.0, 100.0, 66.0, 21.0 ],
								"numoutlets" : 3,
								"fontsize" : 12.0,
								"outlettype" : [ "bang", "bang", "" ],
								"id" : "obj-45",
								"fontname" : "Verdana",
								"numinlets" : 1
							}

						}
, 						{
							"box" : 							{
								"maxclass" : "newobj",
								"text" : "pack 0 0 0 0",
								"patching_rect" : [ 50.0, 125.0, 180.0, 21.0 ],
								"numoutlets" : 1,
								"fontsize" : 12.0,
								"outlettype" : [ "" ],
								"id" : "obj-43",
								"fontname" : "Verdana",
								"numinlets" : 4
							}

						}
, 						{
							"box" : 							{
								"maxclass" : "newobj",
								"text" : "+ 10",
								"patching_rect" : [ 200.0, 100.0, 40.0, 21.0 ],
								"numoutlets" : 1,
								"fontsize" : 12.0,
								"outlettype" : [ "int" ],
								"id" : "obj-42",
								"fontname" : "Verdana",
								"numinlets" : 2
							}

						}
, 						{
							"box" : 							{
								"maxclass" : "newobj",
								"text" : "+ 10",
								"patching_rect" : [ 75.0, 100.0, 40.0, 21.0 ],
								"numoutlets" : 1,
								"fontsize" : 12.0,
								"outlettype" : [ "int" ],
								"id" : "obj-41",
								"fontname" : "Verdana",
								"numinlets" : 2
							}

						}
, 						{
							"box" : 							{
								"maxclass" : "message",
								"text" : "clear, paintoval $1 $2 $3 $4",
								"patching_rect" : [ 50.0, 150.0, 152.0, 18.0 ],
								"numoutlets" : 1,
								"fontsize" : 11.595187,
								"outlettype" : [ "" ],
								"id" : "obj-40",
								"fontname" : "Arial",
								"numinlets" : 2
							}

						}
, 						{
							"box" : 							{
								"maxclass" : "inlet",
								"patching_rect" : [ 57.5, 40.0, 25.0, 25.0 ],
								"numoutlets" : 1,
								"outlettype" : [ "int" ],
								"id" : "obj-58",
								"numinlets" : 0,
								"comment" : ""
							}

						}
, 						{
							"box" : 							{
								"maxclass" : "inlet",
								"patching_rect" : [ 120.0, 40.0, 25.0, 25.0 ],
								"numoutlets" : 1,
								"outlettype" : [ "int" ],
								"id" : "obj-59",
								"numinlets" : 0,
								"comment" : ""
							}

						}
, 						{
							"box" : 							{
								"maxclass" : "inlet",
								"patching_rect" : [ 300.0, 40.0, 25.0, 25.0 ],
								"numoutlets" : 1,
								"outlettype" : [ "int" ],
								"id" : "obj-60",
								"numinlets" : 0,
								"comment" : ""
							}

						}
, 						{
							"box" : 							{
								"maxclass" : "outlet",
								"patching_rect" : [ 228.333344, 228.0, 25.0, 25.0 ],
								"numoutlets" : 0,
								"id" : "obj-61",
								"numinlets" : 1,
								"comment" : ""
							}

						}
 ],
					"lines" : [ 						{
							"patchline" : 							{
								"source" : [ "obj-43", 0 ],
								"destination" : [ "obj-40", 0 ],
								"hidden" : 0,
								"midpoints" : [  ]
							}

						}
, 						{
							"patchline" : 							{
								"source" : [ "obj-45", 0 ],
								"destination" : [ "obj-46", 0 ],
								"hidden" : 0,
								"midpoints" : [  ]
							}

						}
, 						{
							"patchline" : 							{
								"source" : [ "obj-45", 1 ],
								"destination" : [ "obj-47", 0 ],
								"hidden" : 0,
								"midpoints" : [  ]
							}

						}
, 						{
							"patchline" : 							{
								"source" : [ "obj-41", 0 ],
								"destination" : [ "obj-43", 2 ],
								"hidden" : 0,
								"midpoints" : [  ]
							}

						}
, 						{
							"patchline" : 							{
								"source" : [ "obj-42", 0 ],
								"destination" : [ "obj-43", 3 ],
								"hidden" : 0,
								"midpoints" : [  ]
							}

						}
, 						{
							"patchline" : 							{
								"source" : [ "obj-58", 0 ],
								"destination" : [ "obj-43", 0 ],
								"hidden" : 0,
								"midpoints" : [  ]
							}

						}
, 						{
							"patchline" : 							{
								"source" : [ "obj-58", 0 ],
								"destination" : [ "obj-41", 0 ],
								"hidden" : 0,
								"midpoints" : [  ]
							}

						}
, 						{
							"patchline" : 							{
								"source" : [ "obj-59", 0 ],
								"destination" : [ "obj-43", 1 ],
								"hidden" : 0,
								"midpoints" : [  ]
							}

						}
, 						{
							"patchline" : 							{
								"source" : [ "obj-59", 0 ],
								"destination" : [ "obj-42", 0 ],
								"hidden" : 0,
								"midpoints" : [  ]
							}

						}
, 						{
							"patchline" : 							{
								"source" : [ "obj-60", 0 ],
								"destination" : [ "obj-45", 0 ],
								"hidden" : 0,
								"midpoints" : [  ]
							}

						}
, 						{
							"patchline" : 							{
								"source" : [ "obj-40", 0 ],
								"destination" : [ "obj-61", 0 ],
								"hidden" : 0,
								"midpoints" : [  ]
							}

						}
, 						{
							"patchline" : 							{
								"source" : [ "obj-46", 0 ],
								"destination" : [ "obj-61", 0 ],
								"hidden" : 0,
								"midpoints" : [  ]
							}

						}
, 						{
							"patchline" : 							{
								"source" : [ "obj-47", 0 ],
								"destination" : [ "obj-61", 0 ],
								"hidden" : 0,
								"midpoints" : [  ]
							}

						}
 ]
				}
,
				"saved_object_attributes" : 				{
					"fontface" : 0,
					"fontsize" : 10.0,
					"default_fontface" : 0,
					"default_fontname" : "Verdana",
					"default_fontsize" : 10.0,
					"fontname" : "Verdana",
					"globalpatchername" : ""
				}

			}

		}
, 		{
			"box" : 			{
				"maxclass" : "newobj",
				"text" : "0",
				"patching_rect" : [ 325.0, 393.0, 32.5, 20.0 ],
				"numoutlets" : 1,
				"fontsize" : 12.0,
				"outlettype" : [ "int" ],
				"id" : "obj-57",
				"fontname" : "Arial",
				"numinlets" : 2
			}

		}
, 		{
			"box" : 			{
				"maxclass" : "newobj",
				"text" : "!- 1",
				"patching_rect" : [ 400.0, 451.0, 32.5, 20.0 ],
				"numoutlets" : 1,
				"fontsize" : 12.0,
				"outlettype" : [ "int" ],
				"id" : "obj-55",
				"fontname" : "Arial",
				"numinlets" : 2
			}

		}
, 		{
			"box" : 			{
				"maxclass" : "newobj",
				"text" : "gate 1 1",
				"patching_rect" : [ 400.0, 370.0, 54.0, 20.0 ],
				"numoutlets" : 1,
				"fontsize" : 12.0,
				"outlettype" : [ "" ],
				"id" : "obj-54",
				"fontname" : "Arial",
				"numinlets" : 2
			}

		}
, 		{
			"box" : 			{
				"maxclass" : "toggle",
				"patching_rect" : [ 400.0, 420.0, 20.0, 20.0 ],
				"numoutlets" : 1,
				"outlettype" : [ "int" ],
				"id" : "obj-53",
				"numinlets" : 1
			}

		}
, 		{
			"box" : 			{
				"maxclass" : "newobj",
				"text" : "gate 1 0",
				"patching_rect" : [ 209.0, 470.0, 54.0, 20.0 ],
				"numoutlets" : 1,
				"fontsize" : 12.0,
				"outlettype" : [ "" ],
				"id" : "obj-50",
				"fontname" : "Arial",
				"numinlets" : 2
			}

		}
, 		{
			"box" : 			{
				"maxclass" : "newobj",
				"text" : "sel 10",
				"patching_rect" : [ 400.0, 393.0, 43.0, 20.0 ],
				"numoutlets" : 2,
				"fontsize" : 12.0,
				"outlettype" : [ "bang", "" ],
				"id" : "obj-48",
				"fontname" : "Arial",
				"numinlets" : 2
			}

		}
, 		{
			"box" : 			{
				"maxclass" : "lcd",
				"patching_rect" : [ 232.0, 815.0, 256.0, 256.0 ],
				"numoutlets" : 4,
				"outlettype" : [ "list", "list", "int", "" ],
				"id" : "obj-39",
				"numinlets" : 1
			}

		}
, 		{
			"box" : 			{
				"maxclass" : "newobj",
				"text" : "t 65 l",
				"patching_rect" : [ 209.0, 624.0, 42.0, 21.0 ],
				"numoutlets" : 2,
				"fontsize" : 12.0,
				"outlettype" : [ "int", "" ],
				"id" : "obj-35",
				"fontname" : "Verdana",
				"numinlets" : 1
			}

		}
, 		{
			"box" : 			{
				"maxclass" : "comment",
				"text" : "val3",
				"patching_rect" : [ 553.0, 725.0, 37.0, 21.0 ],
				"numoutlets" : 0,
				"fontsize" : 12.0,
				"id" : "obj-1",
				"fontname" : "Verdana",
				"numinlets" : 1
			}

		}
, 		{
			"box" : 			{
				"maxclass" : "number",
				"patching_rect" : [ 482.0, 725.0, 56.0, 21.0 ],
				"numoutlets" : 2,
				"fontsize" : 12.0,
				"outlettype" : [ "int", "bang" ],
				"id" : "obj-3",
				"fontname" : "Verdana",
				"numinlets" : 1
			}

		}
, 		{
			"box" : 			{
				"maxclass" : "comment",
				"text" : "val2",
				"patching_rect" : [ 425.0, 725.0, 37.0, 21.0 ],
				"numoutlets" : 0,
				"fontsize" : 12.0,
				"id" : "obj-18",
				"fontname" : "Verdana",
				"numinlets" : 1
			}

		}
, 		{
			"box" : 			{
				"maxclass" : "comment",
				"text" : "val1",
				"patching_rect" : [ 297.0, 725.0, 37.0, 21.0 ],
				"numoutlets" : 0,
				"fontsize" : 12.0,
				"id" : "obj-20",
				"fontname" : "Verdana",
				"numinlets" : 1
			}

		}
, 		{
			"box" : 			{
				"maxclass" : "number",
				"patching_rect" : [ 357.0, 725.0, 56.0, 21.0 ],
				"numoutlets" : 2,
				"fontsize" : 12.0,
				"outlettype" : [ "int", "bang" ],
				"id" : "obj-22",
				"fontname" : "Verdana",
				"numinlets" : 1
			}

		}
, 		{
			"box" : 			{
				"maxclass" : "number",
				"patching_rect" : [ 232.0, 725.0, 55.0, 21.0 ],
				"numoutlets" : 2,
				"fontsize" : 12.0,
				"outlettype" : [ "int", "bang" ],
				"id" : "obj-23",
				"fontname" : "Verdana",
				"numinlets" : 1
			}

		}
, 		{
			"box" : 			{
				"maxclass" : "newobj",
				"text" : "unpack 0 0 0 0 0",
				"patching_rect" : [ 232.0, 690.0, 269.0, 21.0 ],
				"numoutlets" : 5,
				"fontsize" : 12.0,
				"outlettype" : [ "int", "int", "int", "int", "int" ],
				"id" : "obj-29",
				"fontname" : "Verdana",
				"numinlets" : 1
			}

		}
, 		{
			"box" : 			{
				"maxclass" : "newobj",
				"text" : "select 0 1",
				"patching_rect" : [ 327.0, 215.0, 62.0, 20.0 ],
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
				"patching_rect" : [ 375.0, 306.0, 206.0, 20.0 ],
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
				"patching_rect" : [ 400.0, 280.0, 206.0, 20.0 ],
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
				"patching_rect" : [ 327.0, 306.0, 39.0, 18.0 ],
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
				"patching_rect" : [ 349.0, 280.0, 41.0, 18.0 ],
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
				"patching_rect" : [ 420.0, 254.0, 207.0, 20.0 ],
				"numoutlets" : 0,
				"fontsize" : 12.0,
				"id" : "obj-2",
				"fontname" : "Arial",
				"numinlets" : 1
			}

		}
, 		{
			"box" : 			{
				"maxclass" : "toggle",
				"patching_rect" : [ 244.0, 170.0, 22.0, 22.0 ],
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
				"patching_rect" : [ 244.0, 215.0, 65.0, 20.0 ],
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
				"patching_rect" : [ 369.0, 254.0, 36.0, 18.0 ],
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
				"patching_rect" : [ 244.0, 330.0, 84.0, 20.0 ],
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
				"patching_rect" : [ 28.0, 207.0, 210.0, 34.0 ],
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
				"text" : "Click to start",
				"patching_rect" : [ 271.0, 178.0, 117.0, 20.0 ],
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
				"source" : [ "obj-54", 0 ],
				"destination" : [ "obj-48", 0 ],
				"hidden" : 0,
				"midpoints" : [  ]
			}

		}
, 		{
			"patchline" : 			{
				"source" : [ "obj-48", 0 ],
				"destination" : [ "obj-53", 0 ],
				"hidden" : 0,
				"midpoints" : [  ]
			}

		}
, 		{
			"patchline" : 			{
				"source" : [ "obj-9", 0 ],
				"destination" : [ "obj-14", 0 ],
				"hidden" : 0,
				"midpoints" : [  ]
			}

		}
, 		{
			"patchline" : 			{
				"source" : [ "obj-53", 0 ],
				"destination" : [ "obj-6", 0 ],
				"hidden" : 0,
				"midpoints" : [ 409.5, 446.5, 363.5, 446.5 ]
			}

		}
, 		{
			"patchline" : 			{
				"source" : [ "obj-6", 0 ],
				"destination" : [ "obj-9", 0 ],
				"hidden" : 0,
				"midpoints" : [  ]
			}

		}
, 		{
			"patchline" : 			{
				"source" : [ "obj-35", 0 ],
				"destination" : [ "obj-14", 0 ],
				"hidden" : 0,
				"midpoints" : [ 218.5, 656.0, 182.0, 656.0, 182.0, 315.0, 253.5, 315.0 ]
			}

		}
, 		{
			"patchline" : 			{
				"source" : [ "obj-57", 0 ],
				"destination" : [ "obj-53", 0 ],
				"hidden" : 0,
				"midpoints" : [ 334.5, 416.0, 409.5, 416.0 ]
			}

		}
, 		{
			"patchline" : 			{
				"source" : [ "obj-30", 0 ],
				"destination" : [ "obj-57", 0 ],
				"hidden" : 0,
				"midpoints" : [  ]
			}

		}
, 		{
			"patchline" : 			{
				"source" : [ "obj-55", 0 ],
				"destination" : [ "obj-54", 0 ],
				"hidden" : 0,
				"midpoints" : [ 409.5, 474.0, 468.0, 474.0, 468.0, 366.0, 409.5, 366.0 ]
			}

		}
, 		{
			"patchline" : 			{
				"source" : [ "obj-53", 0 ],
				"destination" : [ "obj-55", 0 ],
				"hidden" : 0,
				"midpoints" : [  ]
			}

		}
, 		{
			"patchline" : 			{
				"source" : [ "obj-23", 0 ],
				"destination" : [ "obj-62", 0 ],
				"hidden" : 0,
				"midpoints" : [  ]
			}

		}
, 		{
			"patchline" : 			{
				"source" : [ "obj-22", 0 ],
				"destination" : [ "obj-62", 1 ],
				"hidden" : 0,
				"midpoints" : [  ]
			}

		}
, 		{
			"patchline" : 			{
				"source" : [ "obj-3", 0 ],
				"destination" : [ "obj-62", 2 ],
				"hidden" : 0,
				"midpoints" : [  ]
			}

		}
, 		{
			"patchline" : 			{
				"source" : [ "obj-62", 0 ],
				"destination" : [ "obj-39", 0 ],
				"hidden" : 0,
				"midpoints" : [  ]
			}

		}
, 		{
			"patchline" : 			{
				"source" : [ "obj-14", 0 ],
				"destination" : [ "obj-54", 1 ],
				"hidden" : 0,
				"midpoints" : [ 253.5, 357.0, 444.5, 357.0 ]
			}

		}
, 		{
			"patchline" : 			{
				"source" : [ "obj-53", 0 ],
				"destination" : [ "obj-50", 0 ],
				"hidden" : 0,
				"midpoints" : [ 409.5, 441.0, 218.5, 441.0 ]
			}

		}
, 		{
			"patchline" : 			{
				"source" : [ "obj-14", 0 ],
				"destination" : [ "obj-50", 1 ],
				"hidden" : 0,
				"midpoints" : [  ]
			}

		}
, 		{
			"patchline" : 			{
				"source" : [ "obj-11", 0 ],
				"destination" : [ "obj-30", 0 ],
				"hidden" : 0,
				"midpoints" : [ 253.5, 206.0, 336.5, 206.0 ]
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
				"source" : [ "obj-30", 0 ],
				"destination" : [ "obj-21", 0 ],
				"hidden" : 0,
				"midpoints" : [  ]
			}

		}
, 		{
			"patchline" : 			{
				"source" : [ "obj-21", 0 ],
				"destination" : [ "obj-14", 0 ],
				"hidden" : 0,
				"midpoints" : [ 336.5, 326.5, 253.5, 326.5 ]
			}

		}
, 		{
			"patchline" : 			{
				"source" : [ "obj-19", 0 ],
				"destination" : [ "obj-14", 0 ],
				"hidden" : 0,
				"midpoints" : [ 358.5, 303.5, 253.5, 303.5 ]
			}

		}
, 		{
			"patchline" : 			{
				"source" : [ "obj-13", 0 ],
				"destination" : [ "obj-14", 0 ],
				"hidden" : 0,
				"midpoints" : [ 378.5, 275.5, 253.5, 275.5 ]
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
				"source" : [ "obj-12", 0 ],
				"destination" : [ "obj-14", 0 ],
				"hidden" : 0,
				"midpoints" : [  ]
			}

		}
, 		{
			"patchline" : 			{
				"source" : [ "obj-10", 2 ],
				"destination" : [ "obj-4", 0 ],
				"hidden" : 0,
				"midpoints" : [ 274.5, 542.0, 274.5, 542.0 ]
			}

		}
, 		{
			"patchline" : 			{
				"source" : [ "obj-10", 0 ],
				"destination" : [ "obj-4", 0 ],
				"hidden" : 0,
				"midpoints" : [ 218.5, 529.5, 274.5, 529.5 ]
			}

		}
, 		{
			"patchline" : 			{
				"source" : [ "obj-4", 0 ],
				"destination" : [ "obj-8", 0 ],
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
				"source" : [ "obj-7", 0 ],
				"destination" : [ "obj-35", 0 ],
				"hidden" : 0,
				"midpoints" : [ 274.5, 614.0, 218.5, 614.0 ]
			}

		}
, 		{
			"patchline" : 			{
				"source" : [ "obj-50", 0 ],
				"destination" : [ "obj-10", 0 ],
				"hidden" : 0,
				"midpoints" : [  ]
			}

		}
, 		{
			"patchline" : 			{
				"source" : [ "obj-29", 0 ],
				"destination" : [ "obj-23", 0 ],
				"hidden" : 0,
				"midpoints" : [  ]
			}

		}
, 		{
			"patchline" : 			{
				"source" : [ "obj-35", 1 ],
				"destination" : [ "obj-29", 0 ],
				"hidden" : 0,
				"midpoints" : [  ]
			}

		}
, 		{
			"patchline" : 			{
				"source" : [ "obj-29", 4 ],
				"destination" : [ "obj-3", 0 ],
				"hidden" : 0,
				"midpoints" : [  ]
			}

		}
, 		{
			"patchline" : 			{
				"source" : [ "obj-29", 2 ],
				"destination" : [ "obj-22", 0 ],
				"hidden" : 0,
				"midpoints" : [  ]
			}

		}
 ]
}

*/
