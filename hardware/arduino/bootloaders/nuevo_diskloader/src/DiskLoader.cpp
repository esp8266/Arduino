/*
 *  DiskLoader.cpp
 */

#include "Platform.h"


int main() 
{
	wdt_disable();
	BOARD_INIT();
	TX_LED_OFF();
	RX_LED_OFF();
	L_LED_OFF();
//	USB.attach();
	
	for (;;) 
	{
		
	}
}
