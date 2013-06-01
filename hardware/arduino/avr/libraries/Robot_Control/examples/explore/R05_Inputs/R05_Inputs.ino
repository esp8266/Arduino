/* Robot Inputs

 This sketch shows you how to use the on-board 
 potentiometer and buttons as inputs. 

 Turning the potentiometer draws a clock-shaped 
 circle. The up and down buttons change the pitch, 
 while the left and right buttons change the tempo. 
 The middle button resets tempo and pitch.

 Circuit:
 * Arduino Robot
 
 created 1 May 2013
 by X. Yang
 modified 12 May 2013
 by D. Cuartielles
 
 This example is in the public domain
 */

#include <ArduinoRobot.h>

// default tempo and pitch of the music
int tempo = 60;
int pitch = 1000;

void setup() {
  // initialize the Robot, SD card, speaker, and display 
  Robot.begin();
  Robot.beginTFT();
  Robot.beginSpeaker();
  Robot.beginSD();

  // draw "lg0.bmp" and "lg1.bmp" on the screen  
  Robot.displayLogos();

  // play a sound file  
  Robot.playFile("Melody.sqm");
}

void loop() {
  // check the value of the buttons
  keyDown(Robot.keyboardRead());

  // check the value of the pot
  drawKnob(Robot.knobRead());
}

// Draw the basic interface
void renderUI() {
  //fill the buttons blank
  Robot.fill(255, 255, 255);      
  Robot.rect(53, 58, 13, 13); // left
  Robot.rect(93, 58, 13, 13); // right
  Robot.rect(73, 38, 13, 13); // up
  Robot.circle(79, 64, 6); // middle
  Robot.rect(73, 78, 13, 13); // down
  Robot.circle(26, 116, 18); // knob

  //draw the vertical bargraph
  int fullPart=map(pitch, 200, 2000, 0, 58);  //length of filled bargraph
  Robot.fill(255, 255, 255);        
  Robot.rect(21, 30, 13, 58-fullPart);
  Robot.fill(0, 0, 255);        
  Robot.rect(21, 88-fullPart, 13, fullPart); //58-fullPart+30

  //draw the horizontal bargraph
  fullPart = map(tempo, 20, 100, 0, 58);  // length of filled bargraph
  Robot.fill(255, 190, 0);  
  Robot.rect(53, 110, fullPart, 13);
  Robot.fill(255, 255, 255);        
  Robot.rect(53+fullPart, 110, 58-fullPart, 13);
}

void keyDown(int keyCode) {
  // use a static int so it is persistent over time
  static int oldKey;
  switch(keyCode) {
    case BUTTON_LEFT:
      //left button pressed, reduces tempo
      tempo -= 5;
      if(tempo < 20) tempo = 20; //lowest tempo 20
      Robot.fill(255,190,0);

      Robot.rect(53, 58, 13, 13);
      break;
    case BUTTON_RIGHT:
      //right button pressed, increases tempo
      tempo += 5;
      if(tempo > 100) tempo = 100; //highest tempo 100
      Robot.fill(255,190,0);
      Robot.rect(93, 58, 13, 13);
      break;
    case BUTTON_UP:
      //up button pressed, increases pitch
      pitch += 120;
      if(pitch > 2000) pitch = 2000;
      Robot.fill(0, 0, 255);

      Robot.rect(73, 38, 13, 13);
      break;
    case BUTTON_DOWN:
      //down button pressed, reduces pitch
      pitch -= 120;
      if(pitch < 200){
        pitch = 200;  
      }
      Robot.fill(0, 0, 255);

      Robot.rect(73, 78, 13, 13);
      break;
    case BUTTON_MIDDLE:
      //middle button pressed, resets tempo and pitch
      tempo = 60;
      pitch = 1000;
      Robot.fill(160,160,160);

      Robot.circle(79, 64, 6);
      break;
    case BUTTON_NONE:
      //Only when the keys are released(thus BUTTON_NONE is
      //encountered the first time), the interface will be 
      //re-drawn.
      if(oldKey != BUTTON_NONE){  
        renderUI();
      }
      break;
  }
  if(oldKey != keyCode) {
    // change the song's tempo
    Robot.tempoWrite(tempo);
    // change the song's pitch
    Robot.tuneWrite(float(pitch/1000.0));
  }
  oldKey = keyCode;
}

void drawKnob(int val) {
  static int x = 0, y = 0, val_old = 0;
  // radian number, -3.14 to 3.14
  float ang = map(val, 0, 1023, -PI*1000, PI*1000) / 1000.0;

  // erase the old line  
  if (val_old != val) {
    Robot.stroke(255, 255, 255);
    Robot.line(26, 116, x, y);
  }
  
  // the following lines avoid a glitch in the TFT library
  // that seems to appear when drawing a vertical line 
  if (val < 1011 && val > 265 || val < 253) { 
    //a bit math for drawing the hand inside the clock
    x = 16*sin(ang)+26;
    y = 16*cos(ang)+116;
  } 
  if (val > 265 && val < 253) {
    x = 10; y = 116; 
  }    
  if (val >= 1011) {
    x = 27; y = 100; 
  }    
  Robot.stroke(0, 0, 0);
  Robot.line(26, 116, x, y); 
  val_old = val;
}
