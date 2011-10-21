/*
 *  DiskLoader.cpp
 */

#include "Platform.h"

//extern "C"
void entrypoint(void) __attribute__ ((naked)) __attribute__ ((section (".vectors")));
void entrypoint(void)
{
	asm volatile (
//				  "eor	r1,		r1\n"	// Zero register
//				  "out	0x3F,	r1\n"	// SREG
//				  "ldi	r28,	0xFF\n"
//				  "ldi	r29,	0x0A\n"
//				  "out	0x3E,	r29\n"	// SPH
//				  "out	0x3D,	r28\n"	// SPL
				  "rjmp	main"			// Stack is all set up, start the main code
				  ::);
}

u8 _flashbuf[128];
volatile u16 _timeout;

void Program(u8 ep, u16 page, u8 count)
{
	u8 write = page < 30*1024;		// Don't write over firmware please
	if (write)
		boot_page_erase(page);
	
	USB_Recv(ep,_flashbuf,count);	// Read while page is erasing
	
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


int main(void) __attribute__ ((naked));
int main() 
{		
	wdt_disable();
	BOARD_INIT();
	/* move interrupts to boot section:
	 * uses inline assembly because the procedure must be completed in four cycles.
	 * seems to fail if called before disabling WDT and calling BOARD_INIT()
	 */
	asm volatile (
				  "ldi r16,	  0x01\n"		// (1<<IVCE)	/* Enable change of interrupt vectors */
				  "out 0x35,  r16\n"		// MCUCR
				  "ldi r16,	  0x02\n"		// (1<<IVSEL)	/* Move interrupts to boot flash section */
				  "out 0x35,  r16\n"		// MCUCR				  
				  );	
	TX_LED_OFF();
	RX_LED_OFF();
	L_LED_OFF();	
	USB.attach();
	sei();
	
	for (;;) 
	{
		u8* packet = _flashbuf;
		u16 address = 0;
		for (;;)
		{
			if (Serial.available() > 0) {
				u8 cmd = Serial.read();
				// Read packet contents
				u8 len;
				const u8* rs = _readSize;
				for (;;) 
				{
					u8 c = pgm_read_byte(rs++);
					len = pgm_read_byte(rs++);
					if (c == cmd || c == 0)
						break;
				}
				_timeout = 0;
				// Read params
				USB_Recv(CDC_RX, packet, len);
				
			}
		}
	}
}
