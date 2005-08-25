//  osc.c
//
//  Open Sound Control message sending fn's for avrmini
//  
//  Scott Wilson
//  July 21, 2002
//

#include <progmem.h>
#include <stdarg.h>
#include "OSC-client.h"
#include "osc.h"
//#include "debug.h"
#include "uart.h"

#define OSC_BUFFER_LEN 40

void _oscSendPacket();

u08    oscDataBuffer[OSC_BUFFER_LEN];
OSCbuf oscbuf;

void oscInit() {
	uartInit();
	OSC_initBuffer(&oscbuf, OSC_BUFFER_LEN, oscDataBuffer);
//	debug(PSTR("OSC init ok packet"));
}


// call oscInit() and uartInit() before using this function
void oscSendMessage(const char PROGMEM *address) {
	OSC_writeAddress(&oscbuf, address);

	_oscSendPacket();
}

void oscSendMessageInt(const char PROGMEM *address, s32 arg) {
	OSC_writeAddress(&oscbuf, address);

	OSC_writeIntArg(&oscbuf, arg);

	_oscSendPacket();
}

void oscSendMessageIntInt(const char PROGMEM *address, s32 arg, s32 arg2) {
	OSC_writeAddress(&oscbuf, address);

	OSC_writeIntArg(&oscbuf, arg);
	OSC_writeIntArg(&oscbuf, arg2);

	_oscSendPacket();
}

void oscSendMessageString(const char PROGMEM *address, const char PROGMEM *arg) {
	OSC_writeAddress(&oscbuf, address);

	OSC_writeStringArg(&oscbuf, arg);

	_oscSendPacket();
}


void _oscSendPacket() {
	u08 j;
	u08 *oscDataPtr;	
	u08 oscPacketSize;
	register u08 checksum=0;
	register u08 data;

	// send the packet
	if (OSC_isBufferDone(&oscbuf)) {
		// begin packet sync byte
		uartSendByte((u08)0xbe);

		// send length byte
		uartSendByte((u08)(OSC_BUFFER_LEN - OSC_freeSpaceInBuffer(&oscbuf)));

		oscDataPtr = OSC_getPacket(&oscbuf);
		oscPacketSize = OSC_packetSize(&oscbuf);
//			debug(PSTR("packet size: %x"),(unsigned int)oscPacketSize);
		for (j=0; j<oscPacketSize; j++)  {
			data = *(oscDataPtr+j);
			checksum += data;
			uartSendByte(data);
		}
		// send checksum byte
		uartSendByte(checksum);
		OSC_resetBuffer(&oscbuf);
	} else {
		//debug(PSTR("Error creating OSC packet"));
	}
}




