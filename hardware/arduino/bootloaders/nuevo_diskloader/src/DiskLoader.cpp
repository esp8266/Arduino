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

ISR(BADISR_vect)
{
	L_LED_ON();
}

int main(void) __attribute__ ((naked));
int main() 
{
	/* Enable change of interrupt vectors */ 
	MCUCR = (1<<IVCE); 
	/* Move interrupts to boot flash section */ 
	MCUCR = (1<<IVSEL); 
	
	wdt_disable();
	BOARD_INIT();
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
