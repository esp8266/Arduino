

/* Copyright (c) 2011, Peter Barrett  
**  
** Permission to use, copy, modify, and/or distribute this software for  
** any purpose with or without fee is hereby granted, provided that the  
** above copyright notice and this permission notice appear in all copies.  
** 
** THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL  
** WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED  
** WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR  
** BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES  
** OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,  
** WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  
** ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS  
** SOFTWARE.  
*/

#include "Platform.h"
#include "USBAPI.h"
#include <avr/wdt.h>

#if defined(USBCON)
#ifdef CDC_ENABLED

void Reboot()
{
	USB.detach();
	cli();
	asm volatile("jmp 0x7800");		// jump to bootloader - DiskLoader takes up last 2 kB
}

typedef struct
{
	u32	dwDTERate;
	u8	bCharFormat;
	u8 	bParityType;
	u8 	bDataBits;
	u8	lineState;
} LineInfo;

static volatile LineInfo _usbLineInfo = { 57600, 0x00, 0x00, 0x00, 0x00 };

#define WEAK __attribute__ ((weak))

extern const CDCDescriptor _cdcInterface PROGMEM;
const CDCDescriptor _cdcInterface =
{
	D_IAD(0,2,CDC_COMMUNICATION_INTERFACE_CLASS,CDC_ABSTRACT_CONTROL_MODEL,1),

	//	CDC communication interface
	D_INTERFACE(CDC_ACM_INTERFACE,1,CDC_COMMUNICATION_INTERFACE_CLASS,CDC_ABSTRACT_CONTROL_MODEL,0),
	D_CDCCS(CDC_HEADER,0x10,0x01),								// Header (1.10 bcd)
	D_CDCCS(CDC_CALL_MANAGEMENT,1,1),							// Device handles call management (not)
	D_CDCCS4(CDC_ABSTRACT_CONTROL_MANAGEMENT,6),				// SET_LINE_CODING, GET_LINE_CODING, SET_CONTROL_LINE_STATE supported
	D_CDCCS(CDC_UNION,CDC_ACM_INTERFACE,CDC_DATA_INTERFACE),	// Communication interface is master, data interface is slave 0
	D_ENDPOINT(USB_ENDPOINT_IN (CDC_ENDPOINT_ACM),USB_ENDPOINT_TYPE_INTERRUPT,0x10,0x40),

	//	CDC data interface
	D_INTERFACE(CDC_DATA_INTERFACE,2,CDC_DATA_INTERFACE_CLASS,0,0),
	D_ENDPOINT(USB_ENDPOINT_OUT(CDC_ENDPOINT_OUT),USB_ENDPOINT_TYPE_BULK,0x40,0),
	D_ENDPOINT(USB_ENDPOINT_IN (CDC_ENDPOINT_IN ),USB_ENDPOINT_TYPE_BULK,0x40,0)
};

int WEAK CDC_GetInterface(u8* interfaceNum)
{
	interfaceNum[0] += 2;	// uses 2
	return USB_SendControl(TRANSFER_PGM,&_cdcInterface,sizeof(_cdcInterface));
}

bool WEAK CDC_Setup(Setup& setup)
{
	u8 r = setup.bRequest;
	u8 requestType = setup.bmRequestType;

	if (REQUEST_DEVICETOHOST_CLASS_INTERFACE == requestType)
	{
		if (CDC_GET_LINE_CODING == r)
		{
			USB_SendControl(0,(void*)&_usbLineInfo,7);
			return true;
		}
	}

	if (REQUEST_HOSTTODEVICE_CLASS_INTERFACE == requestType)
	{
		if (CDC_SET_LINE_CODING == r)
		{
			USB_RecvControl((void*)&_usbLineInfo,7);
			return true;
		}

		if (CDC_SET_CONTROL_LINE_STATE == r)
		{
			if (0 != _usbLineInfo.lineState && 1200 == _usbLineInfo.dwDTERate)	// auto-reset is triggered when the port, already open at 1200 bps, is closed
				Reboot();
			_usbLineInfo.lineState = setup.wValueL;
			return true;
		}
	}
	return false;
}


int _serialPeek = -1;
void Serial_::begin(uint16_t baud_count)
{
}

void Serial_::end(void)
{
}

int Serial_::available(void)
{
	u8 avail = USB_Available(CDC_RX);
	if (_serialPeek != -1)
		avail++;
	return avail;
}

//	peek is nasty
int Serial_::peek(void)
{
	if (_serialPeek == -1)
		_serialPeek = read();
	return _serialPeek;
}

int Serial_::read(void)
{
	int c;
	if (_serialPeek != -1)
	{
		c = _serialPeek;
		_serialPeek = -1;
	} else {
		c = USB_Recv(CDC_RX);
	}
	return c;
}

void Serial_::flush(void)
{
	USB_Flush(CDC_TX);
}

size_t Serial_::write(uint8_t c)
{
	/* only try to send bytes if the high-level CDC connection itself 
	 is open (not just the pipe) - the OS should set lineState when the port
	 is opened and clear lineState when the port is closed.
	 bytes sent before the user opens the connection or after
	 the connection is closed are lost - just like with a UART. */
	
	// TODO - ZE - check behavior on different OSes and test what happens if an
	// open connection isn't broken cleanly (cable is yanked out, host dies
	// or locks up, or host virtual serial port hangs)
	if (_usbLineInfo.lineState > 0)	{
		int r = USB_Send(CDC_TX,&c,1);
		if (r > 0) {
			return r;
		} else {
			setWriteError();
			return 0;
		}
	}
	setWriteError();
	return 0;
}

Serial_ Serial;

#endif
#endif /* if defined(USBCON) */