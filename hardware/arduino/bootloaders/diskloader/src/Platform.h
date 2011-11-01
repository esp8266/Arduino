/*
 *  Platform.h
 */

#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/boot.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C"{
#endif

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

#define USB_PID_LEONARDO 0x0032
#define USB_PID_MICRO 0x0033
#define USB_VID 0x2341	// arduino LLC vid
#define USB_PID ARDUINO_MODEL_PID	// passed in by Makefile - 0x0032 for Leonardo, 0x0033 for Micro

#define min(a,b) ((a)<(b)?(a):(b))

#define CPU_PRESCALE(n)	(CLKPR = 0x80, CLKPR = (n))
#define DISABLE_JTAG()  MCUCR = (1 << JTD) | (1 << IVCE) | (0 << PUD); MCUCR = (1 << JTD) | (0 << IVSEL) | (0 << IVCE) | (0 << PUD)

#define BOARD_INIT()	DDRC |= (1<<7); DDRB |= (1<<0); DDRE |= (1<<6); CPU_PRESCALE(0); DISABLE_JTAG();	// for XXX-series boards
//#define BOARD_INIT()	DDRC |= (1<<7); DDRB |= (1<<0); DDRD |= (1<<5); CPU_PRESCALE(0); DISABLE_JTAG();	// for non-XXX boards

// for XXX-series boards
#define TX_LED_OFF()		PORTE |= (1<<6)
#define TX_LED_ON()			PORTE &= ~(1<<6)
#define RX_LED_OFF()		PORTB |= (1<<0)
#define RX_LED_ON()			PORTB &= ~(1<<0)
#define L_LED_OFF()			PORTC &= ~(1<<7)
#define L_LED_ON()			PORTC |= (1<<7)
#define L_LED_TOGGLE()		PORTC ^= (1<<7)

// these for non-XXX boards
//#define LED0			PORTC &= ~(1<<7)
//#define LED1			PORTC |= (1<<7)
//#define TXLED0			PORTD |= (1<<5)
//#define TXLED1			PORTD &= ~(1<<5)
//#define RXLED0			PORTB |= (1<<0)
//#define RXLED1			PORTB &= ~(1<<0)

#ifdef __cplusplus
}	/* extern "C"{ */
#endif 

#include "USBDesc.h"
#include "USBCore.h"	
#include "USBAPI.h"

#endif /* __PLATFORM_H__ */
