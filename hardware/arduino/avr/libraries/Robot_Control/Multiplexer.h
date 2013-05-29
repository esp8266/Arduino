#ifndef Multiplexer_h
#define Multiplexer_h

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class Multiplexer{
	public:
		void begin(uint8_t* selectors, uint8_t Z, uint8_t length);
		void selectPin(uint8_t num);
		int getAnalogValue();
		int getAnalogValueAt(uint8_t num);
		bool getDigitalValue();
		bool getDigitalValueAt(uint8_t num);
	private:
		uint8_t selectors[4];
		uint8_t pin_Z;
		uint8_t length;
};

#endif
