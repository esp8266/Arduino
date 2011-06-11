#define ARDUINO_MAIN
#include <Arduino.h>

int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

