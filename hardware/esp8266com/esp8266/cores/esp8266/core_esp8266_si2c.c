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
*/
#include "twi.h"
#include "pins_arduino.h"
#include "wiring_private.h"

#define SDA_LOW()   (GPES = (1 << twi->sda)) //Enable SDA (becomes output and since GPO is 0 for the pin, it will pull the line low)
#define SDA_HIGH()  (GPEC = (1 << twi->sda)) //Disable SDA (becomes input and since it has pullup it will go high)
#define SDA_READ()  ((GPI & (1 << twi->sda)) != 0)
#define SCL_LOW()   (GPES = (1 << twi->scl))
#define SCL_HIGH()  (GPEC = (1 << twi->scl))
#define SCL_READ()  ((GPI & (1 << twi->scl)) != 0)

#ifndef FCPU80
#define FCPU80 80000000L
#endif

#if F_CPU == FCPU80
#define TWI_CLOCK_STRETCH 800
#else
#define TWI_CLOCK_STRETCH 1600
#endif

void twi_setClock(TwiConfig* twi, unsigned int freq){
#if F_CPU == FCPU80
  if(freq <= 100000) twi->dcount = 19;//about 100KHz
  else if(freq <= 200000) twi->dcount = 8;//about 200KHz
  else if(freq <= 300000) twi->dcount = 3;//about 300KHz
  else if(freq <= 400000) twi->dcount = 1;//about 400KHz
  else twi->dcount = 1;//about 400KHz
#else
  if(freq <= 100000) twi->dcount = 32;//about 100KHz
  else if(freq <= 200000) twi->dcount = 14;//about 200KHz
  else if(freq <= 300000) twi->dcount = 8;//about 300KHz
  else if(freq <= 400000) twi->dcount = 5;//about 400KHz
  else if(freq <= 500000) twi->dcount = 3;//about 500KHz
  else if(freq <= 600000) twi->dcount = 2;//about 600KHz
  else twi->dcount = 1;//about 700KHz
#endif
}

void twi_init(TwiConfig* twi, unsigned char sda, unsigned char scl){
  twi->sda = sda;
  twi->scl = scl;
  pinMode(twi->sda, INPUT_PULLUP);
  pinMode(twi->scl, INPUT_PULLUP);
  twi_setClock(twi, 100000);
}

void twi_stop(TwiConfig* twi){
  pinMode(twi->sda, INPUT);
  pinMode(twi->scl, INPUT);
}

static void twi_delay(unsigned char v){
  unsigned int i;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
  unsigned int reg;
  for(i=0;i<v;i++) reg = GPI;
#pragma GCC diagnostic pop
}

static bool twi_write_start(TwiConfig* twi) {
  SCL_HIGH();
  SDA_HIGH();
  if (SDA_READ() == 0) return false;
  twi_delay(twi->dcount);
  SDA_LOW();
  twi_delay(twi->dcount);
  return true;
}

static bool twi_write_stop(TwiConfig* twi){
  unsigned int i = 0;
  SCL_LOW();
  SDA_LOW();
  twi_delay(twi->dcount);
  SCL_HIGH();
  while (SCL_READ() == 0 && (i++) < TWI_CLOCK_STRETCH);// Clock stretching (up to 100us)
  twi_delay(twi->dcount);
  SDA_HIGH();
  twi_delay(twi->dcount);

  return true;
}

static bool twi_write_bit(TwiConfig* twi, bool bit) {
  unsigned int i = 0;
  SCL_LOW();
  if (bit) SDA_HIGH();
  else SDA_LOW();
  twi_delay(twi->dcount+1);
  SCL_HIGH();
  while (SCL_READ() == 0 && (i++) < TWI_CLOCK_STRETCH);// Clock stretching (up to 100us)
  twi_delay(twi->dcount);
  return true;
}

static bool twi_read_bit(TwiConfig* twi) {
  unsigned int i = 0;
  SCL_LOW();
  SDA_HIGH();
  twi_delay(twi->dcount+2);
  SCL_HIGH();
  while (SCL_READ() == 0 && (i++) < TWI_CLOCK_STRETCH);// Clock stretching (up to 100us)
  bool bit = SDA_READ();
  twi_delay(twi->dcount);
  return bit;
}

static bool twi_write_byte(TwiConfig* twi, unsigned char byte) {
  unsigned char bit;
  for (bit = 0; bit < 8; bit++) {
    twi_write_bit(twi, byte & 0x80);
    byte <<= 1;
  }
  return !twi_read_bit(twi);//NACK/ACK
}

static unsigned char twi_read_byte(TwiConfig* twi, bool nack) {
  unsigned char byte = 0;
  unsigned char bit;
  for (bit = 0; bit < 8; bit++) byte = (byte << 1) | twi_read_bit(twi);
  twi_write_bit(twi, nack);
  return byte;
}

unsigned char twi_writeTo(TwiConfig* twi, unsigned char address, unsigned char * buf, unsigned int len, unsigned char sendStop){
  unsigned int i;
  if(!twi_write_start(twi)) return 4;//line busy
  if(!twi_write_byte(twi, ((address << 1) | 0) & 0xFF)) return 2;//received NACK on transmit of address
  for(i=0; i<len; i++){
    if(!twi_write_byte(twi, buf[i])) return 3;//received NACK on transmit of data
  }
  if(sendStop) twi_write_stop(twi);
  i = 0;
  while(SDA_READ() == 0 && (i++) < 10){
    SCL_LOW();
    twi_delay(twi->dcount);
    SCL_HIGH();
    twi_delay(twi->dcount);
  }
  return 0;
}

unsigned char twi_readFrom(TwiConfig* twi, unsigned char address, unsigned char* buf, unsigned int len, unsigned char sendStop){
  unsigned int i;
  if(!twi_write_start(twi)) return 4;//line busy
  if(!twi_write_byte(twi, ((address << 1) | 1) & 0xFF)) return 2;//received NACK on transmit of address
  for(i=0; i<(len-1); i++) buf[i] = twi_read_byte(twi, false);
  buf[len-1] = twi_read_byte(twi, true);
  if(sendStop) twi_write_stop(twi);
  i = 0;
  while(SDA_READ() == 0 && (i++) < 10){
    SCL_LOW();
    twi_delay(twi->dcount);
    SCL_HIGH();
    twi_delay(twi->dcount);
  }
  return 0;
}
