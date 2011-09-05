#define ARDUINO_MAIN
#include <Arduino.h>

int main(void)
{
	init();

#if defined(USBCON)
	USB.attach();
#endif
	
	setup();
    
	for (;;) {
		loop();
		serialEventRun();
	}
        
	return 0;
}

