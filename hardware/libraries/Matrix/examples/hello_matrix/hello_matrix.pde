#include <Sprite.h>
#include <Matrix.h>

// Hello Matrix
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates the use of the Matrix library
// For MAX7219 LED Matrix Controllers
// Blinks welcoming face on screen

// Created 13 February 2006

/* create a new Matrix instance
   pin 0: data  (din)
   pin 1: load  (load)
   pin 2: clock (clk)
*/
Matrix myMatrix = Matrix(0, 2, 1);

void setup()
{
}

void loop()
{
  myMatrix.clear(); // clear display

  delay(1000);

  // turn some pixels on
  myMatrix.write(1, 5, HIGH);
  myMatrix.write(2, 2, HIGH);
  myMatrix.write(2, 6, HIGH);
  myMatrix.write(3, 6, HIGH);
  myMatrix.write(4, 6, HIGH);
  myMatrix.write(5, 2, HIGH);
  myMatrix.write(5, 6, HIGH);
  myMatrix.write(6, 5, HIGH);

  delay(1000);
}

