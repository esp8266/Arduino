/*! \file i2csw.c \brief Software-driven I2C interface using port pins. */
//*****************************************************************************
//
// File Name	: 'i2csw.c'
// Title		: Software-driven I2C interface using port pins
// Author		: Pascal Stang
// Created		: 11/22/2000
// Revised		: 5/2/2002
// Version		: 1.1
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#include <avr/io.h>

#include "i2csw.h"

// Standard I2C bit rates are:
// 100KHz for slow speed
// 400KHz for high speed

//#define QDEL	delay(5)		// i2c quarter-bit delay
//#define HDEL	delay(10)		// i2c half-bit delay

// i2c quarter-bit delay
#define QDEL	asm volatile("nop"); asm volatile("nop"); asm volatile("nop"); asm volatile("nop"); asm volatile("nop");
// i2c half-bit delay
#define HDEL	asm volatile("nop"); asm volatile("nop"); asm volatile("nop"); asm volatile("nop"); asm volatile("nop"); asm volatile("nop"); asm volatile("nop"); asm volatile("nop"); asm volatile("nop"); asm volatile("nop");

#define I2C_SDL_LO      cbi( SDAPORT, SDA)
#define I2C_SDL_HI      sbi( SDAPORT, SDA)

#define I2C_SCL_LO      cbi( SCLPORT, SCL); 
#define I2C_SCL_HI      sbi( SCLPORT, SCL); 

#define I2C_SCL_TOGGLE  HDEL; I2C_SCL_HI; HDEL; I2C_SCL_LO;
#define I2C_START       I2C_SDL_LO; QDEL; I2C_SCL_LO; 
#define I2C_STOP        HDEL; I2C_SCL_HI; QDEL; I2C_SDL_HI; HDEL;

/*
void i2ct(void)
{
	HDEL; I2C_SCL_HI; HDEL; I2C_SCL_LO;
}

void i2cstart(void)
{
	I2C_SDL_LO; QDEL; I2C_SCL_LO; 
}

void i2cstop(void)
{
	HDEL; I2C_SCL_HI; QDEL; I2C_SDL_HI; HDEL;
}


#define I2C_SCL_TOGGLE  i2ct();
#define I2C_START       i2cstart();
#define I2C_STOP        i2cstop();	
*/

UINT i2cPutbyte(u08 b)
{
	int i;
	
	for (i=7;i>=0;i--)
	{
		if ( b & (1<<i) )
			I2C_SDL_HI;
		else
			I2C_SDL_LO;			// address bit
			I2C_SCL_TOGGLE;		// clock HI, delay, then LO
	}

	I2C_SDL_HI;					// leave SDL HI
	// added    
	cbi(SDADDR, SDA);			// change direction to input on SDA line (may not be needed)
	HDEL;
	I2C_SCL_HI;					// clock back up
  	b = inb(SDAPIN) & (1<<SDA);	// get the ACK bit

	HDEL;
	I2C_SCL_LO;					// not really ??
	sbi(SDADDR, SDA);			// change direction back to output
	HDEL;
	return (b == 0);			// return ACK value
}


u08 i2cGetbyte(UINT last)
{
	int i;
	u08 c,b = 0;
		
	I2C_SDL_HI;					// make sure pullups are ativated
	cbi(SDADDR, SDA);			// change direction to input on SDA line (may not be needed)

	for(i=7;i>=0;i--)
	{
		HDEL;
		I2C_SCL_HI;				// clock HI
	  	c = inb(SDAPIN) & (1<<SDA);  
		b <<= 1;
		if(c) b |= 1;
		HDEL;
    	I2C_SCL_LO;				// clock LO
	}

	sbi(SDADDR, SDA);			// change direction to output on SDA line
  
	if (last)
		I2C_SDL_HI;				// set NAK
	else
		I2C_SDL_LO;				// set ACK

	I2C_SCL_TOGGLE;				// clock pulse
	I2C_SDL_HI;					// leave with SDL HI
	return b;					// return received byte
}


//************************
//* I2C public functions *
//************************

//! Initialize I2C communication
void i2cInit(void)
{
	sbi( SDADDR, SDA);			// set SDA as output
	sbi( SCLDDR, SCL);			// set SCL as output
	I2C_SDL_HI;					// set I/O state and pull-ups
	I2C_SCL_HI;					// set I/O state and pull-ups
}

//! Send a byte sequence on the I2C bus
void i2cSend(u08 device, u08 subAddr, u08 length, u08 *data)
{
	I2C_START;      			// do start transition
	i2cPutbyte(device); 		// send DEVICE address
	i2cPutbyte(subAddr);		// and the subaddress

	// send the data
	while (length--)
		i2cPutbyte(*data++);

	I2C_SDL_LO;					// clear data line and
	I2C_STOP;					// send STOP transition
}

//! Retrieve a byte sequence on the I2C bus
void i2cReceive(u08 device, u08 subAddr, u08 length, u08 *data)
{
	int j = length;
	u08 *p = data;

	I2C_START;					// do start transition
	i2cPutbyte(device);			// send DEVICE address
	i2cPutbyte(subAddr);   		// and the subaddress
	HDEL;
	I2C_SCL_HI;      			// do a repeated START
	I2C_START;					// transition

	i2cPutbyte(device | READ);	// resend DEVICE, with READ bit set

	// receive data bytes
	while (j--)
		*p++ = i2cGetbyte(j == 0);

	I2C_SDL_LO;					// clear data line and
	I2C_STOP;					// send STOP transition
}

