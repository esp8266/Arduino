

#include "Platform.h"

//	This bootloader creates a composite Serial device
//
//	The serial interface supports a STK500v1 protocol that is very similar to optiboot
//
//	The bootloader will timeout and start the firmware after a few hundred milliseconds
//	if a usb connection is not detected.
//	
//	The tweakier code is to keep the bootloader below 2k (no interrupt table, for example)

extern "C"
void entrypoint(void) __attribute__ ((naked)) __attribute__ ((section (".vectors")));
void entrypoint(void)
{
	asm volatile (
		"eor	r1,		r1\n"	// Zero register
		"out	0x3F,	r1\n"	// SREG
		"ldi	r28,	0xFF\n"
		"ldi	r29,	0x0A\n"
		"out	0x3E,	r29\n"	// SPH
		"out	0x3D,	r28\n"	// SPL
		"rjmp	main"			// Stack is all set up, start the main code
		::);
}

u8 _flashbuf[128];
u8 _inSync;
u8 _ok;
extern volatile u8 _ejected;
extern volatile u16 _timeout;

void Program(u8 ep, u16 page, u8 count)
{
	u8 write = page < 30*1024;		// Don't write over firmware please
	if (write)
		boot_page_erase(page);

	Recv(ep,_flashbuf,count);		// Read while page is erasing

	if (!write)
		return;

	boot_spm_busy_wait();			// Wait until the memory is erased.

	count >>= 1;
	u16* p = (u16*)page;
	u16* b = (u16*)_flashbuf;
	for (u8 i = 0; i < count; i++)
		boot_page_fill(p++, b[i]);

    boot_page_write(page);
    boot_spm_busy_wait();
    boot_rww_enable ();
}


int USBGetChar();
#define getch USBGetChar

#define HW_VER	 0x02
#define SW_MAJOR 0x01
#define SW_MINOR 0x10

#define STK_OK              0x10
#define STK_INSYNC          0x14  // ' '
#define CRC_EOP             0x20  // 'SPACE'
#define STK_GET_SYNC        0x30  // '0'

#define STK_GET_PARAMETER   0x41  // 'A'
#define STK_SET_DEVICE      0x42  // 'B'
#define STK_SET_DEVICE_EXT  0x45  // 'E'
#define STK_LOAD_ADDRESS    0x55  // 'U'
#define STK_UNIVERSAL       0x56  // 'V'
#define STK_PROG_PAGE       0x64  // 'd'
#define STK_READ_PAGE       0x74  // 't'
#define STK_READ_SIGN       0x75  // 'u'

extern const u8 _readSize[] PROGMEM;
const u8 _readSize[] = 
{
	STK_GET_PARAMETER,	1,
	STK_SET_DEVICE,		20,
	STK_SET_DEVICE_EXT,	5,
	STK_UNIVERSAL,		4,
	STK_LOAD_ADDRESS,	2,
	STK_PROG_PAGE,		3,
	STK_READ_PAGE,		3,
	0,0
};

extern const u8 _consts[] PROGMEM;
const u8 _consts[] = 
{
	SIGNATURE_0,
	SIGNATURE_1,
	SIGNATURE_2,
	HW_VER,		// Hardware version
	SW_MAJOR,	// Software major version
	SW_MINOR,	// Software minor version
	0x03,		// Unknown but seems to be required by avr studio 3.56
	0x00,		// 
};


void USBInit(void);
int main(void) __attribute__ ((naked));

//	STK500v1 main loop, very similar to optiboot in protocol and implementation
int main()
{
	wdt_disable();
	TXLED0;
	RXLED0;
	LED0;
	BOARD_INIT();
	USBInit();

	_inSync = STK_INSYNC;
	_ok = STK_OK;

	if (pgm_read_word(0) != -1)
		_ejected = 1;

	for(;;)
	{
		u8* packet = _flashbuf;
		u16 address = 0;
		for (;;)
		{
			u8 cmd = getch();

			//	Read packet contents
			u8 len;
			const u8* rs = _readSize;
			for(;;)
			{
				u8 c = pgm_read_byte(rs++);
				len = pgm_read_byte(rs++);
				if (c == cmd || c == 0)
					break;
			}
			_timeout = 0;
			//	Read params
			Recv(CDC_RX,packet,len);

			//	Send a response
			u8 send = 0;
			const u8* pgm = _consts+7;			// 0
			if (STK_GET_PARAMETER == cmd)
			{
				u8 i = packet[0] - 0x80;
				if (i > 2)
					i = (i == 0x18) ? 3 : 4;	// 0x80:HW_VER,0x81:SW_MAJOR,0x82:SW_MINOR,0x18:3 or 0
				pgm = _consts + i + 3;
				send = 1;
			}

			else if (STK_UNIVERSAL == cmd)
			{
				if (packet[0] == 0x30)
					pgm = _consts + packet[2];	// read signature
				send = 1;
			}
			
			//	Read signature bytes
			else if (STK_READ_SIGN == cmd)
			{
				pgm = _consts;
				send = 3;
			}

			else if (STK_LOAD_ADDRESS == cmd)
			{
				address = *((u16*)packet);		// word addresses
				address += address;
			}

			else if (STK_PROG_PAGE == cmd)
			{
				Program(CDC_RX,address,packet[1]);
			}

			else if (STK_READ_PAGE == cmd)
			{
				send = packet[1];
				pgm = (const u8*)address;
				address += send; // not sure of this is required
			}

			// Check sync
			if (getch() != ' ')
				break;
			Transfer(CDC_TX,&_inSync,1);

			// Send result
			if (send)
				Transfer(CDC_TX|TRANSFER_PGM,pgm,send);	// All from pgm memory

			//	Send ok
			Transfer(CDC_TX|TRANSFER_RELEASE,&_ok,1);

			if (cmd == 'Q')
				break;
		}
		_timeout = 500;		// wait a moment before exiting the bootloader - may need to finish responding to 'Q' for example
		_ejected = 1;
	}
}

//	Nice breathing LED indicates we are in the firmware
u16 _pulse;
void LEDPulse()
{
	_pulse += 4;
	u8 p = _pulse >> 9;
	if (p > 63)
		p = 127-p;
	p += p;
	if (((u8)_pulse) > p)
		LED0;
	else
		LED1;
}

void Reboot()
{
	TXLED0;		// switch off the RX and TX LEDs before starting the user sketch
	RXLED0;
	UDCON = 1;		// Detatch USB
	UDIEN = 0;
	asm volatile (	// Reset vector to run firmware
		"clr r30\n"
		"clr r31\n"
		"ijmp\n"
	::);
}
