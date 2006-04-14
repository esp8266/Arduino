/*
  WConstants.h - Main definitions file for Wiring 
  Part of the Wiring project - http://wiring.org.co

  Copyright (c) 2004-2005 Hernando Barragan

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef WConstants_h
#define WConstants_h

// Wiring API version for libraries
// this is also defined at compile-time
#ifndef WIRING
#define WIRING 6
#endif

// passed in at compile-time
#ifndef F_CPU
#define F_CPU 16000000L
#endif

// passed in at compile-time
#ifndef CPU_FREQ
#define CPU_FREQ 16000000L
#endif

#define LOW  0x0
#define HIGH 0x1

#define INPUT  0x0
#define OUTPUT 0x1

#define true  0x1
#define false 0x0

#define PI         (3.1415927)
#define TWO_PI     (6.2831854)
#define HALF_PI    (1.57079)
#define EPSILON    (0.0001)
#define DEG_TO_RAD (0.01745329)
#define RAD_TO_DEG (57.2957786)

#define int(x)     ((int)(x))
#define char(x)    ((char)(x))
#define long(x)    ((long)(x))
#define byte(x)    ((uint8_t)(x))
#define float(x)   ((float)(x))
#define boolean(x) ((uint8_t)((x)==0?0:1))

// undefine stdlib's abs if encountered
#ifdef abs
#undef abs
#endif

#define sq(x)        ((x)*(x))
#define abs(x)       ((x)>0?(x):-(x))
#define min(a,b)     ((a)<(b)?(a):(b))
#define max(a,b)     ((a)>(b)?(a):(b))
#define round(x)     ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))
#define radians(deg) ((deg)*DEG_TO_RAD)
#define degrees(rad) ((rad)*RAD_TO_DEG)
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

#define WPIN0 (1<<0)
#define WPIN1 (1<<1)
#define WPIN2 (1<<2)
#define WPIN3 (1<<3)
#define WPIN4 (1<<4)
#define WPIN5 (1<<5)
#define WPIN6 (1<<6)
#define WPIN7 (1<<7)

#define WPWMPIN5 (1<<5) // PINB5
#define WPWMPIN4 (1<<6) // PINB6
#define WPWMPIN3 (1<<7) // PINB7
#define WPWMPIN2 (1<<3) // PINE3
#define WPWMPIN1 (1<<4) // PINE4
#define WPWMPIN0 (1<<5) // PINE5

#define WPORTA PORTA
#define WPORTB PORTB
#define WPORTC PORTC
#define WPORTD PORTD
#define WPORTE PORTE
#define WPORTF PORTF
#define WPORTG PORTG

#define WPINA PINA
#define WPINB PINB
#define WPINC PINC
#define WPIND PIND
#define WPINE PINE
#define WPINF PINF
#define WPING PING

#define WDDRA DDRA
#define WDDRB DDRB
#define WDDRC DDRC
#define WDDRD DDRD
#define WDDRE DDRE
#define WDDRF DDRF
#define WDDRG DDRG

#define TIMER0OVERFLOW_INT      0
#define TIMER0OUTCOMPARE_INT    1
#define TIMER1OVERFLOW_INT      2
#define TIMER1OUTCOMPAREA_INT   3
#define TIMER1OUTCOMPAREB_INT   4
#define TIMER1OUTCOMPAREC_INT   5
#define TIMER1INPUTCAPTURE_INT  6
#define TIMER2OVERFLOW_INT      7
#define TIMER2OUTCOMPARE_INT    8
#define TIMER3OVERFLOW_INT      9
#define TIMER3OUTCOMPAREA_INT   10
#define TIMER3OUTCOMPAREB_INT   11
#define TIMER3OUTCOMPAREC_INT   12
#define TIMER3INPUTCAPTURE_INT  13

#define TIMER_NUM_INTERRUPTS    14

#define TIMER_CLK_STOP      0x00
#define TIMER_CLK_DIV1      0x01
#define TIMER_CLK_DIV8      0x02
#define TIMER_CLK_DIV64     0x03
#define TIMER_CLK_DIV256    0x04
#define TIMER_CLK_DIV1024   0x05
#define TIMER_CLK_T_FALL    0x06
#define TIMER_CLK_T_RISE    0x07
#define TIMER_PRESCALE_MASK 0x07

#define TIMERRTC_CLK_STOP      0x00
#define TIMERRTC_CLK_DIV1      0x01
#define TIMERRTC_CLK_DIV8      0x02
#define TIMERRTC_CLK_DIV32     0x03
#define TIMERRTC_CLK_DIV64     0x04
#define TIMERRTC_CLK_DIV128    0x05
#define TIMERRTC_CLK_DIV256    0x06
#define TIMERRTC_CLK_DIV1024   0x07
#define TIMERRTC_PRESCALE_MASK 0x07

#define TIMER0PRESCALE TIMERRTC_CLK_DIV64
#define TIMER1PRESCALE TIMER_CLK_DIV64
#define TIMER2PRESCALE TIMER_CLK_DIV8
#define TIMER3PRESCALE TIMER_CLK_DIV64

#define EXTERNAL_INT_0 0
#define EXTERNAL_INT_1 1
#define EXTERNAL_INT_2 2
#define EXTERNAL_INT_3 3
#define EXTERNAL_INT_4 4
#define EXTERNAL_INT_5 5
#define EXTERNAL_INT_6 6
#define EXTERNAL_INT_7 7

#define EXTERNAL_NUM_INTERRUPTS 8

typedef uint8_t byte;
typedef uint8_t boolean;
typedef void (*voidFuncPtr)(void);

#endif

