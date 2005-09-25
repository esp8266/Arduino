/*
  WConstats.h - main definitions file for Wiring 
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

#define PI (3.1415927)
#define HALF_PI (1.57079)
#define TWO_PI (6.2831854)
#define DEG_TO_RAD (0.01745329)
#define RAD_TO_DEG (57.2957786)
#define EPSILON (0.0001)

#define WPIN0  (1<<0)
#define WPIN1  (1<<1)
#define WPIN2  (1<<2)
#define WPIN3  (1<<3)
#define WPIN4  (1<<4)
#define WPIN5  (1<<5)
#define WPIN6  (1<<6)
#define WPIN7  (1<<7)

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


#define HIGH 0x1
#define LOW  0x0

#define INPUT 0x0
#define OUTPUT 0x1

#define true 0x1
#define false 0x0

#define CPU_FREQ 16000000L  

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

#define TIMER_CLK_STOP		0x00
#define TIMER_CLK_DIV1		0x01
#define TIMER_CLK_DIV8		0x02
#define TIMER_CLK_DIV64		0x03
#define TIMER_CLK_DIV256	0x04
#define TIMER_CLK_DIV1024	0x05
#define TIMER_CLK_T_FALL	0x06
#define TIMER_CLK_T_RISE	0x07
#define TIMER_PRESCALE_MASK	0x07

#define TIMERRTC_CLK_STOP	0x00
#define TIMERRTC_CLK_DIV1	0x01
#define TIMERRTC_CLK_DIV8	0x02
#define TIMERRTC_CLK_DIV32	0x03
#define TIMERRTC_CLK_DIV64	0x04
#define TIMERRTC_CLK_DIV128	0x05
#define TIMERRTC_CLK_DIV256	0x06
#define TIMERRTC_CLK_DIV1024	0x07
#define TIMERRTC_PRESCALE_MASK	0x07

#define TIMER0PRESCALE TIMERRTC_CLK_DIV64 
#define TIMER1PRESCALE TIMER_CLK_DIV64	  
#define TIMER2PRESCALE TIMER_CLK_DIV8	 
#define TIMER3PRESCALE TIMER_CLK_DIV64

#ifndef __cplusplus
volatile struct {
  uint8_t rxInt : 1;
  uint8_t rx1Int : 1;
  uint8_t uart0TxComplete : 1;
  uint8_t uart1TxComplete : 1;
} event;
#endif

typedef struct wbuffer_t {
  uint16_t   buflen;
  uint16_t   bufcnt;
  uint8_t  * in;
  uint8_t  * out;
  uint8_t  * buf;
} WBuffer;

typedef struct {
  void (*begin) (int);
  uint8_t (*available) ();
  int (*read) ();
  void (*write) (int);
  void (*writeBytes) (char[]);
} WSerial;

extern WSerial Serial1;
extern WSerial Serial;

typedef uint8_t boolean;
typedef uint8_t byte;

//int serial;
#define DEF_UART  0
#define AUX_UART  1
#define SERIAL0 0
#define SERIAL1 1

#define UART_BUF_LEN 32



#define min(a,b)               ((a<b)?(a):(b))
#define max(a,b)               ((a>b)?(a):(b))
#define abs(x)	               ((x>0)?(x):(-x))
#define constrain(amt,low,high) ((amt<low)?(low):((amt>high)?(high):(amt)))
#define radians(deg) ((deg)*DEG_TO_RAD)
#define degrees(rad) ((rad)*RAD_TO_DEG)
#define sq(x) ((x)*(x))

#define SERVO_NUM_CHANNELS     8
#define SERVO_MAX              142 
#define SERVO_MIN	       34 
#define POSITION_MAX	       255 
#define SERVO_DEFAULT_PORT     PORTA


//static char uartBuffer[UART_BUF_LEN];
//WBuffer uartbuf;
typedef void (*voidFuncPtru08)(void);
volatile static voidFuncPtru08 uartRxFunc;
typedef void (*voidFuncPtr)(void);


#define WD_CTRL_PORT  PORTC
#define WD_CTRL_DDR   DDRC
#define WD_CTRL_RS    0
#define WD_CTRL_RW    1
#define WD_CTRL_E     2
#define WD_DATA_POUT  PORTA
#define WD_DATA_PIN   PINA
#define WD_DATA_DDR   DDRA
#define WD_LINE0_DDRAMADDR  0x00
#define WD_LINE1_DDRAMADDR  0x40
#define WD_LINE2_DDRAMADDR  0x14
#define WD_LINE3_DDRAMADDR  0x54

#define WD_CLR             0      
#define WD_HOME            1      
#define WD_ENTRY_MODE      2      
#define WD_ENTRY_INC       1      
#define WD_ENTRY_SHIFT     0      
#define WD_ON_CTRL         3      
#define WD_ON_DISPLAY      2      
#define WD_ON_CURSOR       1      
#define WD_ON_BLINK        0      
#define WD_MOVE            4      
#define WD_MOVE_DISP       3      
#define WD_MOVE_RIGHT      2     
#define WD_FUNCTION        5      
#define WD_FUNCTION_8BIT   4      
#define WD_FUNCTION_2LINES 3      
#define WD_FUNCTION_10DOTS 2     
#define WD_CGRAM           6      
#define WD_DDRAM           7      
#define WD_BUSY            7      

#define DATA_8_BITS	0x0
#define DATA_4_BITS	0x1

#define SERIAL	0x0
#define DISPLAY 0x1

#define EXTERNAL_NUM_INTERRUPTS 8
#define NUM_ENCODERS 4
#define EXTERNAL_INT_0 0
#define EXTERNAL_INT_1 1
#define EXTERNAL_INT_2 2
#define EXTERNAL_INT_3 3
#define EXTERNAL_INT_4 4
#define EXTERNAL_INT_5 5
#define EXTERNAL_INT_6 6
#define EXTERNAL_INT_7 7

#ifndef __cplusplus
void print(const char *template, ...);
void pinMode(uint8_t, uint8_t);
void digitalWrite(uint8_t, uint8_t);
void delay(int);
#endif

#endif
