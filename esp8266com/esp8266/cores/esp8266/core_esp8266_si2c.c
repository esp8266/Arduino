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

unsigned char twi_dcount = 18;
static unsigned char twi_sda, twi_scl;

#define SDA_LOW()   (GPES = (1 << twi_sda)) //Enable SDA (becomes output and since GPO is 0 for the pin, it will pull the line low)
#define SDA_HIGH()  (GPEC = (1 << twi_sda)) //Disable SDA (becomes input and since it has pullup it will go high) 
#define SDA_READ()  ((GPI & (1 << twi_sda)) != 0)
#define SCL_LOW()   (GPES = (1 << twi_scl)) 
#define SCL_HIGH()  (GPEC = (1 << twi_scl)) 
#define SCL_READ()  ((GPI & (1 << twi_scl)) != 0)

#ifndef FCPU80
#define FCPU80 80000000L
#endif

#if F_CPU == FCPU80
#define TWI_CLOCK_STRETCH 200
#else
#define TWI_CLOCK_STRETCH 400
#endif

void twi_setClock(unsigned int freq){
#if F_CPU == FCPU80
  if(freq <= 100000) twi_dcount = 18;//about 100KHz
  else if(freq <= 200000) twi_dcount = 8;//about 200KHz
  else if(freq <= 300000) twi_dcount = 4;//about 300KHz
  else if(freq <= 400000) twi_dcount = 2;//about 370KHz
  else twi_dcount = 1;//about 450KHz
#else
  if(freq <= 100000) twi_dcount = 32;//about 100KHz
  else if(freq <= 200000) twi_dcount = 16;//about 200KHz
  else if(freq <= 300000) twi_dcount = 8;//about 300KHz
  else if(freq <= 400000) twi_dcount = 4;//about 370KHz
  else twi_dcount = 2;//about 450KHz
#endif
}

void twi_init(unsigned char sda, unsigned char scl){
  twi_sda = sda;
  twi_scl = scl;
  pinMode(twi_sda, INPUT_PULLUP);
  pinMode(twi_scl, INPUT_PULLUP);
  twi_setClock(100000);
}

void twi_stop(void){
  pinMode(twi_sda, INPUT);
  pinMode(twi_scl, INPUT);
}

static void twi_delay(unsigned char v){
  unsigned int i;
  unsigned int reg;
  for(i=0;i<v;i++) reg = GPI;
}

static bool twi_write_start(void) {
  SCL_HIGH();
  SDA_HIGH();
  if (SDA_READ() == 0) return false;
  twi_delay(twi_dcount);
  SDA_LOW();
  twi_delay(twi_dcount);
  return true;
}

static bool twi_write_stop(void){
  unsigned int i = 0;
  SCL_LOW();
  SDA_LOW();
  twi_delay(twi_dcount);
  SCL_HIGH();
  while (SCL_READ() == 0 && (i++) < TWI_CLOCK_STRETCH);// Clock stretching (up to 100us)
  twi_delay(twi_dcount);
  SDA_HIGH();
  twi_delay(twi_dcount);
  
  return true;
}

static bool twi_write_bit(bool bit) {
  unsigned int i = 0;
  SCL_LOW();
  if (bit) SDA_HIGH();
  else SDA_LOW();
  twi_delay(twi_dcount+1);
  SCL_HIGH();
  while (SCL_READ() == 0 && (i++) < TWI_CLOCK_STRETCH);// Clock stretching (up to 100us)
  twi_delay(twi_dcount+1);
  return true;
}

static bool twi_read_bit(void) {
  unsigned int i = 0;
  SCL_LOW();
  SDA_HIGH();
  twi_delay(twi_dcount+1);
  SCL_HIGH();
  while (SCL_READ() == 0 && (i++) < TWI_CLOCK_STRETCH);// Clock stretching (up to 100us)
  bool bit = SDA_READ();
  twi_delay(twi_dcount);
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
  if(!twi_write_byte(((address << 1) | 0) & 0xFF)) return 2;//received NACK on transmit of address
  for(i=0; i<len; i++){
    if(!twi_write_byte(buf[i])) return 3;//received NACK on transmit of data
  }
  if(sendStop) twi_write_stop();
  return 0;
}

unsigned char twi_readFrom(unsigned char address, unsigned char* buf, unsigned int len, unsigned char sendStop){
  unsigned int i;
  if(!twi_write_start()) return 4;//line busy
  if(!twi_write_byte(((address << 1) | 1) & 0xFF)) return 2;//received NACK on transmit of address
  for(i=0; i<len; i++) buf[i] = twi_read_byte(false);
  if(sendStop){
    twi_write_stop();
    i = 0;
    while(SDA_READ() == 0 && (i++) < 10){
      SCL_LOW();
      twi_delay(twi_dcount);
      SCL_HIGH();
      twi_delay(twi_dcount);
    }
  } 
  return 0;
}
