
#include <Arduino.h>

#include <unistd.h> // usleep

int main (void)
{
	setup();
	while (true)
	{
		usleep(10000); // not 100% cpu
		loop();
	}
	return 0;
}

