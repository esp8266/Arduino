
#include <Arduino.h>

void pinMode (uint8_t pin, uint8_t mode)
{
	#define xxx(mode) case mode: m=STRHELPER(mode); break
	const char* m;
	switch (mode)
	{
	case INPUT: m="INPUT"; break;
	case OUTPUT: m="OUTPUT"; break;
	case INPUT_PULLUP: m="INPUT_PULLUP"; break;
	case OUTPUT_OPEN_DRAIN: m="OUTPUT_OPEN_DRAIN"; break;
	case INPUT_PULLDOWN_16: m="INPUT_PULLDOWN_16"; break;
	case WAKEUP_PULLUP: m="WAKEUP_PULLUP"; break;
	case WAKEUP_PULLDOWN: m="WAKEUP_PULLDOWN"; break;
	default: m="(special)";
	}
	fprintf(stderr, MOCK "gpio%d: mode='%s'\n", pin, m);
}

void digitalWrite(uint8_t pin, uint8_t val)
{
	fprintf(stderr, MOCK "digitalWrite(pin=%d val=%d)\n", pin, val);
}

