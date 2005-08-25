/*
  wiring.h - Wiring API Partial Implementation
  Part of Arduino / Wiring Lite

  Copyright (c) 2005 David A. Mellis

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA  02111-1307  USA

  $Id: wiring.h,v 1.4 2005/05/24 17:47:41 mellis Exp $
*/

#ifndef Wiring_h
#define Wiring_h


#define HIGH 0x1
#define LOW  0x0

#define INPUT 0x0
#define OUTPUT 0x1

#define true 0x1
#define false 0x0


#define NOT_A_PIN 0
#define NOT_A_PORT -1

void delay(unsigned long);
void delay_ms(unsigned short ms);
void pinMode(int, int);
void digitalWrite(int, int);
int digitalRead(int);
void analogWrite(int, int);
int analogRead(int);
unsigned long millis(void);
void setup(void);
void loop(void);
void beginSerial(int);
void serialWrite(unsigned char);

typedef struct {
	int port;
	int bit;
} pin_t;

extern int port_to_mode[];
extern int port_to_input[];
extern int port_to_output[];
extern pin_t *digital_pin_to_port;
extern pin_t *analog_in_pin_to_port;
extern pin_t *analog_out_pin_to_port;

#endif
