/*
  Dimmer
 
 Demonstrates the sending data from the computer to the Arduino board,
 in this case to control the brightness of an LED.  The data is sent
 in individual bytes, each of which ranges from 0 to 255.  Arduino
 reads these bytes and uses them to set the brightness of the LED.
 
 The circuit:
 LED attached from digital pin 9 to ground.
 Serial connection to Processing, Max/MSP, or another serial application
 
 created 2006
 by David A. Mellis
 modified 14 Apr 2009
 by Tom Igoe and Scott Fitzgerald
 
 This example code is in the public domain.
 
 http://www.arduino.cc/en/Tutorial/Dimmer
 
 */

const int ledPin = 9;      // the pin that the LED is attached to

void setup()
{
  // initialize the serial communication:
  Serial.begin(9600);
  // initialize the ledPin as an output:
  pinMode(ledPin, OUTPUT);
}

void loop() {
  byte brightness;

  // check if data has been sent from the computer:
  if (Serial.available()) {
    // read the most recent byte (which will be from 0 to 255):
    brightness = Serial.read();
    // set the brightness of the LED:
    analogWrite(ledPin, brightness);
  }
}

/* Processing code for this example
 // Dimmer - sends bytes over a serial port
 // by David A. Mellis
 //This example code is in the public domain.
 
 import processing.serial.*;
 Serial port;
 
 void setup() {
 size(256, 150);
 
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
 
 void draw() {
 // draw a gradient from black to white
 for (int i = 0; i < 256; i++) {
 stroke(i);
 line(i, 0, i, 150);
 }
 
 // write the current X-position of the mouse to the serial port as
 // a single byte
 port.write(mouseX);
 }
 */

/* Max/MSP v5 patch for this example
 
 {
 	"boxes" : [ 		{
 			"box" : 			{
 				"maxclass" : "comment",
 				"text" : "Dimmer\n\nThis patch sends a binary number from 0 to 255 out the serial port to an Arduino connected to the port.  It dims an LED attached to the Arduino.\n\ncreated 2006\nby David A. Mellis\nmodified 14 Apr 2009\nby Scott Fitzgerald and Tom Igoe",
 				"linecount" : 10,
 				"patching_rect" : [ 209.0, 55.0, 344.0, 144.0 ],
 				"numoutlets" : 0,
 				"fontsize" : 12.0,
 				"id" : "obj-32",
 				"fontname" : "Arial",
 				"numinlets" : 1
 			}
 
 		}
 , 		{
 			"box" : 			{
 				"maxclass" : "comment",
 				"text" : "change the slider to alter the brightness of the LED",
 				"linecount" : 3,
 				"patching_rect" : [ 90.0, 235.0, 117.0, 48.0 ],
 				"numoutlets" : 0,
 				"fontsize" : 12.0,
 				"id" : "obj-7",
 				"fontname" : "Arial",
 				"numinlets" : 1
 			}
 
 		}
 , 		{
 			"box" : 			{
 				"maxclass" : "number",
 				"patching_rect" : [ 215.0, 385.0, 50.0, 19.0 ],
 				"numoutlets" : 2,
 				"fontsize" : 10.0,
 				"outlettype" : [ "int", "bang" ],
 				"id" : "obj-6",
 				"fontname" : "Verdana",
 				"numinlets" : 1
 			}
 
 		}
 , 		{
 			"box" : 			{
 				"maxclass" : "slider",
 				"patching_rect" : [ 215.0, 235.0, 20.0, 140.0 ],
 				"numoutlets" : 1,
 				"outlettype" : [ "" ],
 				"bgcolor" : [ 0.94902, 0.94902, 0.94902, 0.0 ],
 				"id" : "obj-1",
 				"size" : 256.0,
 				"numinlets" : 1
 			}
 
 		}
 , 		{
 			"box" : 			{
 				"maxclass" : "newobj",
 				"text" : "select 0 1",
 				"patching_rect" : [ 342.0, 305.0, 62.0, 20.0 ],
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
 				"patching_rect" : [ 390.0, 396.0, 206.0, 20.0 ],
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
 				"patching_rect" : [ 415.0, 370.0, 206.0, 20.0 ],
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
 				"patching_rect" : [ 342.0, 396.0, 39.0, 18.0 ],
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
 				"patching_rect" : [ 364.0, 370.0, 41.0, 18.0 ],
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
 				"patching_rect" : [ 435.0, 344.0, 207.0, 20.0 ],
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
 				"patching_rect" : [ 342.0, 268.0, 15.0, 15.0 ],
 				"numoutlets" : 1,
 				"outlettype" : [ "int" ],
 				"id" : "obj-11",
 				"numinlets" : 1
 			}
 
 		}
 , 		{
 			"box" : 			{
 				"maxclass" : "message",
 				"text" : "print",
 				"patching_rect" : [ 384.0, 344.0, 36.0, 18.0 ],
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
 				"patching_rect" : [ 259.0, 420.0, 84.0, 20.0 ],
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
 				"text" : "Click to start",
 				"patching_rect" : [ 369.0, 268.0, 117.0, 20.0 ],
 				"numoutlets" : 0,
 				"fontsize" : 12.0,
 				"id" : "obj-17",
 				"fontname" : "Arial",
 				"numinlets" : 1
 			}
 
 		}
 , 		{
 			"box" : 			{
 				"maxclass" : "panel",
 				"patching_rect" : [ 215.0, 235.0, 21.0, 139.0 ],
 				"numoutlets" : 0,
 				"mode" : 1,
 				"grad1" : [ 1.0, 1.0, 1.0, 1.0 ],
 				"id" : "obj-8",
 				"grad2" : [ 0.509804, 0.509804, 0.509804, 1.0 ],
 				"numinlets" : 1,
 				"angle" : 270.0
 			}
 
 		}
 ],
 	"lines" : [ 		{
 			"patchline" : 			{
 				"source" : [ "obj-11", 0 ],
 				"destination" : [ "obj-30", 0 ],
 				"hidden" : 0,
 				"midpoints" : [ 351.0, 296.0, 351.5, 296.0 ]
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
 				"midpoints" : [ 351.5, 416.5, 268.5, 416.5 ]
 			}
 
 		}
 , 		{
 			"patchline" : 			{
 				"source" : [ "obj-19", 0 ],
 				"destination" : [ "obj-14", 0 ],
 				"hidden" : 0,
 				"midpoints" : [ 373.5, 393.5, 268.5, 393.5 ]
 			}
 
 		}
 , 		{
 			"patchline" : 			{
 				"source" : [ "obj-13", 0 ],
 				"destination" : [ "obj-14", 0 ],
 				"hidden" : 0,
 				"midpoints" : [ 393.5, 365.5, 268.5, 365.5 ]
 			}
 
 		}
 , 		{
 			"patchline" : 			{
 				"source" : [ "obj-1", 0 ],
 				"destination" : [ "obj-6", 0 ],
 				"hidden" : 0,
 				"midpoints" : [  ]
 			}
 
 		}
 , 		{
 			"patchline" : 			{
 				"source" : [ "obj-6", 0 ],
 				"destination" : [ "obj-14", 0 ],
 				"hidden" : 0,
 				"midpoints" : [ 224.5, 411.5, 268.5, 411.5 ]
 			}
 
 		}
 ]
 }
 */
