/*
  Arduino Starter Kit example
 Project 14  - Tweak the Arduino Logo

 This sketch is written to accompany Project 14 in the
 Arduino Starter Kit

 Parts required:
 10 kilohm potentiometer

 Software required :
 Processing http://processing.org
 Active internet connection

 Created 18 September 2012
 by Scott Fitzgerald

 http://www.arduino.cc/starterKit

 This example code is part of the public domain
 */


void setup() {
  // initialize serial communication
  Serial.begin(9600);
}

void loop() {
  // read the value of A0, divide by 4 and
  // send it as a byte over the serial connection
  Serial.write(analogRead(A0) / 4);
  delay(1);
}

/* Processing code for this example
 // Tweak the Arduno Logo
 // by Scott Fitzgerald
 // This example code is in the public domain

 // import the serial library
 import processing.serial.*;

 // create an instance of the serial library
 Serial myPort;

 // create an instance of PImage
 PImage logo;

 // a variable to hold the background color
 int bgcolor = 0;

 void setup() {
 // set the color mode to Hue/Saturation/Brightness
 colorMode(HSB, 255);

 // load the Arduino logo into the PImage instance
 logo = loadImage("http://www.arduino.cc/en/pub/skins/arduinoWide/img/logo.png");

 // make the window the same size as the image
 size(logo.width, logo.height);

 // print a list of available serial ports to the
 // Processing staus window
 println("Available serial ports:");
 println(Serial.list());

 // Tell the serial object the information it needs to communicate
 // with the Arduno. Change Serial.list()[0] to the correct
 // port corresponding to your Arduino board.  The last
 // parameter (e.g. 9600) is the speed of the communication.  It
 // has to correspond to the value passed to Serial.begin() in your
 // Arduino sketch.
 myPort = new Serial(this, Serial.list()[0], 9600);

 // If you know the name of the port used by the Arduino board, you
 // can specify it directly like this.
 // port = new Serial(this, "COM1", 9600);

 }

 void draw() {

 // if there is information in the serial port
 if ( myPort.available() > 0) {
 // read the value and store it in a variable
 bgcolor = myPort.read();

 // print the value to the status window
 println(bgcolor);
 }

 // Draw the background. the variable bgcolor
 // contains the Hue, determined by the value
 // from the serial port
 background(bgcolor, 255, 255);

 // draw the Arduino logo
 image(logo, 0, 0);
 }

 */


