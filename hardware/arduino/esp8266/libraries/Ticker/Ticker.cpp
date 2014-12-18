#include <stddef.h>
#include <stdint.h>

extern "C" {
#include "c_types.h"
#include "eagle_soc.h"
#include "ets_sys.h"
#include "osapi.h"
}

#include "Ticker.h"

Ticker::Ticker()
: _timer(0)
{
}

Ticker::~Ticker()
{
	detach();
}

void Ticker::_attach_ms(callback_with_arg_t callback, uint32_t milliseconds, void* arg)
{
	const int REPEAT = 1;

	if (_timer)
	{
		os_timer_disarm(_timer);
	}
	else
	{
		_timer = new ETSTimer;
	}

	os_timer_setfn(_timer, reinterpret_cast<ETSTimerFunc*>(callback), arg);
	os_timer_arm(_timer, milliseconds, REPEAT);
}

void Ticker::detach()
{
	if (!_timer)
		return;

	os_timer_disarm(_timer);
	delete _timer;
	_timer = 0;
}
