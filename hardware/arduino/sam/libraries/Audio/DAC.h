
#ifndef DAC_INCLUDED
#define DAC_INCLUDED

#include "Arduino.h"

typedef void (*OnQueueFree_CB)(void);

class DACClass
{
public:
	DACClass(Dacc *_dac, uint32_t _dacId, IRQn_Type _isrId) :
		dac(_dac), dacId(_dacId), isrId(_isrId), cb(NULL) { };
	void begin(uint32_t period);
	void end();
	bool canQueue();
	size_t queueBuffer(const uint32_t *buffer, size_t size);
	void setOnQueueFree_CB(OnQueueFree_CB _cb);
	void onService();

private:
	Dacc *dac;
	uint32_t dacId;
	IRQn_Type isrId;
	OnQueueFree_CB cb;
};

extern DACClass DAC;

#endif
