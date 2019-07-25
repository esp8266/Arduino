/*
  si2c.c - Software I2C library for esp8266

  Copyright (c) 2015 Hristo Gochkov. All rights reserved.
  This file is part of the esp8266 core for Arduino environment.

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
  Modified January 2017 by Bjorn Hammarberg (bjoham@esp8266.com) - i2c slave support
*/
#include "twi.h"
#include "pins_arduino.h"
#include "wiring_private.h"



extern "C" {
#include "twi_util.h"

#include "ets_sys.h"


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"

// modes (private)
typedef enum { TWIPM_UNKNOWN = 0, TWIPM_IDLE, TWIPM_ADDRESSED, TWIPM_WAIT} twipModeType;

// states (private)
typedef enum { TWIP_UNKNOWN = 0, TWIP_IDLE, TWIP_START, TWIP_SEND_ACK, TWIP_WAIT_ACK, TWIP_WAIT_STOP, TWIP_SLA_W, TWIP_SLA_R, TWIP_REP_START, TWIP_READ, TWIP_STOP, TWIP_REC_ACK, TWIP_READ_ACK, TWIP_RWAIT_ACK, TWIP_WRITE, TWIP_BUS_ERR } twipStateType;
typedef enum { TWI_READY=0, TWI_MRX, TWI_MTX, TWI_SRX, TWI_STX } twiStateType;

static struct twi {
  unsigned int preferred_si2c_clock; // = 100000;
  unsigned char twi_dcount;// = 18;
  unsigned char twi_sda, twi_scl;
  uint32_t twi_clockStretchLimit;
  unsigned char twi_addr;// = 0;


  volatile twipModeType twip_mode;// = TWIPM_IDLE;
  volatile twipStateType twip_state;// = TWIP_IDLE;
  volatile int twip_status;// = TW_NO_INFO;
  volatile int bitCount;// = 0;

#define TWDR twi.twi_data
  volatile uint8_t twi_data;// = 0x00;
  volatile int twi_ack;// = 0;
  volatile int twi_ack_rec;// = 0;
  volatile int twi_timeout_ms;// = 10;

  volatile twiStateType twi_state;// = TWI_READY;
  volatile uint8_t twi_error;// = 0xFF;

  uint8_t twi_txBuffer[TWI_BUFFER_LENGTH];
  volatile int twi_txBufferIndex;
  volatile int twi_txBufferLength;

  uint8_t twi_rxBuffer[TWI_BUFFER_LENGTH];
  volatile int twi_rxBufferIndex;

  void (*twi_onSlaveTransmit)(void);
  void (*twi_onSlaveReceive)(uint8_t*, size_t);

#define EVENTTASK_QUEUE_SIZE 1
#define EVENTTASK_QUEUE_PRIO 2

#define TWI_SIG_RANGE	0x00000100
#define TWI_SIG_RX 		(TWI_SIG_RANGE + 0x01)
#define TWI_SIG_TX 		(TWI_SIG_RANGE + 0x02)

  ETSEvent eventTaskQueue[EVENTTASK_QUEUE_SIZE];
  ETSTimer timer;
} twi = { 100000, 18, 0, 0, 0, 0, TWIPM_IDLE, TWIP_IDLE, TW_NO_INFO, 0, 0, 0, 0, 10, TWI_READY, 0xff, {0}, 0, 0, {0}, 0, NULL, NULL, {{0}}, {0,0,0,0}};
// = { preferred_si2c_clock: 100000, twi_dcount: 18, twi_addr: 0, twip_mode: TWIPM_IDLE, twip_state: TWIP_IDLE, twip_status: TW_NO_INFO, bitCount: 0, twi_data: 0x00, twi_ack: 0, twi_ack_rec: 0, twi_timeout_ms: 10, twi_state: TWI_READY, twi_error: 0xFF };
#pragma GCC diagnostic pop

static void onSclChange(void);
static void onSdaChange(void);
static void eventTask(ETSEvent *e);
static void onTimer(void *unused);

#define SDA_LOW()   (GPES = (1 << twi.twi_sda)) //Enable SDA (becomes output and since GPO is 0 for the pin, it will pull the line low)
#define SDA_HIGH()  (GPEC = (1 << twi.twi_sda)) //Disable SDA (becomes input and since it has pullup it will go high)
#define SDA_READ()  ((GPI & (1 << twi.twi_sda)) != 0)
#define SCL_LOW()   (GPES = (1 << twi.twi_scl))
#define SCL_HIGH()  (GPEC = (1 << twi.twi_scl))
#define SCL_READ()  ((GPI & (1 << twi.twi_scl)) != 0)

#ifndef FCPU80
#define FCPU80 80000000L
#endif

#if F_CPU == FCPU80
#define TWI_CLOCK_STRETCH_MULTIPLIER 3
#else
#define TWI_CLOCK_STRETCH_MULTIPLIER 6
#endif

void twi_setClock(unsigned int freq){
  twi.preferred_si2c_clock = freq;
#if F_CPU == FCPU80
  if(freq <= 50000) twi.twi_dcount = 38;//about 50KHz
  else if(freq <= 100000) twi.twi_dcount = 19;//about 100KHz
  else if(freq <= 200000) twi.twi_dcount = 8;//about 200KHz
  else if(freq <= 300000) twi.twi_dcount = 3;//about 300KHz
  else if(freq <= 400000) twi.twi_dcount = 1;//about 400KHz
  else twi.twi_dcount = 1;//about 400KHz
#else
  if(freq <= 50000) twi.twi_dcount = 64;//about 50KHz
  else if(freq <= 100000) twi.twi_dcount = 32;//about 100KHz
  else if(freq <= 200000) twi.twi_dcount = 14;//about 200KHz
  else if(freq <= 300000) twi.twi_dcount = 8;//about 300KHz
  else if(freq <= 400000) twi.twi_dcount = 5;//about 400KHz
  else if(freq <= 500000) twi.twi_dcount = 3;//about 500KHz
  else if(freq <= 600000) twi.twi_dcount = 2;//about 600KHz
  else twi.twi_dcount = 1;//about 700KHz
#endif
}

void twi_setClockStretchLimit(uint32_t limit){
  twi.twi_clockStretchLimit = limit * TWI_CLOCK_STRETCH_MULTIPLIER;
}

void twi_init(unsigned char sda, unsigned char scl)
{
  // set timer function
  ets_timer_setfn(&twi.timer, onTimer, NULL);

  // create event task
  ets_task(eventTask, EVENTTASK_QUEUE_PRIO, twi.eventTaskQueue, EVENTTASK_QUEUE_SIZE);

  twi.twi_sda = sda;
  twi.twi_scl = scl;
  pinMode(twi.twi_sda, INPUT_PULLUP);
  pinMode(twi.twi_scl, INPUT_PULLUP);
  twi_setClock(twi.preferred_si2c_clock);
  twi_setClockStretchLimit(230); // default value is 230 uS

  if (twi.twi_addr != 0)
  {
	attachInterrupt(scl, onSclChange, CHANGE);
	attachInterrupt(sda, onSdaChange, CHANGE);
  }
}

void twi_setAddress(uint8_t address)
{
  // set twi slave address (skip over R/W bit)
  twi.twi_addr = address << 1;
}

static void ICACHE_RAM_ATTR twi_delay(unsigned char v){
  unsigned int i;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
  unsigned int reg;
  for (i = 0; i < v; i++) {
    reg = GPI;
  }
  (void)reg;
#pragma GCC diagnostic pop
}

static bool twi_write_start(void) {
  SCL_HIGH();
  SDA_HIGH();
  if (SDA_READ() == 0) {
    return false;
  }
  twi_delay(twi.twi_dcount);
  SDA_LOW();
  twi_delay(twi.twi_dcount);
  return true;
}

static bool twi_write_stop(void){
  uint32_t i = 0;
  SCL_LOW();
  SDA_LOW();
  twi_delay(twi.twi_dcount);
  SCL_HIGH();
  while (SCL_READ() == 0 && (i++) < twi.twi_clockStretchLimit); // Clock stretching
  twi_delay(twi.twi_dcount);
  SDA_HIGH();
  twi_delay(twi.twi_dcount);
  return true;
}

static bool twi_write_bit(bool bit) {
  uint32_t i = 0;
  SCL_LOW();
  if (bit) SDA_HIGH();
  else SDA_LOW();
  twi_delay(twi.twi_dcount+1);
  SCL_HIGH();
  while (SCL_READ() == 0 && (i++) < twi.twi_clockStretchLimit);// Clock stretching
  twi_delay(twi.twi_dcount);
  return true;
}

static bool twi_read_bit(void) {
  uint32_t i = 0;
  SCL_LOW();
  SDA_HIGH();
  twi_delay(twi.twi_dcount+2);
  SCL_HIGH();
  while (SCL_READ() == 0 && (i++) < twi.twi_clockStretchLimit);// Clock stretching
  bool bit = SDA_READ();
  twi_delay(twi.twi_dcount);
  return bit;
}

static bool twi_write_byte(unsigned char byte) {
  unsigned char bit;
  for (bit = 0; bit < 8; bit++) {
    twi_write_bit(byte & 0x80);
    byte <<= 1;
  }
  return !twi_read_bit();//NACK/ACK
}

static unsigned char twi_read_byte(bool nack) {
  unsigned char byte = 0;
  unsigned char bit;
  for (bit = 0; bit < 8; bit++) byte = (byte << 1) | twi_read_bit();
  twi_write_bit(nack);
  return byte;
}

unsigned char twi_writeTo(unsigned char address, unsigned char * buf, unsigned int len, unsigned char sendStop){
  unsigned int i;
  if(!twi_write_start()) return 4;//line busy
  if(!twi_write_byte(((address << 1) | 0) & 0xFF)) {
    if (sendStop) twi_write_stop();
    return 2; //received NACK on transmit of address
  }
  for(i=0; i<len; i++) {
    if(!twi_write_byte(buf[i])) {
      if (sendStop) twi_write_stop();
      return 3;//received NACK on transmit of data
    }
  }
  if(sendStop) twi_write_stop();
  i = 0;
  while(SDA_READ() == 0 && (i++) < 10){
    SCL_LOW();
    twi_delay(twi.twi_dcount);
    SCL_HIGH();
    unsigned int t=0; while(SCL_READ()==0 && (t++)<twi.twi_clockStretchLimit); // twi_clockStretchLimit
    twi_delay(twi.twi_dcount);
  }
  return 0;
}

unsigned char twi_readFrom(unsigned char address, unsigned char* buf, unsigned int len, unsigned char sendStop){
  unsigned int i;
  if(!twi_write_start()) return 4;//line busy
  if(!twi_write_byte(((address << 1) | 1) & 0xFF)) {
    if (sendStop) twi_write_stop();
    return 2;//received NACK on transmit of address
  }
  for(i=0; i<(len-1); i++) buf[i] = twi_read_byte(false);
  buf[len-1] = twi_read_byte(true);
  if(sendStop) twi_write_stop();
  i = 0;
  while(SDA_READ() == 0 && (i++) < 10){
    SCL_LOW();
    twi_delay(twi.twi_dcount);
    SCL_HIGH();
    unsigned int t=0; while(SCL_READ()==0 && (t++)<twi.twi_clockStretchLimit); // twi_clockStretchLimit
    twi_delay(twi.twi_dcount);
  }
  return 0;
}

uint8_t twi_status() {
    if (SCL_READ() == 0)
      return I2C_SCL_HELD_LOW; // SCL held low by another device, no procedure available to recover

    int clockCount = 20;
    while (SDA_READ() == 0 && clockCount-- > 0) { // if SDA low, read the bits slaves have to sent to a max
        twi_read_bit();
        if (SCL_READ() == 0) {
          return I2C_SCL_HELD_LOW_AFTER_READ; // I2C bus error. SCL held low beyond slave clock stretch time
        }
    }
    if (SDA_READ() == 0)
      return I2C_SDA_HELD_LOW; // I2C bus error. SDA line held low by slave/another_master after n bits.
    
    if (!twi_write_start())
      return I2C_SDA_HELD_LOW_AFTER_INIT;  // line busy. SDA again held low by another device. 2nd master?

    return I2C_OK;
}

uint8_t twi_transmit(const uint8_t* data, uint8_t length)
{
  uint8_t i;

  // ensure data will fit into buffer
  if (length > TWI_BUFFER_LENGTH) {
    return 1;
  }

  // ensure we are currently a slave transmitter
  if (twi.twi_state != TWI_STX) {
    return 2;
  }

  // set length and copy data into tx buffer
  twi.twi_txBufferLength = length;
  for (i = 0; i < length; ++i) {
    twi.twi_txBuffer[i] = data[i];
  }

  return 0;
}

void twi_attachSlaveRxEvent( void (*function)(uint8_t*, size_t) )
{
  twi.twi_onSlaveReceive = function;
}

void twi_attachSlaveTxEvent( void (*function)(void) )
{
  twi.twi_onSlaveTransmit = function;
}

inline void ICACHE_RAM_ATTR twi_reply(uint8_t ack)
{
  // transmit master read ready signal, with or without ack
  if (ack) {
    //TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWINT) | _BV(TWEA);
	SCL_HIGH();		// _BV(TWINT)
	twi.twi_ack = 1; 	// _BV(TWEA)
  } else {
	//TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWINT);
	SCL_HIGH();		// _BV(TWINT)
	twi.twi_ack = 0;	// ~_BV(TWEA)
  }
}

inline void ICACHE_RAM_ATTR twi_stop(void)
{
	// send stop condition
	//TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWEA) | _BV(TWINT) | _BV(TWSTO);
	SCL_HIGH();		// _BV(TWINT)
	twi.twi_ack = 1;	// _BV(TWEA)
	twi_delay(5);	// Maybe this should be here
	SDA_HIGH();		// _BV(TWSTO)
	// update twi state
	twi.twi_state = TWI_READY;
}

inline void ICACHE_RAM_ATTR twi_releaseBus(void)
{
	// release bus
	//TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWEA) | _BV(TWINT);
	SCL_HIGH();		// _BV(TWINT)
	twi.twi_ack = 1; 	// _BV(TWEA)
	SDA_HIGH();

	// update twi state
	twi.twi_state = TWI_READY;
}


void ICACHE_RAM_ATTR twi_onTwipEvent(uint8_t status)
{
  twi.twip_status = status;
  switch(status) {
    // Slave Receiver
    case TW_SR_SLA_ACK:   // addressed, returned ack
    case TW_SR_GCALL_ACK: // addressed generally, returned ack
    case TW_SR_ARB_LOST_SLA_ACK:   // lost arbitration, returned ack
    case TW_SR_ARB_LOST_GCALL_ACK: // lost arbitration, returned ack
      // enter slave receiver mode
      twi.twi_state = TWI_SRX;
      // indicate that rx buffer can be overwritten and ack
      twi.twi_rxBufferIndex = 0;
      twi_reply(1);
      break;
    case TW_SR_DATA_ACK:       // data received, returned ack
    case TW_SR_GCALL_DATA_ACK: // data received generally, returned ack
      // if there is still room in the rx buffer
      if(twi.twi_rxBufferIndex < TWI_BUFFER_LENGTH){
        // put byte in buffer and ack
        twi.twi_rxBuffer[twi.twi_rxBufferIndex++] = TWDR;
        twi_reply(1);
      }else{
        // otherwise nack
        twi_reply(0);
      }
      break;
    case TW_SR_STOP: // stop or repeated start condition received
      // put a null char after data if there's room
      if(twi.twi_rxBufferIndex < TWI_BUFFER_LENGTH){
        twi.twi_rxBuffer[twi.twi_rxBufferIndex] = '\0';
      }
      // callback to user-defined callback over event task to allow for non-RAM-residing code
	  //twi_rxBufferLock = true; // This may be necessary
	  ets_post(EVENTTASK_QUEUE_PRIO, TWI_SIG_RX, twi.twi_rxBufferIndex);

      // since we submit rx buffer to "wire" library, we can reset it
      twi.twi_rxBufferIndex = 0;
      break;

    case TW_SR_DATA_NACK:       // data received, returned nack
    case TW_SR_GCALL_DATA_NACK: // data received generally, returned nack
      // nack back at master
      twi_reply(0);
      break;

    // Slave Transmitter
    case TW_ST_SLA_ACK:          // addressed, returned ack
    case TW_ST_ARB_LOST_SLA_ACK: // arbitration lost, returned ack
      // enter slave transmitter mode
      twi.twi_state = TWI_STX;
      // ready the tx buffer index for iteration
      twi.twi_txBufferIndex = 0;
      // set tx buffer length to be zero, to verify if user changes it
      twi.twi_txBufferLength = 0;
      // callback to user-defined callback over event task to allow for non-RAM-residing code
      // request for txBuffer to be filled and length to be set
      // note: user must call twi_transmit(bytes, length) to do this
	  ets_post(EVENTTASK_QUEUE_PRIO, TWI_SIG_TX, 0);
	  break;

	case TW_ST_DATA_ACK: // byte sent, ack returned
      // copy data to output register
      TWDR = twi.twi_txBuffer[twi.twi_txBufferIndex++];

	  twi.bitCount = 8;
	  twi.bitCount--;
	  (twi.twi_data & 0x80) ? SDA_HIGH() : SDA_LOW();
	  twi.twi_data <<= 1;

      // if there is more to send, ack, otherwise nack
      if(twi.twi_txBufferIndex < twi.twi_txBufferLength){
        twi_reply(1);
      }else{
        twi_reply(0);
      }
      break;
    case TW_ST_DATA_NACK: // received nack, we are done
    case TW_ST_LAST_DATA: // received ack, but we are done already!
      // leave slave receiver state
      twi_releaseBus();
      break;

    // All
    case TW_NO_INFO:   // no state information
      break;
    case TW_BUS_ERROR: // bus error, illegal stop/start
      twi.twi_error = TW_BUS_ERROR;
      twi_stop();
      break;
  }
}

void ICACHE_RAM_ATTR onTimer(void *unused)
{
	(void)unused;
	twi_releaseBus();
	twi_onTwipEvent(TW_BUS_ERROR);
	twi.twip_mode = TWIPM_WAIT;
	twi.twip_state = TWIP_BUS_ERR;
}

static void eventTask(ETSEvent *e)
{

	if (e == NULL) {
		return;
	}

	switch (e->sig)
	{
		case TWI_SIG_TX:
			twi.twi_onSlaveTransmit();

			// if they didn't change buffer & length, initialize it
			if (twi.twi_txBufferLength == 0) {
				twi.twi_txBufferLength = 1;
				twi.twi_txBuffer[0] = 0x00;
			}

			// Initiate transmission
			twi_onTwipEvent(TW_ST_DATA_ACK);

			break;

		case TWI_SIG_RX:
			// ack future responses and leave slave receiver state
			twi_releaseBus();
			twi.twi_onSlaveReceive(twi.twi_rxBuffer, e->par);
			break;
	}
}

// The state machine is converted from a 0...15 state to a 1-hot encoded state, and then
// compared to the logical-or of all states with the same branch.  This removes the need
// for a large series of straight-line compares.  The biggest win is when multiple states
// all have the same branch (onSdaChange), but for others there is some benefit, still.
#define S2M(x) (1<<(x))
// Shorthand for if the state is any of the or'd bitmask x
#define IFSTATE(x) if (twip_state_mask & (x))

void ICACHE_RAM_ATTR onSclChange(void)
{
	unsigned int sda;
	unsigned int scl;

	sda	= SDA_READ();
	scl = SCL_READ();

	twi.twip_status = 0xF8;		// reset TWI status

	int twip_state_mask = S2M(twi.twip_state);
	IFSTATE(S2M(TWIP_START)|S2M(TWIP_REP_START)|S2M(TWIP_SLA_W)|S2M(TWIP_READ)) {
		if (!scl) {
			// ignore
		} else {
			twi.bitCount--;
			twi.twi_data <<= 1;
			twi.twi_data |= sda;

			if (twi.bitCount != 0) {
				// continue
			} else {
				twi.twip_state = TWIP_SEND_ACK;
			}
		}
	} else IFSTATE(S2M(TWIP_SEND_ACK)) {
		if (scl) {
			// ignore
		} else {
			if (twi.twip_mode == TWIPM_IDLE) {
				if ((twi.twi_data & 0xFE) != twi.twi_addr) {
					// ignore
				} else {
					SDA_LOW();
				}
			} else {
				if (!twi.twi_ack) {
					// ignore
				} else {
					SDA_LOW();
				}
			}
			twi.twip_state = TWIP_WAIT_ACK;
		}
	} else IFSTATE(S2M(TWIP_WAIT_ACK)) {
		if (scl) {
			// ignore
		} else {
			if (twi.twip_mode == TWIPM_IDLE) {
				if ((twi.twi_data & 0xFE) != twi.twi_addr) {
					SDA_HIGH();
					twi.twip_state = TWIP_WAIT_STOP;
				} else {
					SCL_LOW();	// clock stretching
					SDA_HIGH();
					twi.twip_mode = TWIPM_ADDRESSED;
					if (!(twi.twi_data & 0x01)) {
						twi_onTwipEvent(TW_SR_SLA_ACK);
						twi.bitCount = 8;
						twi.twip_state = TWIP_SLA_W;
					} else {
						twi_onTwipEvent(TW_ST_SLA_ACK);
						twi.twip_state = TWIP_SLA_R;
					}
				}
			} else {
				SCL_LOW();	// clock stretching
				SDA_HIGH();
				if (!twi.twi_ack) {
					twi_onTwipEvent(TW_SR_DATA_NACK);
					twi.twip_mode = TWIPM_WAIT;
					twi.twip_state = TWIP_WAIT_STOP;
				} else {
					twi_onTwipEvent(TW_SR_DATA_ACK);
					twi.bitCount = 8;
					twi.twip_state = TWIP_READ;
				}
			}
		}
	} else IFSTATE(S2M(TWIP_SLA_R)|S2M(TWIP_WRITE)) {
		if (scl) {
			// ignore
		} else {
			twi.bitCount--;
			(twi.twi_data & 0x80) ? SDA_HIGH() : SDA_LOW();
			twi.twi_data <<= 1;

			if (twi.bitCount != 0) {
				// continue
			} else {
				twi.twip_state = TWIP_REC_ACK;
			}
		}
	} else IFSTATE(S2M(TWIP_REC_ACK)) {
		if (scl) {
			// ignore
		} else {
			SDA_HIGH();
			twi.twip_state = TWIP_READ_ACK;
		}
	} else IFSTATE(S2M(TWIP_READ_ACK)) {
		if (!scl) {
			// ignore
		} else {
			twi.twi_ack_rec = !sda;
			twi.twip_state = TWIP_RWAIT_ACK;
		}
	} else IFSTATE(S2M(TWIP_RWAIT_ACK)) {
		if (scl) {
			// ignore
		} else {
			SCL_LOW();	// clock stretching
			if (twi.twi_ack && twi.twi_ack_rec) {
				twi_onTwipEvent(TW_ST_DATA_ACK);
				twi.twip_state = TWIP_WRITE;
			} else {
				// we have no more data to send and/or the master doesn't want anymore
				twi_onTwipEvent(twi.twi_ack_rec ? TW_ST_LAST_DATA : TW_ST_DATA_NACK);
				twi.twip_mode = TWIPM_WAIT;
				twi.twip_state = TWIP_WAIT_STOP;
			}
		}
	}
}

void ICACHE_RAM_ATTR onSdaChange(void)
{
	unsigned int sda;
	unsigned int scl;
	sda	= SDA_READ();
	scl = SCL_READ();

	int twip_state_mask = S2M(twi.twip_state);
	if (scl) { /* !DATA */
		IFSTATE(S2M(TWIP_IDLE)) {
			if (sda) {
				// STOP - ignore
			} else {
				// START
				twi.bitCount = 8;
				twi.twip_state = TWIP_START;
				ets_timer_arm_new(&twi.timer, twi.twi_timeout_ms, false, true); // Once, ms
			}
		} else IFSTATE(S2M(TWIP_START)|S2M(TWIP_REP_START)|S2M(TWIP_SEND_ACK)|S2M(TWIP_WAIT_ACK)|S2M(TWIP_SLA_R)|S2M(TWIP_REC_ACK)|S2M(TWIP_READ_ACK)|S2M(TWIP_RWAIT_ACK)|S2M(TWIP_WRITE)) {
			// START or STOP
			SDA_HIGH();	 // Should not be necessary
			twi_onTwipEvent(TW_BUS_ERROR);
			twi.twip_mode = TWIPM_WAIT;
			twi.twip_state = TWIP_BUS_ERR;
		} else IFSTATE(S2M(TWIP_WAIT_STOP)|S2M(TWIP_BUS_ERR)) {
			if (sda) {
				// STOP
				SCL_LOW();	// clock stretching
				ets_timer_disarm(&twi.timer);
				twi.twip_state = TWIP_IDLE;
				twi.twip_mode = TWIPM_IDLE;
				SCL_HIGH();
			} else {
				// START
				if (twi.twip_state == TWIP_BUS_ERR) {
					// ignore
				} else {
					twi.bitCount = 8;
					twi.twip_state = TWIP_REP_START;
					ets_timer_arm_new(&twi.timer, twi.twi_timeout_ms, false, true); // Once, ms
				}
			}
		} else IFSTATE(S2M(TWIP_SLA_W)|S2M(TWIP_READ)) {
			// START or STOP
			if (twi.bitCount != 7) {
				// inside byte transfer - error
				twi_onTwipEvent(TW_BUS_ERROR);
				twi.twip_mode = TWIPM_WAIT;
				twi.twip_state = TWIP_BUS_ERR;
			} else {
				// during first bit in byte transfer - ok
				SCL_LOW();	// clock stretching
				twi_onTwipEvent(TW_SR_STOP);
				if (sda) {
					// STOP
					ets_timer_disarm(&twi.timer);
					twi.twip_state = TWIP_IDLE;
					twi.twip_mode = TWIPM_IDLE;
				} else {
					// START
					twi.bitCount = 8;
					ets_timer_arm_new(&twi.timer, twi.twi_timeout_ms, false, true); // Once, ms
					twi.twip_state = TWIP_REP_START;
					twi.twip_mode = TWIPM_IDLE;
				}
			}
		}
	}
}

};
