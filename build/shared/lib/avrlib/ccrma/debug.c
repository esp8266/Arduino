
#include <io.h>
#include <stdarg.h>

#include "debug.h"
#include "lcd.h"
#include "rprintf.h"
#include "timer.h"
#include "osc.h"


u08 debugMode = 0;
u08 lcdDebugX;
u08 lcdDebugY;

void debugInitLCD(u08 x, u08 y) {
	lcdInit();
	lcdClear();

	lcdDebugX = x;
	lcdDebugY = y;

	debugMode |= DEBUG_MODE_LCD;
	
	debug(PSTR("LCD Debug init()"));
}

void debugInitOSC(void) {
	oscInit();
	debugMode |= DEBUG_MODE_OSC;
}

void debug(const char PROGMEM *fmt) {
	int code;

	if (debugMode & DEBUG_MODE_OSC) {
		oscSendMessageString("/debug",fmt);
	}
	if (debugMode & DEBUG_MODE_LCD) {
		rprintfInit(&lcdDataWrite);
		lcdGotoXY(lcdDebugX,lcdDebugY);
		rprintf1RamRom(STRING_IN_ROM, fmt);
	}

}

// debugFlush assumes that timerInit() have been called already
void debugFlash(const u08 port, const u08 pin) {
	sbi(DDR(port), pin);
	cbi(port, pin);
	timerPause(500);
	sbi(port, pin);
}


