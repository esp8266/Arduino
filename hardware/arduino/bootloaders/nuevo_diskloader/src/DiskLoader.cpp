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
		L_LED_ON();
		_delay_ms(250);
		L_LED_OFF();
		_delay_ms(250);
	}
}
