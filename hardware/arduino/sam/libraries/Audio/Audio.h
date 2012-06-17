#ifndef AUDIO_H
#define AUDIO_H

#include "Arduino.h"
//#include <inttypes.h>
#include "Print.h"

class AudioClass : public Print {
public:
	AudioClass(Dacc *_dac, uint32_t _dacId) : dac(_dac), dacId(_dacId) { };
	void begin(uint32_t sampleRate);
	void end();

	virtual size_t write(uint8_t c) { write(&c, 1); };
	virtual size_t write(const uint8_t *buffer, size_t size);

private:
	Dacc *dac;
	uint32_t dacId;
};

extern AudioClass Audio;

#endif
