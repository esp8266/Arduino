#define F_CPU 16000000UL 
#define CPU_FREQ 16000000L 
#define XTAL_CPU 16000000L
#define UART_BAUD_RATE 9600

#ifndef __AVR_ATmega8__
#define __AVR_ATmega8__
#endif

#define __MCU_CLOCK_FREQUENCY__ _16.0000_MHz

#include <avr/io.h>

#include "wiring.h"

#include <avr/interrupt.h>
#include <avr/signal.h>
