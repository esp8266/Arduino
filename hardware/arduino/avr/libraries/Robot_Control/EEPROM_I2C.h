#ifndef EEPROM_I2C_h
#define EEPROM_I2C_h

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define EE24LC512MAXBYTES 64000
#define DEVICEADDRESS 0x50

class EEPROM_I2C{
	public:
		void begin();
		
		void writeByte(unsigned int eeaddresspage, byte data);
		byte readByte(unsigned int eeaddresspage);
		
		void writePage(unsigned int eeaddresspage, byte* data, byte length );
		void readBuffer(unsigned int eeaddress, byte *buffer, int length );
		
		//uint16_t readPixel(uint16_t theMemoryAddress);
		//void readImage(uint16_t theMemoryAddress, int width, int height);
			
	protected:
		void _beginTransmission(unsigned int eeaddress);
		void _endTransmission();
};

#endif