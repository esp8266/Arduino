

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

#ifdef CDC_ENABLED

//	TODO: Should really use the wdt here
//	Not currently working for a non-obvious reason

typedef void (*AppPtr_t)(void) __attribute__ ((noreturn)); 
AppPtr_t Bootloader = (AppPtr_t)(30*1024); 

void Reboot()
{
	USB.detach();
	cli();
	Bootloader();
	//wdt_enable(WDTO_15MS);
	//while(1);	// reboot
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
			_usbLineInfo.lineState = setup.wValueL;
//			if (_usbLineInfo.dwDTERate == 115200 && _usbLineInfo.lineState == 0)	// Emulate DTR reset hack
			if (_usbLineInfo.lineState == 0)
				Reboot();
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

void Serial_::write(uint8_t c)
{
	USB_Send(CDC_TX,&c,1);
}

Serial_ Serial;

#endif
