/*
This file is part of the GSM3 communications library for Arduino
-- Multi-transport communications platform
-- Fully asynchronous
-- Includes code for the Arduino-Telefonica GSM/GPRS Shield V1
-- Voice calls
-- SMS
-- TCP/IP connections
-- HTTP basic clients

This library has been developed by Telefónica Digital - PDI -
- Physical Internet Lab, as part as its collaboration with
Arduino and the Open Hardware Community. 

September-December 2012

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

The latest version of this library can always be found at
https://github.com/BlueVia/Official-Arduino
*/
#include "GSM3SoftSerial.h"
#include "GSM3IO.h"
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "pins_arduino.h"
#include <HardwareSerial.h>
#include <Arduino.h>

#define __XON__ 0x11
#define __XOFF__ 0x13

#define _GSMSOFTSERIALFLAGS_ESCAPED_ 0x01
#define _GSMSOFTSERIALFLAGS_SENTXOFF_ 0x02

//
// Lookup table
//
#define __PARAGRAPHGUARD__ 50
typedef struct _DELAY_TABLE
{
  long baud;
  unsigned short rx_delay_centering;
  unsigned short rx_delay_intrabit;
  unsigned short rx_delay_stopbit;
  unsigned short tx_delay;
} DELAY_TABLE;

#if F_CPU == 16000000

static const DELAY_TABLE PROGMEM table[] = 
{
  //  baud    rxcenter   rxintra    rxstop    tx
  { 115200,   1,         17,        17,       12,    },
  { 57600,    10,        37,        37,       33,    },
  { 38400,    25,        57,        57,       54,    },
  { 31250,    31,        70,        70,       68,    },
  { 28800,    34,        77,        77,       74,    },
  { 19200,    54,        117,       117,      114,   },
  { 14400,    74,        156,       156,      153,   },
  { 9600,     114,       236,       236,      233,   },
  { 4800,     233,       474,       474,      471,   },
  { 2400,     471,       950,       950,      947,   },
  { 1200,     947,       1902,      1902,     1899,  },
  { 300,      3804,      7617,      7617,     7614,  },
};

const int XMIT_START_ADJUSTMENT = 5;

#elif F_CPU == 8000000

static const DELAY_TABLE table[] PROGMEM = 
{
  //  baud    rxcenter    rxintra    rxstop  tx
  { 115200,   1,          5,         5,      3,      },
  { 57600,    1,          15,        15,     13,     },
  { 38400,    2,          25,        26,     23,     },
  { 31250,    7,          32,        33,     29,     },
  { 28800,    11,         35,        35,     32,     },
  { 19200,    20,         55,        55,     52,     },
  { 14400,    30,         75,        75,     72,     },
  { 9600,     50,         114,       114,    112,    },
  { 4800,     110,        233,       233,    230,    },
  { 2400,     229,        472,       472,    469,    },
  { 1200,     467,        948,       948,    945,    },
  { 300,      1895,       3805,      3805,   3802,   },
};

const int XMIT_START_ADJUSTMENT = 4;

#elif F_CPU == 20000000

// 20MHz support courtesy of the good people at macegr.com.
// Thanks, Garrett!

static const DELAY_TABLE PROGMEM table[] =
{
  //  baud    rxcenter    rxintra    rxstop  tx
  { 115200,   3,          21,        21,     18,     },
  { 57600,    20,         43,        43,     41,     },
  { 38400,    37,         73,        73,     70,     },
  { 31250,    45,         89,        89,     88,     },
  { 28800,    46,         98,        98,     95,     },
  { 19200,    71,         148,       148,    145,    },
  { 14400,    96,         197,       197,    194,    },
  { 9600,     146,        297,       297,    294,    },
  { 4800,     296,        595,       595,    592,    },
  { 2400,     592,        1189,      1189,   1186,   },
  { 1200,     1187,       2379,      2379,   2376,   },
  { 300,      4759,       9523,      9523,   9520,   },
};

const int XMIT_START_ADJUSTMENT = 6;

#else

#error This version of GSM3SoftSerial supports only 20, 16 and 8MHz processors

#endif

GSM3SoftSerial* GSM3SoftSerial::_activeObject=0;

GSM3SoftSerial::GSM3SoftSerial():
	_rx_delay_centering(0),
	_rx_delay_intrabit(0),
	_rx_delay_stopbit(0),
	_tx_delay(0),
	cb(this)
{
	setTX();
	setRX();
	//comStatus=0;
	//waitingAnswer=false;
}

int GSM3SoftSerial::begin(long speed)
{
  _rx_delay_centering = _rx_delay_intrabit = _rx_delay_stopbit = _tx_delay = 0;

  for (unsigned i=0; i<sizeof(table)/sizeof(table[0]); ++i)
  {
    long baud = pgm_read_dword(&table[i].baud);
    if (baud == speed)
    {
      _rx_delay_centering = pgm_read_word(&table[i].rx_delay_centering);
      _rx_delay_intrabit = pgm_read_word(&table[i].rx_delay_intrabit);
      _rx_delay_stopbit = pgm_read_word(&table[i].rx_delay_stopbit);
      _tx_delay = pgm_read_word(&table[i].tx_delay);
      break;
    }
  }
  
  if (_rx_delay_stopbit)
  {
    if (digitalPinToPCICR(__RXPIN__))
    {
      *digitalPinToPCICR(__RXPIN__) |= _BV(digitalPinToPCICRbit(__RXPIN__));
      *digitalPinToPCMSK(__RXPIN__) |= _BV(digitalPinToPCMSKbit(__RXPIN__));
    }
    tunedDelay(_tx_delay); // if we were low this establishes the end
  }
  
  _activeObject=this;

}

void GSM3SoftSerial::close()
 {
	_activeObject=0;
 }

size_t GSM3SoftSerial::write(uint8_t c)
{
	if (_tx_delay == 0)
		return 0;

	// Characters to be escaped under XON/XOFF control with Quectel
	if(c==0x11)
	{
		this->finalWrite(0x77);
		return this->finalWrite(0xEE);
	}

	if(c==0x13)
	{
		this->finalWrite(0x77);
		return this->finalWrite(0xEC);
	}

	if(c==0x77)
	{
		this->finalWrite(0x77);
		return this->finalWrite(0x88);
	}
	
	return this->finalWrite(c);
}

size_t GSM3SoftSerial::finalWrite(uint8_t c)
{
	
	uint8_t oldSREG = SREG;
	cli();  // turn off interrupts for a clean txmit

	// Write the start bit
	tx_pin_write(LOW);
	tunedDelay(_tx_delay + XMIT_START_ADJUSTMENT);

	// Write each of the 8 bits
	for (byte mask = 0x01; mask; mask <<= 1)
	{
		if (c & mask) // choose bit
			tx_pin_write(HIGH); // send 1
		else
			tx_pin_write(LOW); // send 0
		tunedDelay(_tx_delay);
	}

	tx_pin_write(HIGH); // restore pin to natural state
	
	SREG = oldSREG; // turn interrupts back on
	tunedDelay(_tx_delay);
				
	return 1;
}

/*inline*/ void GSM3SoftSerial::tunedDelay(uint16_t delay) { 
  uint8_t tmp=0;

  asm volatile("sbiw    %0, 0x01 \n\t"
    "ldi %1, 0xFF \n\t"
    "cpi %A0, 0xFF \n\t"
    "cpc %B0, %1 \n\t"
    "brne .-10 \n\t"
    : "+r" (delay), "+a" (tmp)
    : "0" (delay)
    );
}

void GSM3SoftSerial::tx_pin_write(uint8_t pin_state)
{
  // Direct port manipulation is faster than digitalWrite/Read
  if (pin_state == LOW)
    *_transmitPortRegister &= ~_transmitBitMask;
  else
    *_transmitPortRegister |= _transmitBitMask;
}

void GSM3SoftSerial::setTX()
{
  pinMode(__TXPIN__, OUTPUT);
  digitalWrite(__TXPIN__, HIGH);
  // For digital port direct manipulation
  _transmitBitMask = digitalPinToBitMask(__TXPIN__);
  uint8_t port = digitalPinToPort(__TXPIN__);
  _transmitPortRegister = portOutputRegister(port);
}

void GSM3SoftSerial::setRX()
{
  pinMode(__RXPIN__, INPUT);
  digitalWrite(__RXPIN__, HIGH);  // pullup for normal logic!
  // For digital port direct manipulation
  _receiveBitMask = digitalPinToBitMask(__RXPIN__);
  uint8_t port = digitalPinToPort(__RXPIN__);
  _receivePortRegister = portInputRegister(port);

#ifdef  __AVR_ATmega32U4__
//#define __RXINT__ 1
  attachInterrupt(__RXINT__, GSM3SoftSerial::handle_interrupt, FALLING);
#endif
	// This line comes from the High Middle Ages...
	// attachInterrupt(__RXINT__, GSM3SoftSerial::handle_interrupt, FALLING);
}

void GSM3SoftSerial::handle_interrupt()
{
	if(_activeObject)
		_activeObject->recv();
}

uint8_t GSM3SoftSerial::rx_pin_read()
{
  // Digital port manipulation
  return *_receivePortRegister & _receiveBitMask;
}

void GSM3SoftSerial::recv()
{

#if GCC_VERSION < 40302
// Work-around for avr-gcc 4.3.0 OSX version bug
// Preserve the registers that the compiler misses
// (courtesy of Arduino forum user *etracer*)
  asm volatile(
    "push r18 \n\t"
    "push r19 \n\t"
    "push r20 \n\t"
    "push r21 \n\t"
    "push r22 \n\t"
    "push r23 \n\t"
    "push r26 \n\t"
    "push r27 \n\t"
    ::);
#endif  

  bool firstByte=true;
  byte thisHead;
  
  uint8_t d = 0;
  bool morebytes=false;
  //bool fullbuffer=(cb.availableBytes()<3);
  bool fullbuffer;
  bool capturado_fullbuffer = 0;
  int i;
  byte oldTail;

  // If RX line is high, then we don't see any start bit
  // so interrupt is probably not for us
  if (!rx_pin_read())
  {
	do
	{
		oldTail=cb.getTail();
		// Wait approximately 1/2 of a bit width to "center" the sample
		tunedDelay(_rx_delay_centering);
		
		fullbuffer=(cb.availableBytes()<6);

		
		if(fullbuffer&&(!capturado_fullbuffer))
			tx_pin_write(LOW);

		
		// Read each of the 8 bits
		for (uint8_t i=0x1; i; i <<= 1)
		{
			tunedDelay(_rx_delay_intrabit);
			uint8_t noti = ~i;
			if (rx_pin_read())
				d |= i;
			else // else clause added to ensure function timing is ~balanced
				d &= noti;
			
			if(fullbuffer&&(!capturado_fullbuffer))
			{
			  if((uint8_t)__XOFF__ & i)
				tx_pin_write(HIGH);
			  else
				tx_pin_write(LOW);
			}
		}

		if(fullbuffer&&(!capturado_fullbuffer))
		{
			tunedDelay(_rx_delay_intrabit);
			tx_pin_write(HIGH);	
		}
		
		// So, we know the buffer is full, and we have sent a XOFF
		if (fullbuffer) 
		{
			capturado_fullbuffer =1;
			_flags |=_GSMSOFTSERIALFLAGS_SENTXOFF_;
		}


		// skip the stop bit
		if (!fullbuffer) tunedDelay(_rx_delay_stopbit);
		
		if(keepThisChar(&d))
		{
			cb.write(d);
			if(firstByte)
			{
				firstByte=false;
				thisHead=cb.getTail();
			}
		}
		
		
		// This part is new. It is used to detect the end of a "paragraph"
		// Caveat: the old fashion would let processor a bit of time between bytes, 
		// that here is lost
		// This active waiting avoids drifting
		morebytes=false;
		// TO-DO. This PARAGRAPHGUARD is empyric. We should test it for every speed
		for(i=0;i<__PARAGRAPHGUARD__;i++)
		{	
			tunedDelay(1);
			if(!rx_pin_read())
			{
				morebytes=true;
				break;
			}
		}
	}while(morebytes);
	// If we find a line feed, we are at the end of a paragraph
	// check!
	
	if (fullbuffer)
	{
		// And... go handle it!
		if(mgr)
			mgr->manageMsg(thisHead, cb.getTail());
	}
	else if(d==10)
	{
		// And... go handle it!
		if(mgr)
			mgr->manageMsg(thisHead, cb.getTail());
	}
	else if (d==32)
	{
		// And... go handle it!
		if(mgr)
			mgr->manageMsg(thisHead, cb.getTail());
	}
  }

#if GCC_VERSION < 40302
// Work-around for avr-gcc 4.3.0 OSX version bug
// Restore the registers that the compiler misses
  asm volatile(
    "pop r27 \n\t"
    "pop r26 \n\t"
    "pop r23 \n\t"
    "pop r22 \n\t"
    "pop r21 \n\t"
    "pop r20 \n\t"
    "pop r19 \n\t"
    "pop r18 \n\t"
    ::);
#endif
}

bool GSM3SoftSerial::keepThisChar(uint8_t* c)
{
	// Horrible things for Quectel XON/XOFF
	// 255 is the answer to a XOFF
	// It comes just once
	if((*c==255)&&(_flags & _GSMSOFTSERIALFLAGS_SENTXOFF_))
	{
		_flags ^= _GSMSOFTSERIALFLAGS_SENTXOFF_;
		return false;
	}

	// 0x77, w, is the escape character
	if(*c==0x77)
	{
		_flags |= _GSMSOFTSERIALFLAGS_ESCAPED_;
		return false;
	}
	
	// and these are the escaped codes
	if(_flags & _GSMSOFTSERIALFLAGS_ESCAPED_)
	{
		if(*c==0xEE)
			*c=0x11;
		else if(*c==0xEC)
			*c=0x13;
		else if(*c==0x88)
			*c=0x77;
			
		_flags ^= _GSMSOFTSERIALFLAGS_ESCAPED_;
		return true;
	}
	
	return true;
}

void GSM3SoftSerial::spaceAvailable()
{
	// If there is spaceAvailable in the buffer, lets send a XON
	finalWrite((byte)__XON__);
}


// This is here to avoid problems with Arduino compiler
void GSM3SoftSerialMgr::manageMsg(byte from, byte to){};

//#define PCINT1_vect _VECTOR(2)
//#undef PCINT1_vect

#if defined(PCINT0_vect)
ISR(PCINT0_vect)
{
  GSM3SoftSerial::handle_interrupt();
}
#endif

#if defined(PCINT1_vect)
ISR(PCINT1_vect)
{
  GSM3SoftSerial::handle_interrupt();
}
#endif

#if defined(PCINT2_vect)
ISR(PCINT2_vect)
{
  GSM3SoftSerial::handle_interrupt();
}
#endif

#if defined(PCINT3_vect)
ISR(PCINT3_vect)
{
  GSM3SoftSerial::handle_interrupt();
}
#endif

