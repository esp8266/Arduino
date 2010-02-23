/*
  Physical Pixel
 
 An example of using the Arduino board to receive data from the 
 computer.  In this case, the Arduino boards turns on an LED when
 it receives the character 'H', and turns off the LED when it
 receives the character 'L'.
 
 The data can be sent from the Arduino serial monitor, or another
 program like Processing (see code below), Flash (via a serial-net
 proxy), PD, or Max/MSP.
 
 The circuit:
 * LED connected from digital pin 13 to ground
 
 created 2006
 by David A. Mellis
 modified 14 Apr 2009
 by Tom Igoe and Scott Fitzgerald
 
 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/PhysicalPixel
 */

const int ledPin = 13; // the pin that the LED is attached to
int incomingByte;      // a variable to read incoming serial data into

void setup() {
  // initialize serial communication:
  Serial.begin(9600);
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // see if there's incoming serial data:
  if (Serial.available() > 0) {
    // read the oldest byte in the serial buffer:
    incomingByte = Serial.read();
    // if it's a capital H (ASCII 72), turn on the LED:
    if (incomingByte == 'H') {
      digitalWrite(ledPin, HIGH);
    } 
    // if it's an L (ASCII 76) turn off the LED:
    if (incomingByte == 'L') {
      digitalWrite(ledPin, LOW);
    }
  }
}

/* Processing code for this example
 
 // mouseover serial 
 
 // Demonstrates how to send data to the Arduino I/O board, in order to 
 // turn ON a light if the mouse is over a square and turn it off 
 // if the mouse is not. 
 
 // created 2003-4
 // based on examples by Casey Reas and Hernando Barragan
 // modified 18 Jan 2009
 // by Tom Igoe
 // This example code is in the public domain.

 
 
 import processing.serial.*; 
 
 float boxX;
 float boxY;
 int boxSize = 20;
 boolean mouseOverBox = false;
 
 Serial port; 
 
 void setup()  {
 size(200, 200);
 boxX = width/2.0;
 boxY = height/2.0;
 rectMode(RADIUS); 
 
 // List all the available serial ports in the output pane. 
 // You will need to choose the port that the Arduino board is 
 // connected to from this list. The first port in the list is 
 // port #0 and the third port in the list is port #2. 
 println(Serial.list()); 
 
 // Open the port that the Arduino board is connected to (in this case #0) 
 // Make sure to open the port at the same speed Arduino is using (9600bps) 
 port = new Serial(this, Serial.list()[0], 9600); 
 
 }
 
 void draw() 
 { 
 background(0);
 
 // Test if the cursor is over the box 
 if (mouseX > boxX-boxSize && mouseX < boxX+boxSize && 
 mouseY > boxY-boxSize && mouseY < boxY+boxSize) {
 mouseOverBox = true;  
 // draw a line around the box and change its color:
 stroke(255); 
 fill(153);
 // send an 'H' to indicate mouse is over square:
 port.write('H');       
 } 
 else {
 // return the box to it's inactive state:
 stroke(153);
 fill(153);
 // send an 'L' to turn the LED off: 
 port.write('L');      
 mouseOverBox = false;
 }
 
 // Draw the box
 rect(boxX, boxY, boxSize, boxSize);
 }
 
 
 */

/*
{
 	"boxes" : [ 		{
 			"box" : 			{
 				"maxclass" : "comment",
 				"text" : "Physical Pixel\n\nThis patch sends an ASCII H or an ASCII L out the serial port to turn on an LED attached to an Arduino board. It can also send alternating H and L characters once every second to make the LED blink.\n\ncreated 2006\nby David A. Mellis\nmodified 14 Apr 2009\nby Scott Fitzgerald and Tom Igoe",
 				"linecount" : 11,
 				"patching_rect" : [ 14.0, 35.0, 354.0, 158.0 ],
 				"numoutlets" : 0,
 				"fontsize" : 12.0,
 				"id" : "obj-1",
 				"fontname" : "Arial",
 				"numinlets" : 1
 			}
 
 		}
 , 		{
 			"box" : 			{
 				"maxclass" : "comment",
 				"text" : "Click to blink every second",
 				"patching_rect" : [ 99.0, 251.0, 161.0, 20.0 ],
 				"numoutlets" : 0,
 				"fontsize" : 12.0,
 				"id" : "obj-38",
 				"fontname" : "Arial",
 				"numinlets" : 1
 			}
 
 		}
 , 		{
 			"box" : 			{
 				"maxclass" : "toggle",
 				"patching_rect" : [ 74.0, 251.0, 21.0, 21.0 ],
 				"numoutlets" : 1,
 				"outlettype" : [ "int" ],
 				"id" : "obj-39",
 				"numinlets" : 1
 			}
 
 		}
 , 		{
 			"box" : 			{
 				"maxclass" : "newobj",
 				"text" : "p blink",
 				"patching_rect" : [ 74.0, 286.0, 45.0, 20.0 ],
 				"numoutlets" : 1,
 				"fontsize" : 12.0,
 				"outlettype" : [ "int" ],
 				"id" : "obj-37",
 				"fontname" : "Arial",
 				"numinlets" : 2,
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
 								"maxclass" : "newobj",
 								"text" : "* 1000",
 								"patching_rect" : [ 200.0, 150.0, 46.0, 19.0 ],
 								"numoutlets" : 1,
 								"fontsize" : 10.0,
 								"outlettype" : [ "int" ],
 								"id" : "obj-12",
 								"fontname" : "Verdana",
 								"numinlets" : 2
 							}
 
 						}
 , 						{
 							"box" : 							{
 								"maxclass" : "inlet",
 								"patching_rect" : [ 200.0, 75.0, 25.0, 25.0 ],
 								"numoutlets" : 1,
 								"outlettype" : [ "" ],
 								"id" : "obj-11",
 								"numinlets" : 0,
 								"comment" : ""
 							}
 
 						}
 , 						{
 							"box" : 							{
 								"maxclass" : "toggle",
 								"patching_rect" : [ 125.0, 250.0, 20.0, 20.0 ],
 								"numoutlets" : 1,
 								"outlettype" : [ "int" ],
 								"id" : "obj-10",
 								"numinlets" : 1
 							}
 
 						}
 , 						{
 							"box" : 							{
 								"maxclass" : "newobj",
 								"text" : "metro 1000",
 								"patching_rect" : [ 115.0, 190.0, 69.0, 19.0 ],
 								"numoutlets" : 1,
 								"fontsize" : 10.0,
 								"outlettype" : [ "bang" ],
 								"id" : "obj-3",
 								"fontname" : "Verdana",
 								"numinlets" : 2
 							}
 
 						}
 , 						{
 							"box" : 							{
 								"maxclass" : "outlet",
 								"patching_rect" : [ 125.0, 400.0, 25.0, 25.0 ],
 								"numoutlets" : 0,
 								"id" : "obj-2",
 								"numinlets" : 1,
 								"comment" : ""
 							}
 
 						}
 , 						{
 							"box" : 							{
 								"maxclass" : "inlet",
 								"patching_rect" : [ 100.0, 25.0, 25.0, 25.0 ],
 								"numoutlets" : 1,
 								"outlettype" : [ "int" ],
 								"id" : "obj-1",
 								"numinlets" : 0,
 								"comment" : ""
 							}
 
 						}
 ],
 					"lines" : [ 						{
 							"patchline" : 							{
 								"source" : [ "obj-12", 0 ],
 								"destination" : [ "obj-3", 1 ],
 								"hidden" : 0,
 								"midpoints" : [  ]
 							}
 
 						}
 , 						{
 							"patchline" : 							{
 								"source" : [ "obj-11", 0 ],
 								"destination" : [ "obj-12", 0 ],
 								"hidden" : 0,
 								"midpoints" : [  ]
 							}
 
 						}
 , 						{
 							"patchline" : 							{
 								"source" : [ "obj-1", 0 ],
 								"destination" : [ "obj-3", 0 ],
 								"hidden" : 0,
 								"midpoints" : [  ]
 							}
 
 						}
 , 						{
 							"patchline" : 							{
 								"source" : [ "obj-10", 0 ],
 								"destination" : [ "obj-2", 0 ],
 								"hidden" : 0,
 								"midpoints" : [  ]
 							}
 
 						}
 , 						{
 							"patchline" : 							{
 								"source" : [ "obj-3", 0 ],
 								"destination" : [ "obj-10", 0 ],
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
 				"maxclass" : "comment",
 				"text" : "convert to int",
 				"patching_rect" : [ 154.0, 386.0, 104.0, 20.0 ],
 				"numoutlets" : 0,
 				"fontsize" : 12.0,
 				"id" : "obj-36",
 				"fontname" : "Arial",
 				"numinlets" : 1
 			}
 
 		}
 , 		{
 			"box" : 			{
 				"maxclass" : "comment",
 				"text" : "send L if 0, H if 1",
 				"patching_rect" : [ 154.0, 361.0, 104.0, 20.0 ],
 				"numoutlets" : 0,
 				"fontsize" : 12.0,
 				"id" : "obj-35",
 				"fontname" : "Arial",
 				"numinlets" : 1
 			}
 
 		}
 , 		{
 			"box" : 			{
 				"maxclass" : "comment",
 				"text" : "is it on or off?",
 				"patching_rect" : [ 179.0, 336.0, 95.0, 20.0 ],
 				"numoutlets" : 0,
 				"fontsize" : 12.0,
 				"id" : "obj-34",
 				"fontname" : "Arial",
 				"numinlets" : 1
 			}
 
 		}
 , 		{
 			"box" : 			{
 				"maxclass" : "newobj",
 				"text" : "atoi",
 				"patching_rect" : [ 279.0, 386.0, 46.0, 20.0 ],
 				"numoutlets" : 1,
 				"fontsize" : 12.0,
 				"outlettype" : [ "list" ],
 				"id" : "obj-33",
 				"fontname" : "Arial",
 				"numinlets" : 3
 			}
 
 		}
 , 		{
 			"box" : 			{
 				"maxclass" : "message",
 				"text" : "H",
 				"patching_rect" : [ 329.0, 361.0, 32.5, 17.0 ],
 				"numoutlets" : 1,
 				"fontsize" : 10.0,
 				"outlettype" : [ "" ],
 				"id" : "obj-32",
 				"fontname" : "Verdana",
 				"numinlets" : 2
 			}
 
 		}
 , 		{
 			"box" : 			{
 				"maxclass" : "message",
 				"text" : "L",
 				"patching_rect" : [ 279.0, 361.0, 32.5, 17.0 ],
 				"numoutlets" : 1,
 				"fontsize" : 10.0,
 				"outlettype" : [ "" ],
 				"id" : "obj-31",
 				"fontname" : "Verdana",
 				"numinlets" : 2
 			}
 
 		}
 , 		{
 			"box" : 			{
 				"maxclass" : "newobj",
 				"text" : "select 0 1",
 				"patching_rect" : [ 279.0, 336.0, 62.0, 20.0 ],
 				"numoutlets" : 3,
 				"fontsize" : 12.0,
 				"outlettype" : [ "bang", "bang", "" ],
 				"id" : "obj-25",
 				"fontname" : "Arial",
 				"numinlets" : 1
 			}
 
 		}
 , 		{
 			"box" : 			{
 				"maxclass" : "comment",
 				"text" : "Click to turn the LED on and off",
 				"linecount" : 2,
 				"patching_rect" : [ 130.0, 205.0, 143.0, 34.0 ],
 				"numoutlets" : 0,
 				"fontsize" : 12.0,
 				"id" : "obj-24",
 				"fontname" : "Arial",
 				"numinlets" : 1
 			}
 
 		}
 , 		{
 			"box" : 			{
 				"maxclass" : "toggle",
 				"patching_rect" : [ 279.0, 211.0, 24.0, 24.0 ],
 				"numoutlets" : 1,
 				"outlettype" : [ "int" ],
 				"id" : "obj-23",
 				"numinlets" : 1
 			}
 
 		}
 , 		{
 			"box" : 			{
 				"maxclass" : "newobj",
 				"text" : "select 0 1",
 				"patching_rect" : [ 381.0, 331.0, 62.0, 20.0 ],
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
 				"patching_rect" : [ 429.0, 422.0, 206.0, 20.0 ],
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
 				"patching_rect" : [ 454.0, 396.0, 206.0, 20.0 ],
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
 				"patching_rect" : [ 381.0, 422.0, 39.0, 18.0 ],
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
 				"patching_rect" : [ 403.0, 396.0, 41.0, 18.0 ],
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
 				"patching_rect" : [ 474.0, 370.0, 207.0, 20.0 ],
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
 				"patching_rect" : [ 381.0, 181.0, 21.0, 21.0 ],
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
 				"patching_rect" : [ 423.0, 370.0, 36.0, 18.0 ],
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
 				"patching_rect" : [ 279.0, 461.0, 84.0, 20.0 ],
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
 				"patching_rect" : [ 408.0, 181.0, 117.0, 20.0 ],
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
 				"source" : [ "obj-39", 0 ],
 				"destination" : [ "obj-37", 0 ],
 				"hidden" : 0,
 				"midpoints" : [  ]
 			}
 
 		}
 , 		{
 			"patchline" : 			{
 				"source" : [ "obj-37", 0 ],
 				"destination" : [ "obj-25", 0 ],
 				"hidden" : 0,
 				"midpoints" : [ 83.5, 320.5, 288.5, 320.5 ]
 			}
 
 		}
 , 		{
 			"patchline" : 			{
 				"source" : [ "obj-33", 0 ],
 				"destination" : [ "obj-14", 0 ],
 				"hidden" : 0,
 				"midpoints" : [  ]
 			}
 
 		}
 , 		{
 			"patchline" : 			{
 				"source" : [ "obj-32", 0 ],
 				"destination" : [ "obj-33", 0 ],
 				"hidden" : 0,
 				"midpoints" : [ 338.5, 381.5, 288.5, 381.5 ]
 			}
 
 		}
 , 		{
 			"patchline" : 			{
 				"source" : [ "obj-31", 0 ],
 				"destination" : [ "obj-33", 0 ],
 				"hidden" : 0,
 				"midpoints" : [  ]
 			}
 
 		}
 , 		{
 			"patchline" : 			{
 				"source" : [ "obj-25", 0 ],
 				"destination" : [ "obj-31", 0 ],
 				"hidden" : 0,
 				"midpoints" : [  ]
 			}
 
 		}
 , 		{
 			"patchline" : 			{
 				"source" : [ "obj-25", 1 ],
 				"destination" : [ "obj-32", 0 ],
 				"hidden" : 0,
 				"midpoints" : [ 310.0, 358.0, 338.5, 358.0 ]
 			}
 
 		}
 , 		{
 			"patchline" : 			{
 				"source" : [ "obj-23", 0 ],
 				"destination" : [ "obj-25", 0 ],
 				"hidden" : 0,
 				"midpoints" : [  ]
 			}
 
 		}
 , 		{
 			"patchline" : 			{
 				"source" : [ "obj-13", 0 ],
 				"destination" : [ "obj-14", 0 ],
 				"hidden" : 0,
 				"midpoints" : [ 432.5, 389.0, 367.0, 389.0, 367.0, 411.0, 288.5, 411.0 ]
 			}
 
 		}
 , 		{
 			"patchline" : 			{
 				"source" : [ "obj-19", 0 ],
 				"destination" : [ "obj-14", 0 ],
 				"hidden" : 0,
 				"midpoints" : [ 412.5, 417.0, 288.5, 417.0 ]
 			}
 
 		}
 , 		{
 			"patchline" : 			{
 				"source" : [ "obj-21", 0 ],
 				"destination" : [ "obj-14", 0 ],
 				"hidden" : 0,
 				"midpoints" : [ 390.5, 450.0, 288.5, 450.0 ]
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
 				"midpoints" : [ 390.5, 322.0, 390.5, 322.0 ]
 			}
 
 		}
 ]
 }
 
 */
