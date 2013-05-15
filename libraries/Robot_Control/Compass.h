#ifndef Compass_h
#define Compass_h

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

//0x21==0x42>>1, from bildr's code
#define HMC6352SlaveAddress 0x21
#define HMC6352ReadAddress 0x41

class Compass{
	public:
		void begin();
		float getReading();
	private:
		void _beginTransmission();
		void _endTransmission();

};

#endif