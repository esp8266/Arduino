/* 
  i2s.h - Software I2S library for esp8266

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
#ifndef I2S_h
#define I2S_h

/*
How does this work? Basically, to get sound, you need to:
- Connect an I2S codec to the I2S pins on the ESP.
- Start up a thread that's going to do the sound output
- Call i2s_begin()
- Call i2s_set_rate() with the sample rate you want.
- Generate sound and call i2s_write_sample() with 32-bit samples.
The 32bit samples basically are 2 16-bit signed values (the analog values for
the left and right channel) concatenated as (Rout<<16)+Lout

i2s_write_sample will block when you're sending data too quickly, so you can just
generate and push data as fast as you can and i2s_write_sample will regulate the
speed.
*/

#ifdef __cplusplus
extern "C" {
#endif

void i2s_begin();
void i2s_end();
void i2s_set_rate(uint32_t rate);//Sample Rate in Hz (ex 44100, 48000)
void i2s_set_dividers(uint8_t div1, uint8_t div2);//Direct control over output rate
float i2s_get_real_rate();//The actual Sample Rate on output
bool i2s_write_sample(uint32_t sample);//32bit sample with channels being upper and lower 16 bits (blocking when DMA is full)
bool i2s_write_sample_nb(uint32_t sample);//same as above but does not block when DMA is full and returns false instead
bool i2s_write_lr(int16_t left, int16_t right);//combines both channels and calls i2s_write_sample with the result
bool i2s_is_full();//returns true if DMA is full and can not take more bytes (overflow)
bool i2s_is_empty();//returns true if DMA is empty (underflow)
int16_t i2s_available();// returns the number of samples than can be written before blocking

#ifdef __cplusplus
}
#endif

#endif
