/*! \file timer.c \brief System Timer function library. */
//*****************************************************************************
//
// File Name	: 'timer.c'
// Title		: System Timer function library
// Author		: Pascal Stang - Copyright (C) 2000-2002
// Created		: 11/22/2000
// Revised		: 07/09/2003
// Version		: 1.1
// Target MCU	: Atmel AVR Series
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef WIN32
	#include <avr/io.h>
	#include <avr/signal.h>
	#include <avr/interrupt.h>
	#include <avr/pgmspace.h>
	#include <avr/sleep.h>
#endif

#include "global.h"
#include "timer.h"

#include "rprintf.h"

// Program ROM constants
// the prescale division values stored in order of timer control register index
// STOP, CLK, CLK/8, CLK/64, CLK/256, CLK/1024
unsigned short __attribute__ ((progmem)) TimerPrescaleFactor[] = {0,1,8,64,256,1024};
// the prescale division values stored in order of timer control register index
// STOP, CLK, CLK/8, CLK/32, CLK/64, CLK/128, CLK/256, CLK/1024
unsigned short __attribute__ ((progmem)) TimerRTCPrescaleFactor[] = {0,1,8,32,64,128,256,1024};

// Global variables
// time registers
volatile unsigned long TimerPauseReg;
volatile unsigned long Timer0Reg0;
volatile unsigned long Timer2Reg0;

typedef void (*voidFuncPtr)(void);
volatile static voidFuncPtr TimerIntFunc[TIMER_NUM_INTERRUPTS];

// delay for a minimum of <us> microseconds 
// the time resolution is dependent on the time the loop takes 
// e.g. with 4Mhz and 5 cycles per loop, the resolution is 1.25 us 
void delay_us(unsigned short time_us) 
{
	unsigned short delay_loops;
	register unsigned short i;

	delay_loops = (time_us+3)/5*CYCLES_PER_US; // +3 for rounding up (dirty) 

	// one loop takes 5 cpu cycles 
	for (i=0; i < delay_loops; i++) {};
}
/*
void delay_ms(unsigned char time_ms)
{
	unsigned short delay_count = F_CPU / 4000;

	unsigned short cnt;
	asm volatile ("\n"
                  "L_dl1%=:\n\t"
                  "mov %A0, %A2\n\t"
                  "mov %B0, %B2\n"
                  "L_dl2%=:\n\t"
                  "sbiw %A0, 1\n\t"
                  "brne L_dl2%=\n\t"
                  "dec %1\n\t" "brne L_dl1%=\n\t":"=&w" (cnt)
                  :"r"(time_ms), "r"((unsigned short) (delay_count))
	);
}
*/
void timerInit(void)
{
	u08 intNum;
	// detach all user functions from interrupts
	for(intNum=0; intNum<TIMER_NUM_INTERRUPTS; intNum++)
		timerDetach(intNum);

	// initialize all timers
	timer0Init();
	timer1Init();
	#ifdef TCNT2	// support timer2 only if it exists
	timer2Init();
	#endif
	// enable interrupts
	sei();
}

void timer0Init()
{
	// initialize timer 0
	timer0SetPrescaler( TIMER0PRESCALE );	// set prescaler
	outb(TCNT0, 0);							// reset TCNT0
	sbi(TIMSK, TOIE0);						// enable TCNT0 overflow interrupt

	timer0ClearOverflowCount();				// initialize time registers
}

void timer1Init(void)
{
	// initialize timer 1
	timer1SetPrescaler( TIMER1PRESCALE );	// set prescaler
	outb(TCNT1H, 0);						// reset TCNT1
	outb(TCNT1L, 0);
	sbi(TIMSK, TOIE1);						// enable TCNT1 overflow
}

#ifdef TCNT2	// support timer2 only if it exists
void timer2Init(void)
{
	// initialize timer 2
	timer2SetPrescaler( TIMER2PRESCALE );	// set prescaler
	outb(TCNT2, 0);							// reset TCNT2
	sbi(TIMSK, TOIE2);						// enable TCNT2 overflow

	timer2ClearOverflowCount();				// initialize time registers
}
#endif

void timer0SetPrescaler(u08 prescale)
{
	// set prescaler on timer 0
	outb(TCCR0, (inb(TCCR0) & ~TIMER_PRESCALE_MASK) | prescale);
}

void timer1SetPrescaler(u08 prescale)
{
	// set prescaler on timer 1
	outb(TCCR1B, (inb(TCCR1B) & ~TIMER_PRESCALE_MASK) | prescale);
}

#ifdef TCNT2	// support timer2 only if it exists
void timer2SetPrescaler(u08 prescale)
{
	// set prescaler on timer 2
	outb(TCCR2, (inb(TCCR2) & ~TIMER_PRESCALE_MASK) | prescale);
}
#endif

u16 timer0GetPrescaler(void)
{
	// get the current prescaler setting
	return (pgm_read_word(TimerPrescaleFactor+(inb(TCCR0) & TIMER_PRESCALE_MASK)));
}

u16 timer1GetPrescaler(void)
{
	// get the current prescaler setting
	return (pgm_read_word(TimerPrescaleFactor+(inb(TCCR1B) & TIMER_PRESCALE_MASK)));
}

#ifdef TCNT2	// support timer2 only if it exists
u16 timer2GetPrescaler(void)
{
	//TODO: can we assume for all 3-timer AVR processors,
	// that timer2 is the RTC timer?

	// get the current prescaler setting
	return (pgm_read_word(TimerRTCPrescaleFactor+(inb(TCCR2) & TIMER_PRESCALE_MASK)));
}
#endif

void timerAttach(u08 interruptNum, void (*userFunc)(void) )
{
	// make sure the interrupt number is within bounds
	if(interruptNum < TIMER_NUM_INTERRUPTS)
	{
		// set the interrupt function to run
		// the supplied user's function
		TimerIntFunc[interruptNum] = userFunc;
	}
}

void timerDetach(u08 interruptNum)
{
	// make sure the interrupt number is within bounds
	if(interruptNum < TIMER_NUM_INTERRUPTS)
	{
		// set the interrupt function to run nothing
		TimerIntFunc[interruptNum] = 0;
	}
}
/*
u32 timerMsToTics(u16 ms)
{
	// calculate the prescaler division rate
	u16 prescaleDiv = 1<<(pgm_read_byte(TimerPrescaleFactor+inb(TCCR0)));
	// calculate the number of timer tics in x milliseconds
	return (ms*(F_CPU/(prescaleDiv*256)))/1000;
}

u16 timerTicsToMs(u32 tics)
{
	// calculate the prescaler division rate
	u16 prescaleDiv = 1<<(pgm_read_byte(TimerPrescaleFactor+inb(TCCR0)));
	// calculate the number of milliseconds in x timer tics
	return (tics*1000*(prescaleDiv*256))/F_CPU;
}
*/
void timerPause(unsigned short pause_ms)
{
	// pauses for exactly <pause_ms> number of milliseconds
	u08 timerThres;
	u32 ticRateHz;
	u32 pause;

	// capture current pause timer value
	timerThres = inb(TCNT0);
	// reset pause timer overflow count
	TimerPauseReg = 0;
	// calculate delay for [pause_ms] milliseconds
	// prescaler division = 1<<(pgm_read_byte(TimerPrescaleFactor+inb(TCCR0)))
	ticRateHz = F_CPU/timer0GetPrescaler();
	// precision management
	// prevent overflow and precision underflow
	//	-could add more conditions to improve accuracy
	if( ((ticRateHz < 429497) && (pause_ms <= 10000)) )
		pause = (pause_ms*ticRateHz)/1000;
	else
		pause = pause_ms*(ticRateHz/1000);

	// loop until time expires
	while( ((TimerPauseReg<<8) | inb(TCNT0)) < (pause+timerThres) )
	{
		if( TimerPauseReg < (pause>>8));
		{
			// save power by idling the processor
			set_sleep_mode(SLEEP_MODE_IDLE);
			sleep_mode();
		}
	}

	/* old inaccurate code, for reference
	
	// calculate delay for [pause_ms] milliseconds
	u16 prescaleDiv = 1<<(pgm_read_byte(TimerPrescaleFactor+inb(TCCR0)));
	u32 pause = (pause_ms*(F_CPU/(prescaleDiv*256)))/1000;
	
	TimerPauseReg = 0;
	while(TimerPauseReg < pause);

	*/
}

void timer0ClearOverflowCount(void)
{
	// clear the timer overflow counter registers
	Timer0Reg0 = 0;	// initialize time registers
}

long timer0GetOverflowCount(void)
{
	// return the current timer overflow count
	// (this is since the last timer0ClearOverflowCount() command was called)
	return Timer0Reg0;
}

#ifdef TCNT2	// support timer2 only if it exists
void timer2ClearOverflowCount(void)
{
	// clear the timer overflow counter registers
	Timer2Reg0 = 0;	// initialize time registers
}

long timer2GetOverflowCount(void)
{
	// return the current timer overflow count
	// (this is since the last timer2ClearOverflowCount() command was called)
	return Timer2Reg0;
}
#endif

void timer1PWMInit(u08 bitRes)
{
	// configures timer1 for use with PWM output
	// on OC1A and OC1B pins

	// enable timer1 as 8,9,10bit PWM
	if(bitRes == 9)
	{	// 9bit mode
		sbi(TCCR1A,PWM11);
		cbi(TCCR1A,PWM10);
	}
	else if( bitRes == 10 )
	{	// 10bit mode
		sbi(TCCR1A,PWM11);
		sbi(TCCR1A,PWM10);
	}
	else
	{	// default 8bit mode
		cbi(TCCR1A,PWM11);
		sbi(TCCR1A,PWM10);
	}

	// clear output compare value A
	outb(OCR1AH, 0);
	outb(OCR1AL, 0);
	// clear output compare value B
	outb(OCR1BH, 0);
	outb(OCR1BL, 0);
}

#ifdef WGM10
// include support for arbitrary top-count PWM
// on new AVR processors that support it
void timer1PWMInitICR(u16 topcount)
{
	// set PWM mode with ICR top-count
	cbi(TCCR1A,WGM10);
	sbi(TCCR1A,WGM11);
	sbi(TCCR1B,WGM12);
	sbi(TCCR1B,WGM13);
	
	// set top count value
	ICR1 = topcount;
	
	// clear output compare value A
	OCR1A = 0;
	// clear output compare value B
	OCR1B = 0;

}
#endif

void timer1PWMOff(void)
{
	// turn off timer1 PWM mode
	cbi(TCCR1A,PWM11);
	cbi(TCCR1A,PWM10);
	// set PWM1A/B (OutputCompare action) to none
	timer1PWMAOff();
	timer1PWMBOff();
}

void timer1PWMAOn(void)
{
	// turn on channel A (OC1A) PWM output
	// set OC1A as non-inverted PWM
	sbi(TCCR1A,COM1A1);
	cbi(TCCR1A,COM1A0);
}

void timer1PWMBOn(void)
{
	// turn on channel B (OC1B) PWM output
	// set OC1B as non-inverted PWM
	sbi(TCCR1A,COM1B1);
	cbi(TCCR1A,COM1B0);
}

void timer1PWMAOff(void)
{
	// turn off channel A (OC1A) PWM output
	// set OC1A (OutputCompare action) to none
	cbi(TCCR1A,COM1A1);
	cbi(TCCR1A,COM1A0);
}

void timer1PWMBOff(void)
{
	// turn off channel B (OC1B) PWM output
	// set OC1B (OutputCompare action) to none
	cbi(TCCR1A,COM1B1);
	cbi(TCCR1A,COM1B0);
}

void timer1PWMASet(u16 pwmDuty)
{
	// set PWM (output compare) duty for channel A
	// this PWM output is generated on OC1A pin
	// NOTE:	pwmDuty should be in the range 0-255 for 8bit PWM
	//			pwmDuty should be in the range 0-511 for 9bit PWM
	//			pwmDuty should be in the range 0-1023 for 10bit PWM
	//outp( (pwmDuty>>8), OCR1AH);		// set the high 8bits of OCR1A
	//outp( (pwmDuty&0x00FF), OCR1AL);	// set the low 8bits of OCR1A
	OCR1A = pwmDuty;
}

void timer1PWMBSet(u16 pwmDuty)
{
	// set PWM (output compare) duty for channel B
	// this PWM output is generated on OC1B pin
	// NOTE:	pwmDuty should be in the range 0-255 for 8bit PWM
	//			pwmDuty should be in the range 0-511 for 9bit PWM
	//			pwmDuty should be in the range 0-1023 for 10bit PWM
	//outp( (pwmDuty>>8), OCR1BH);		// set the high 8bits of OCR1B
	//outp( (pwmDuty&0x00FF), OCR1BL);	// set the low 8bits of OCR1B
	OCR1B = pwmDuty;
}

//! Interrupt handler for tcnt0 overflow interrupt
TIMER_INTERRUPT_HANDLER(SIG_OVERFLOW0)
{
	Timer0Reg0++;			// increment low-order counter

	// increment pause counter
	TimerPauseReg++;

	// if a user function is defined, execute it too
	if(TimerIntFunc[TIMER0OVERFLOW_INT])
		TimerIntFunc[TIMER0OVERFLOW_INT]();
}

//! Interrupt handler for tcnt1 overflow interrupt
TIMER_INTERRUPT_HANDLER(SIG_OVERFLOW1)
{
	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER1OVERFLOW_INT])
		TimerIntFunc[TIMER1OVERFLOW_INT]();
}

#ifdef TCNT2	// support timer2 only if it exists
//! Interrupt handler for tcnt2 overflow interrupt
TIMER_INTERRUPT_HANDLER(SIG_OVERFLOW2)
{
	Timer2Reg0++;			// increment low-order counter

	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER2OVERFLOW_INT])
		TimerIntFunc[TIMER2OVERFLOW_INT]();
}
#endif

#ifdef OCR0
// include support for Output Compare 0 for new AVR processors that support it
//! Interrupt handler for OutputCompare0 match (OC0) interrupt
TIMER_INTERRUPT_HANDLER(SIG_OUTPUT_COMPARE0)
{
	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER0OUTCOMPARE_INT])
		TimerIntFunc[TIMER0OUTCOMPARE_INT]();
}
#endif

//! Interrupt handler for CutputCompare1A match (OC1A) interrupt
TIMER_INTERRUPT_HANDLER(SIG_OUTPUT_COMPARE1A)
{
	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER1OUTCOMPAREA_INT])
		TimerIntFunc[TIMER1OUTCOMPAREA_INT]();
}

//! Interrupt handler for OutputCompare1B match (OC1B) interrupt
TIMER_INTERRUPT_HANDLER(SIG_OUTPUT_COMPARE1B)
{
	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER1OUTCOMPAREB_INT])
		TimerIntFunc[TIMER1OUTCOMPAREB_INT]();
}

//! Interrupt handler for InputCapture1 (IC1) interrupt
TIMER_INTERRUPT_HANDLER(SIG_INPUT_CAPTURE1)
{
	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER1INPUTCAPTURE_INT])
		TimerIntFunc[TIMER1INPUTCAPTURE_INT]();
}

//! Interrupt handler for OutputCompare2 match (OC2) interrupt
TIMER_INTERRUPT_HANDLER(SIG_OUTPUT_COMPARE2)
{
	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER2OUTCOMPARE_INT])
		TimerIntFunc[TIMER2OUTCOMPARE_INT]();
}
