#define ARDUINO_MAIN
#include <Arduino.h>

int main(void)
{
	init();

	setup();
    
	for (;;) {
		loop();
		serialEventRun();
	}
        
	return 0;
}

