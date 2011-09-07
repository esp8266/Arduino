#define ARDUINO_MAIN
#include <Arduino.h>

int main(void)
{
	init();

	setup();
    
	for (;;) {
		loop();
		if (serialEventRun) serialEventRun();
	}
        
	return 0;
}

