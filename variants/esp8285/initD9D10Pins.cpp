#include "Arduino.h"

// The 8285 allows the use of GPIO pins 9 and 10, so set them to inputs
// on startup just like the other pins.  This allows their use for interrupts
// as well
void initVariant() {
  pinMode(9, INPUT);
  pinMode(10, INPUT);
}

