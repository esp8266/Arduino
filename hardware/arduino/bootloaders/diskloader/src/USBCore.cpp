

/* Copyright (c) 2010, Peter Barrett  
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

#define CDC_TX CDC_ENDPOINT_IN
#define CDC_RX CDC_ENDPOINT_OUT

#define EP_TYPE_CONTROL				0x00
#define EP_TYPE_BULK_IN				0x81
#define EP_TYPE_BULK_OUT			0x80
#define EP_TYPE_INTERRUPT_IN		0xC1
#define EP_TYPE_INTERRUPT_OUT		0xC0
#define EP_TYPE_ISOCHRONOUS_IN		0x41
#define EP_TYPE_ISOCHRONOUS_OUT		0x40

/** Pulse generation counters to keep track of the number of milliseconds remaining for each pulse type */
#define TX_RX_LED_PULSE_MS 100
u8 TxLEDPulse; /**< Milliseconds remaining for data Tx LED pulse */
u8 RxLEDPulse; /**< Milliseconds remaining for data Rx LED pulse */

void Reboot();

//==================================================================
//==================================================================

typedef struct
{
	u32	dwDTERate;
	u8	bCharFormat;
	u8 	bParityType;
	u8 	bDataBits;
	u8	lineState;
} LineInfo;

static volatile LineInfo _usbLineInfo = { 57600, 0x00, 0x00, 0x00, 0x00 };

//==================================================================
//==================================================================

//	4 bytes of RAM
volatile u8 _usbConfiguration;
volatile u8 _ejected;
volatile u16 _timeout;

static inline void WaitIN(void)
{
	while (!(UEINTX & (1<<TXINI)));
}

static inline void ClearIN(void)
{
	UEINTX = ~(1<<TXINI);
}

static inline void WaitOUT(void)
{
	while (!(UEINTX & (1<<RXOUTI)))
		;
}

static inline u8 WaitForINOrOUT()
{
	while (!(UEINTX & ((1<<TXINI)|(1<<RXOUTI))))
		;
	return (UEINTX & (1<<RXOUTI)) == 0;
}

static inline void ClearOUT(void)
{
	UEINTX = ~(1<<RXOUTI);
}

static
void Send(volatile const u8* data, u8 count)
{
	TXLED1;					// light the TX LED
	TxLEDPulse = TX_RX_LED_PULSE_MS;
	while (count--)
		UEDATX = *data++;
}

void Recv(volatile u8* data, u8 count)
{
	RXLED1;					// light the RX LED
	RxLEDPulse = TX_RX_LED_PULSE_MS;
	while (count--)
		*data++ = UEDATX;
}

static inline u8 Recv8()
{
	RXLED1;					// light the RX LED
	RxLEDPulse = TX_RX_LED_PULSE_MS;
	return UEDATX;
}

static inline void Send8(u8 d)
{
	TXLED1;					// light the TX LED
	TxLEDPulse = TX_RX_LED_PULSE_MS;
	UEDATX = d;
}

static inline void SetEP(u8 ep)
{
	UENUM = ep;
}

static inline u8 FifoByteCount()
{
	return UEBCLX;
}

static inline u8 ReceivedSetupInt()
{
	return UEINTX & (1<<RXSTPI);
}

static inline void ClearSetupInt()
{
	UEINTX = ~((1<<RXSTPI) | (1<<RXOUTI) | (1<<TXINI));
}

static inline void Stall()
{
	UECONX = (1<<STALLRQ) | (1<<EPEN);
}

static inline u8 ReadWriteAllowed()
{
	return UEINTX & (1<<RWAL);
}

static inline u8 Stalled()
{
	return UEINTX & (1<<STALLEDI);
}

static inline u8 FifoFree()
{
	return UEINTX & (1<<FIFOCON);
}

static inline void ReleaseRX()
{
	UEINTX = 0x6B;	// FIFOCON=0 NAKINI=1 RWAL=1 NAKOUTI=0 RXSTPI=1 RXOUTI=0 STALLEDI=1 TXINI=1
}

static inline void ReleaseTX()
{
	UEINTX = 0x3A;	// FIFOCON=0 NAKINI=0 RWAL=1 NAKOUTI=1 RXSTPI=1 RXOUTI=0 STALLEDI=1 TXINI=0
}

static inline u8 FrameNumber()
{
	return UDFNUML;
}

//==================================================================
//==================================================================

#define EP_SINGLE_64 0x32	// EP0
#define EP_DOUBLE_64 0x36	// Other endpoints

static void InitEP(u8 index, u8 type, u8 size)
{
	UENUM = index;
	UECONX = 1;
	UECFG0X = type;
	UECFG1X = size;
}

//	API
void USBInit(void)
{
	_timeout = 0;
	_usbConfiguration = 0;
	_ejected = 0;
	
	UHWCON = 0x01;						// power internal reg (don't need this?)
	USBCON = (1<<USBE)|(1<<FRZCLK);		// clock frozen, usb enabled
	PLLCSR = 0x12;						// Need 16 MHz xtal
	while (!(PLLCSR & (1<<PLOCK)))		// wait for lock pll
		;
	USBCON = ((1<<USBE)|(1<<OTGPADE));	// start USB clock
	UDCON = 0;							// enable attach resistor
}

u8 USBGetConfiguration(void)
{
	return _usbConfiguration;
}

u8 HasData(u8 ep)
{
	SetEP(ep);
	return ReadWriteAllowed();	// count in fifo
}

int USBGetChar();
void Recv(u8 ep, u8* dst, u8 len)
{
	SetEP(ep);
	while (len--)
	{
		while (!ReadWriteAllowed())
			;
		*dst++ = Recv8();
		if (!ReadWriteAllowed())	// release empty buffer
			ReleaseRX();
	}
}

//	Transmit a packet to endpoint
void Transfer(u8 ep, const u8* data, int len)
{
	u8 zero = ep & TRANSFER_ZERO;
	SetEP(ep & 7);
	while (len--)
	{
		while (!ReadWriteAllowed())
			;	// TODO Check for STALL etc

		u8 d = (ep & TRANSFER_PGM) ? pgm_read_byte(data) : data[0];
		data++;
		if (zero)
			d = 0;
		Send8(d);

		if (!ReadWriteAllowed())
			ReleaseTX();
	}
	if (ep & TRANSFER_RELEASE)
		ReleaseTX();
}

extern const u8 _initEndpoints[] PROGMEM;
const u8 _initEndpoints[] = 
{
	0,

#ifdef CDC_ENABLED
	EP_TYPE_INTERRUPT_IN,		// CDC_ENDPOINT_ACM
	EP_TYPE_BULK_OUT,			// CDC_ENDPOINT_OUT
	EP_TYPE_BULK_IN,			// CDC_ENDPOINT_IN
#endif

	EP_TYPE_INTERRUPT_IN,		// HID_ENDPOINT_INT
};

static void InitEndpoints()
{
	for (u8 i = 1; i < sizeof(_initEndpoints); i++)
	{
		UENUM = i;
		UECONX = 1;
		UECFG0X = pgm_read_byte(_initEndpoints+i);
		UECFG1X = EP_DOUBLE_64;
	}
	UERST = 0x7E;	// And reset them
	UERST = 0;
}

typedef struct
{
	u8 bmRequestType;
	u8 bRequest;
	u8 wValueL;
	u8 wValueH;
	u16 wIndex;
	u16 wLength;
} Setup;
Setup _setup;

//bool USBHook(Setup& setup)
bool USBHook()
{
	Setup& setup = _setup;
	u8 r = setup.bRequest;

	//	CDC Requests
	if (CDC_GET_LINE_CODING == r)
	{
		Send((const volatile u8*)&_usbLineInfo,7);
	}

	else if (CDC_SET_LINE_CODING ==  r)
	{
		WaitOUT();
		Recv((volatile u8*)&_usbLineInfo,7);
		ClearOUT();
	}

	else if (CDC_SET_CONTROL_LINE_STATE == r)
	{
		_usbLineInfo.lineState = setup.wValueL;
	}

	return true;
}

extern const u8 _rawHID[] PROGMEM;
#define LSB(_x) ((_x) & 0xFF)
#define MSB(_x) ((_x) >> 8)

#define RAWHID_USAGE_PAGE	0xFFC0
#define RAWHID_USAGE		0x0C00
#define RAWHID_TX_SIZE 64
#define RAWHID_RX_SIZE 64

const u8 _rawHID[] =
{
	//	RAW HID
	0x06, LSB(RAWHID_USAGE_PAGE), MSB(RAWHID_USAGE_PAGE),	// 30
	0x0A, LSB(RAWHID_USAGE), MSB(RAWHID_USAGE),

	0xA1, 0x01,				// Collection 0x01
    0x85, 0x03,             // REPORT_ID (3)
	0x75, 0x08,				// report size = 8 bits
	0x15, 0x00,				// logical minimum = 0
	0x26, 0xFF, 0x00,		// logical maximum = 255

	0x95, 64,				// report count TX
	0x09, 0x01,				// usage
	0x81, 0x02,				// Input (array)

	0x95, 64,				// report count RX
	0x09, 0x02,				// usage
	0x91, 0x02,				// Output (array)
	0xC0					// end collection
};

u8 _cdcComposite = 0;

bool SendDescriptor()
{
	Setup& setup = _setup;
	u8 desc_length = 0;
	const u8* desc_addr = 0;

	u8 t = setup.wValueH;
	if (0x22 == t)
	{
		desc_addr = _rawHID;
		desc_length = sizeof(desc_length);
	} else if (USB_DEVICE_DESCRIPTOR_TYPE == t)
	{
		if (setup.wLength == 8)
			_cdcComposite = 1;
		desc_addr = _cdcComposite ?  (const u8*)&USB_DeviceDescriptorA : (const u8*)&USB_DeviceDescriptor;
	}
	else if (USB_CONFIGURATION_DESCRIPTOR_TYPE == t)
	{
		desc_addr = (const u8*)&USB_ConfigDescriptor;
		desc_length = sizeof(USB_ConfigDescriptor);
	}
	else if (USB_STRING_DESCRIPTOR_TYPE == t)
	{
		if (setup.wValueL == 0)
			desc_addr = (const u8*)&STRING_LANGUAGE;
		else
			return false;
	} else 
		return false;

	if (desc_length == 0)
		desc_length = pgm_read_byte(desc_addr);
	if ((u8)setup.wLength < desc_length)		// bit of a cheat limiting to 255 bytes TODO (saved 8 bytes)
		desc_length = (u8)setup.wLength;

	//	Send descriptor
	//	EP0 is 64 bytes long
	//	RWAL and FIFOCON don't work on EP0
	u8 n = 0;
	do
	{
		if (!WaitForINOrOUT())
			return false;
		Send8(pgm_read_byte(&desc_addr[n++]));
		u8 clr = n & 0x3F;
		if (!clr)
			ClearIN();	// Fifo is full, release this packet
	} while (n < desc_length);
	return true;
}

void USBSetupInterrupt()
{
    SetEP(0);
	if (!ReceivedSetupInt())
		return;

	Setup& setup = _setup;	// global saves ~30 bytes
	Recv((u8*)&setup,8);
	ClearSetupInt();

	if (setup.bmRequestType & DEVICETOHOST)
		WaitIN();
	else
		ClearIN();

    bool ok = true;
	u8 r = setup.bRequest;
	if (SET_ADDRESS == r)
	{
		WaitIN();
		UDADDR = setup.wValueL | (1<<ADDEN);
	}
	else if (SET_CONFIGURATION == r)
	{
		_usbConfiguration = setup.wValueL;
		InitEndpoints();
	}
	else if (GET_CONFIGURATION == r)
	{
		Send8(_usbConfiguration);
	}
	else if (GET_STATUS == r)
	{
		Send8(0);		// All good as far as I know
	}
	else if (GET_DESCRIPTOR == r)
	{
		ok = SendDescriptor();
	}
	else
	{
		ok = USBHook();
	}

	if (ok)
		ClearIN();
	else
		Stall();
}

void USBGeneralInterrupt()
{
	u8 udint = UDINT;
	UDINT = 0;

	//	End of Reset
	if (udint & (1<<EORSTI))
	{
		InitEP(0,EP_TYPE_CONTROL,EP_SINGLE_64);	// init ep0
		_usbConfiguration = 0;			// not configured yet
	}

	//	Start of Frame - happens every millisecond so we use it for TX and RX LED one-shot timing, too
	if (udint & (1<<SOFI))
	{
		// check whether the one-shot period has elapsed.  if so, turn off the LED
		if (TxLEDPulse && !(--TxLEDPulse))
			TXLED0;
		if (RxLEDPulse && !(--RxLEDPulse))
			RXLED0;
		
		if (!_ejected)
			_timeout = 0;
	}
}

void LEDPulse();
int USBGetChar()
{
	for(;;)
	{
		USBSetupInterrupt();
		USBGeneralInterrupt();

		//	Read a char
		if (HasData(CDC_RX))
		{
			u8 c = Recv8();
			if (!ReadWriteAllowed())
				ReleaseRX();
			return c;
		} 

		if (!--_timeout) {
			Reboot();	// USB not connected, run firmware
		}
		
		_delay_us(100);	// stretch out the bootloader period to about 5 seconds after enumeration
		LEDPulse();
	}
	return -1;
}
