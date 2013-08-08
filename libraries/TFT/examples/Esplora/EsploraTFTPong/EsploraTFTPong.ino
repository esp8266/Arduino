/*

 Esplora TFT Pong
 
 This example for the Esplora with an Arduino TFT screen reads 
 the value of the joystick to move a rectangular platform
 on the x and y axes. The platform can intersect with a ball
 causing it to bounce. The Esplora's slider adjusts the speed
 of the ball.
 
 This example code is in the public domain.
 
 Created by Tom Igoe December 2012
 Modified 15 April 2013 by Scott Fitzgerald
 
 http://arduino.cc/en/Tutorial/EsploraTFTPong
 
 */

#include <Esplora.h>
#include <TFT.h>            // Arduino LCD library
#include <SPI.h>

// variables for the position of the ball and paddle
int paddleX = 0;
int paddleY = 0;
int oldPaddleX, oldPaddleY;
int ballDirectionX = 1;
int ballDirectionY = 1;

int ballX, ballY, oldBallX, oldBallY;

void setup() {

  Serial.begin(9600);

  // initialize the display
  EsploraTFT.begin();
  // set the background the black
  EsploraTFT.background(0,0,0); 
}

void loop() {
  // save the width and height of the screen
  int myWidth = EsploraTFT.width();
  int myHeight = EsploraTFT.height();

  // map the paddle's location to the joystick's position 
  paddleX = map(Esplora.readJoystickX(), 512, -512, 0, myWidth) - 20/2; 
  paddleY = map(Esplora.readJoystickY(), -512, 512, 0, myHeight) - 5/2; 
  Serial.print(paddleX);
  Serial.print(" ");
  Serial.println(paddleY);

  // set the fill color to black and erase the previous 
  // position of the paddle if different from present
  EsploraTFT.fill(0,0,0);

  if (oldPaddleX != paddleX || oldPaddleY != paddleY) {
    EsploraTFT.rect(oldPaddleX, oldPaddleY, 20, 5);
  }

  // draw the paddle on screen, save the current position
  // as the previous.
  EsploraTFT.fill(255,255,255);
  EsploraTFT.rect(paddleX, paddleY, 20, 5);
  oldPaddleX = paddleX;
  oldPaddleY = paddleY;

  // read the slider to determinde the speed of the ball
  int ballSpeed = map(Esplora.readSlider(), 0, 1023, 0, 80)+1;
  if (millis() % ballSpeed < 2) {
    moveBall();
  }
}


// this function determines the ball's position on screen
void moveBall() {
  // if the ball goes offscreen, reverse the direction:
  if (ballX > EsploraTFT.width() || ballX < 0) {
    ballDirectionX = -ballDirectionX;
  }

  if (ballY > EsploraTFT.height() || ballY < 0) {
    ballDirectionY = -ballDirectionY;
  }  

  // check if the ball and the paddle occupy the same space on screen
  if (inPaddle(ballX, ballY, paddleX, paddleY, 20, 5)) {
    ballDirectionY = -ballDirectionY;
  }

  // update the ball's position
  ballX += ballDirectionX;
  ballY += ballDirectionY;

  // erase the ball's previous position
  EsploraTFT.fill(0,0,0);

  if (oldBallX != ballX || oldBallY != ballY) {
    EsploraTFT.rect(oldBallX, oldBallY, 5, 5);
  }

  // draw the ball's current position
  EsploraTFT.fill(255,255,255);

  EsploraTFT.rect(ballX, ballY, 5, 5);

  oldBallX = ballX;
  oldBallY = ballY;

}

// this function checks the position of the ball
// to see if it intersects with the paddle
boolean inPaddle(int x, int y, int rectX, int rectY, int rectWidth, int rectHeight) {
  boolean result = false;

  if ((x >= rectX && x <= (rectX + rectWidth)) && 
    (y >= rectY && y <= (rectY + rectHeight))) {
    result = true; 
  }

  return result;  
}
