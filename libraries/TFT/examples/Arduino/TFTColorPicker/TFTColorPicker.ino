/*

 TFT Color Picker
 
 This example for the Arduino screen reads the input of 
 potentiometers or analog sensors attached to A0, A1,
 and A2 and uses the values to change the screen's color.
 
 This example code is in the public domain.
 
 Created 15 April 2013 by Scott Fitzgerald
 
 http://arduino.cc/en/Tutorial/TFTColorPicker
 
 */
 
// pin definition for the Uno
#define cs   10
#define dc   9
#define rst  8  

// pin definition for the Leonardo
// #define cs   7
// #define dc   0
// #define rst  1 

#include <TFT.h>  // Arduino LCD library
#include <SPI.h>

TFT TFTscreen = TFT(cs, dc, rst);

void setup() {
  // begin serial communication
  Serial.begin(9600);

  // initialize the display
  TFTscreen.begin();

  // set the background to white
  TFTscreen.background(255, 255, 255);

}

void loop() {

  // read the values from your sensors and scale them to 0-255
  int redVal = map(analogRead(A0), 0, 1023, 0, 255); 
  int greenVal = map(analogRead(A1), 0, 1023, 0, 255); 
  int blueVal = map(analogRead(A2), 0, 1023, 0, 255);
  
  // draw the background based on the mapped values
  TFTscreen.background(redVal, greenVal, blueVal);
  
  // send the values to the serial monitor
  Serial.print("background(");
  Serial.print(redVal);
  Serial.print(" , ");
  Serial.print(greenVal);
  Serial.print(" , ");
  Serial.print(blueVal);
  Serial.println(")");
  
  // wait for a moment
  delay(33);

}

