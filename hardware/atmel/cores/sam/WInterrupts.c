#include "board.h"

#include <inttypes.h>
#include <stdio.h>

#include "wiring_private.h"

volatile static voidFuncPtr intFunc[EXTERNAL_NUM_INTERRUPTS];

void attachInterrupt(uint8_t interruptNum, void (*userFunc)(void), int mode) {
  if(interruptNum < EXTERNAL_NUM_INTERRUPTS)
  {
    intFunc[interruptNum] = userFunc;
  }
}

void detachInterrupt( uint8_t interruptNum )
{
  if(interruptNum < EXTERNAL_NUM_INTERRUPTS)
  {
    intFunc[interruptNum] = 0;
  }
}
