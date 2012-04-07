/* Copyright (c) 2003  Theodore A. Roth
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.

   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.

   * Neither the name of the copyright holders nor the names of
     contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE. */

/* $Id: portpins.h,v 1.4 2006/03/10 16:15:38 aesok Exp $ */

#ifndef _AVR_PORTPINS_H_
#define _AVR_PORTPINS_H_ 1

/* This file should only be included from <avr/io.h>, never directly. */

#ifndef _AVR_IO_H_
#  error "Include <avr/io.h> instead of this file."
#endif

/* Define Generic PORTn, DDn, and PINn values. */

/* Port Data Register (generic) */
#define    PORT7        7
#define    PORT6        6
#define    PORT5        5
#define    PORT4        4
#define    PORT3        3
#define    PORT2        2
#define    PORT1        1
#define    PORT0        0

/* Port Data Direction Register (generic) */
#define    DD7          7
#define    DD6          6
#define    DD5          5
#define    DD4          4
#define    DD3          3
#define    DD2          2
#define    DD1          1
#define    DD0          0

/* Port Input Pins (generic) */
#define    PIN7         7
#define    PIN6         6
#define    PIN5         5
#define    PIN4         4
#define    PIN3         3
#define    PIN2         2
#define    PIN1         1
#define    PIN0         0

/* Define PORTxn values for all possible port pins. */

/* PORT A */

#if defined(PA0)
#  define PORTA0 PA0
#endif
#if defined(PA1)
#  define PORTA1 PA1
#endif
#if defined(PA2)
#  define PORTA2 PA2
#endif
#if defined(PA3)
#  define PORTA3 PA3
#endif
#if defined(PA4)
#  define PORTA4 PA4
#endif
#if defined(PA5)
#  define PORTA5 PA5
#endif
#if defined(PA6)
#  define PORTA6 PA6
#endif
#if defined(PA7)
#  define PORTA7 PA7
#endif

/* PORT B */

#if defined(PB0)
#  define PORTB0 PB0
#endif
#if defined(PB1)
#  define PORTB1 PB1
#endif
#if defined(PB2)
#  define PORTB2 PB2
#endif
#if defined(PB3)
#  define PORTB3 PB3
#endif
#if defined(PB4)
#  define PORTB4 PB4
#endif
#if defined(PB5)
#  define PORTB5 PB5
#endif
#if defined(PB6)
#  define PORTB6 PB6
#endif
#if defined(PB7)
#  define PORTB7 PB7
#endif

/* PORT C */

#if defined(PC0)
#  define PORTC0 PC0
#endif
#if defined(PC1)
#  define PORTC1 PC1
#endif
#if defined(PC2)
#  define PORTC2 PC2
#endif
#if defined(PC3)
#  define PORTC3 PC3
#endif
#if defined(PC4)
#  define PORTC4 PC4
#endif
#if defined(PC5)
#  define PORTC5 PC5
#endif
#if defined(PC6)
#  define PORTC6 PC6
#endif
#if defined(PC7)
#  define PORTC7 PC7
#endif

/* PORT D */

#if defined(PD0)
#  define PORTD0 PD0
#endif
#if defined(PD1)
#  define PORTD1 PD1
#endif
#if defined(PD2)
#  define PORTD2 PD2
#endif
#if defined(PD3)
#  define PORTD3 PD3
#endif
#if defined(PD4)
#  define PORTD4 PD4
#endif
#if defined(PD5)
#  define PORTD5 PD5
#endif
#if defined(PD6)
#  define PORTD6 PD6
#endif
#if defined(PD7)
#  define PORTD7 PD7
#endif

/* PORT E */

#if defined(PE0)
#  define PORTE0 PE0
#endif
#if defined(PE1)
#  define PORTE1 PE1
#endif
#if defined(PE2)
#  define PORTE2 PE2
#endif
#if defined(PE3)
#  define PORTE3 PE3
#endif
#if defined(PE4)
#  define PORTE4 PE4
#endif
#if defined(PE5)
#  define PORTE5 PE5
#endif
#if defined(PE6)
#  define PORTE6 PE6
#endif
#if defined(PE7)
#  define PORTE7 PE7
#endif

/* PORT F */

#if defined(PF0)
#  define PORTF0 PF0
#endif
#if defined(PF1)
#  define PORTF1 PF1
#endif
#if defined(PF2)
#  define PORTF2 PF2
#endif
#if defined(PF3)
#  define PORTF3 PF3
#endif
#if defined(PF4)
#  define PORTF4 PF4
#endif
#if defined(PF5)
#  define PORTF5 PF5
#endif
#if defined(PF6)
#  define PORTF6 PF6
#endif
#if defined(PF7)
#  define PORTF7 PF7
#endif

/* PORT G */

#if defined(PG0)
#  define PORTG0 PG0
#endif
#if defined(PG1)
#  define PORTG1 PG1
#endif
#if defined(PG2)
#  define PORTG2 PG2
#endif
#if defined(PG3)
#  define PORTG3 PG3
#endif
#if defined(PG4)
#  define PORTG4 PG4
#endif
#if defined(PG5)
#  define PORTG5 PG5
#endif
#if defined(PG6)
#  define PORTG6 PG6
#endif
#if defined(PG7)
#  define PORTG7 PG7
#endif

/* PORT H */

#if defined(PH0)
#  define PORTH0 PH0
#endif
#if defined(PH1)
#  define PORTH1 PH1
#endif
#if defined(PH2)
#  define PORTH2 PH2
#endif
#if defined(PH3)
#  define PORTH3 PH3
#endif
#if defined(PH4)
#  define PORTH4 PH4
#endif
#if defined(PH5)
#  define PORTH5 PH5
#endif
#if defined(PH6)
#  define PORTH6 PH6
#endif
#if defined(PH7)
#  define PORTH7 PH7
#endif

/* PORT J */

#if defined(PJ0)
#  define PORTJ0 PJ0
#endif
#if defined(PJ1)
#  define PORTJ1 PJ1
#endif
#if defined(PJ2)
#  define PORTJ2 PJ2
#endif
#if defined(PJ3)
#  define PORTJ3 PJ3
#endif
#if defined(PJ4)
#  define PORTJ4 PJ4
#endif
#if defined(PJ5)
#  define PORTJ5 PJ5
#endif
#if defined(PJ6)
#  define PORTJ6 PJ6
#endif
#if defined(PJ7)
#  define PORTJ7 PJ7
#endif

/* PORT K */

#if defined(PK0)
#  define PORTK0 PK0
#endif
#if defined(PK1)
#  define PORTK1 PK1
#endif
#if defined(PK2)
#  define PORTK2 PK2
#endif
#if defined(PK3)
#  define PORTK3 PK3
#endif
#if defined(PK4)
#  define PORTK4 PK4
#endif
#if defined(PK5)
#  define PORTK5 PK5
#endif
#if defined(PK6)
#  define PORTK6 PK6
#endif
#if defined(PK7)
#  define PORTK7 PK7
#endif

/* PORT L */

#if defined(PL0)
#  define PORTL0 PL0
#endif
#if defined(PL1)
#  define PORTL1 PL1
#endif
#if defined(PL2)
#  define PORTL2 PL2
#endif
#if defined(PL3)
#  define PORTL3 PL3
#endif
#if defined(PL4)
#  define PORTL4 PL4
#endif
#if defined(PL5)
#  define PORTL5 PL5
#endif
#if defined(PL6)
#  define PORTL6 PL6
#endif
#if defined(PL7)
#  define PORTL7 PL7
#endif

#endif /* _AVR_PORTPINS_H_ */
