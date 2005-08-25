

#ifndef _DEBUG_H
#define _DEBUG_H

#include <progmem.h>
#include "global.h"

#define DEBUG_MODE_LCD		0x01
#define DEBUG_MODE_SERIAL	0x02
#define DEBUG_MODE_OSC		0x04

void debugInitLCD(u08 x, u08 y);

void debugInitOSC(void);

void debug(const char * fmt);

void debugFlash(u08 port, u08 pin);

#endif

