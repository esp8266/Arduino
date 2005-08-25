/*! \file timer128.c \brief System Timer function library for Mega128. */
//*****************************************************************************
//
// File Name	: 'timer128.c'
// Title		: System Timer function library for Mega128
// Author		: Pascal Stang - Copyright (C) 2000-2003
// Created		: 11/22/2000
// Revised		: 02/24/2003
// Version		: 1.2
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
#include "timer128.h"

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
volatile unsigned long Timer0Reg1;
volatile unsigned long Timer2Reg0;
volatile unsigned long Timer2Reg1;

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
	timer2Init();
	timer3Init();
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

void timer2Init(void)
{
	// initialize timer 2
	timer2SetPrescaler( TIMER2PRESCALE );	// set prescaler
	outb(TCNT2, 0);							// reset TCNT2
	sbi(TIMSK, TOIE2);						// enable TCNT2 overflow

	timer2ClearOverflowCount();				// initialize time registers
}

void timer3Init(void)
{
	// initialize timer 3
	timer3SetPrescaler( TIMER3PRESCALE );	// set prescaler
	outb(TCNT3H, 0);						// reset TCNT3
	outb(TCNT3L, 0);
	sbi(ETIMSK, TOIE3);						// enable TCNT3 overflow
}

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

void timer2SetPrescaler(u08 prescale)
{
	// set prescaler on timer 2
	outb(TCCR2, (inb(TCCR2) & ~TIMER_PRESCALE_MASK) | prescale);
}

void timer3SetPrescaler(u08 prescale)
{
	// set prescaler on timer 2
	outb(TCCR3B, (inp(TCCR3B) & ~TIMER_PRESCALE_MASK) | prescale);
}

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

u16 timer2GetPrescaler(void)
{
	// get the current prescaler setting
	return (pgm_read_word(TimerPrescaleFactor+(inb(TCCR2) & TIMER_PRESCALE_MASK)));
}

u16 timer3GetPrescaler(void)
{
	// get the current prescaler setting
	return (pgm_read_word(TimerPrescaleFactor+(inb(TCCR3B) & TIMER_PRESCALE_MASK)));
}

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

void timerPause(unsigned short pause_ms)
{
	// pauses for exactly <pause_ms> number of milliseconds
	u08 timerThres;
	u32 ticRateHz;
	u32 pause;

	// capture current pause timer value
	timerThres = inb(TCNT2);
	// reset pause timer overflow count
	TimerPauseReg = 0;
	// calculate delay for [pause_ms] milliseconds
	// prescaler division = 1<<(pgm_read_byte(TimerPrescaleFactor+inb(TCCR2)))
	ticRateHz = F_CPU/timer2GetPrescaler();
	// precision management
	// prevent overflow and precision underflow
	//	-could add more conditions to improve accuracy
	if( ((ticRateHz < 429497) && (pause_ms <= 10000)) )
		pause = (pause_ms*ticRateHz)/1000;
	else
		pause = pause_ms*(ticRateHz/1000);
	
	// loop until time expires
	while( ((TimerPauseReg<<8) | inb(TCNT2)) < (pause+timerThres) )
	{
		if( TimerPauseReg < (pause>>8));
		{
			// save power by idling the processor
			set_sleep_mode(SLEEP_MODE_IDLE);
			sleep_mode();
		}
	}
}

void timer0ClearOverflowCount(void)
{
	// clear the timer overflow counter registers
	Timer0Reg0 = 0;	// initialize time registers
	Timer0Reg1 = 0;	// initialize time registers
}

long timer0GetOverflowCount(void)
{
	// return the current timer overflow count
	// (this is since the last timer0ClearOverflowCount() command was called)
	return Timer0Reg0;
}

void timer2ClearOverflowCount(void)
{
	// clear the timer overflow counter registers
	Timer2Reg0 = 0;	// initialize time registers
	Timer2Reg1 = 0;	// initialize time registers
}

long timer2GetOverflowCount(void)
{
	// return the current timer overflow count
	// (this is since the last timer2ClearOverflowCount() command was called)
	return Timer2Reg0;
}


void timer1PWMInit(u08 bitRes)
{
	// configures timer1 for use with PWM output
	// on pins OC1A, OC1B, and OC1C

	// enable Timer1 as 8,9,10bit PWM
	if(bitRes == 9)
	{	// 9bit mode
		sbi(TCCR1A,WGMA1);
		cbi(TCCR1A,WGMA0);
	}
	else if( bitRes == 10 )
	{	// 10bit mode
		sbi(TCCR1A,WGMA1);
		sbi(TCCR1A,WGMA0);
	}
	else
	{	// default 8bit mode
		cbi(TCCR1A,WGMA1);
		sbi(TCCR1A,WGMA0);
	}

	// set clear-timer-on-compare-match
	//cbi(TCCR1B,CTC1);
	// clear output compare value A
	outb(OCR1AH, 0);
	outb(OCR1AL, 0);
	// clear output compare value B
	outb(OCR1BH, 0);
	outb(OCR1BL, 0);
	// clear output compare value C
	outb(OCR1CH, 0);
	outb(OCR1CL, 0);
}

void timer1PWMInitICR(u16 topcount)
{
	// set PWM mode with ICR top-count
	cbi(TCCR1A,WGM10);
	sbi(TCCR1A,WGM11);
	sbi(TCCR1B,WGM12);
	sbi(TCCR1B,WGM13);
	
	// set top count value
	ICR1H = (u08)(topcount>>8);
	ICR1L = (u08)topcount;
	
	// clear output compare value A
	outb(OCR1AH, 0);
	outb(OCR1AL, 0);
	// clear output compare value B
	outb(OCR1BH, 0);
	outb(OCR1BL, 0);
	// clear output compare value C
	outb(OCR1CH, 0);
	outb(OCR1CL, 0);
}

void timer1PWMOff(void)
{
	// turn off PWM on Timer1
	cbi(TCCR1A,WGMA1);
	cbi(TCCR1A,WGMA0);
	// clear (disable) clear-timer-on-compare-match
	//cbi(TCCR1B,CTC1);
	// set PWM1A/B/C (OutputCompare action) to none
	timer1PWMAOff();
	timer1PWMBOff();
	timer1PWMCOff();
}

void timer1PWMAOn(void)
{
	// turn on channel A (OC1A) PWM output
	// set OC1A as non-inverted PWM
	sbi(TCCR1A,COMA1);
	cbi(TCCR1A,COMA0);
}

void timer1PWMBOn(void)
{
	// turn on channel B (OC1B) PWM output
	// set OC1B as non-inverted PWM
	sbi(TCCR1A,COMB1);
	cbi(TCCR1A,COMB0);
}

void timer1PWMCOn(void)
{
	// turn on channel C (OC1C) PWM output
	// set OC1C as non-inverted PWM
	sbi(TCCR1A,COMC1);
	cbi(TCCR1A,COMC0);
}

void timer1PWMAOff(void)
{
	// turn off channel A (OC1A) PWM output
	// set OC1A (OutputCompare action) to none
	cbi(TCCR1A,COMA1);
	cbi(TCCR1A,COMA0);
}

void timer1PWMBOff(void)
{
	// turn off channel B (OC1B) PWM output
	// set OC1B (OutputCompare action) to none
	cbi(TCCR1A,COMB1);
	cbi(TCCR1A,COMB0);
}

void timer1PWMCOff(void)
{
	// turn off channel C (OC1C) PWM output
	// set OC1C (OutputCompare action) to none
	cbi(TCCR1A,COMC1);
	cbi(TCCR1A,COMC0);
}

void timer1PWMASet(u16 pwmDuty)
{
	// set PWM (output compare) duty for channel A
	// this PWM output is generated on OC1A pin
	// NOTE:	pwmDuty should be in the range 0-255 for 8bit PWM
	//			pwmDuty should be in the range 0-511 for 9bit PWM
	//			pwmDuty should be in the range 0-1023 for 10bit PWM
	outb(OCR1AH, (pwmDuty>>8));		// set the high 8bits of OCR1A
	outb(OCR1AL, (pwmDuty&0x00FF));	// set the low 8bits of OCR1A
}

void timer1PWMBSet(u16 pwmDuty)
{
	// set PWM (output compare) duty for channel B
	// this PWM output is generated on OC1B pin
	// NOTE:	pwmDuty should be in the range 0-255 for 8bit PWM
	//			pwmDuty should be in the range 0-511 for 9bit PWM
	//			pwmDuty should be in the range 0-1023 for 10bit PWM
	outb(OCR1BH, (pwmDuty>>8));		// set the high 8bits of OCR1B
	outb(OCR1BL, (pwmDuty&0x00FF));	// set the low 8bits of OCR1B
}

void timer1PWMCSet(u16 pwmDuty)
{
	// set PWM (output compare) duty for channel C
	// this PWM output is generated on OC1C pin
	// NOTE:	pwmDuty should be in the range 0-255 for 8bit PWM
	//			pwmDuty should be in the range 0-511 for 9bit PWM
	//			pwmDuty should be in the range 0-1023 for 10bit PWM
	outb(OCR1CH, (pwmDuty>>8));		// set the high 8bits of OCR1C
	outb(OCR1CL, (pwmDuty&0x00FF));	// set the low 8bits of OCR1C
}


void timer3PWMInit(u08 bitRes)
{
	// configures timer1 for use with PWM output
	// on pins OC3A, OC3B, and OC3C

	// enable Timer3 as 8,9,10bit PWM
	if(bitRes == 9)
	{	// 9bit mode
		sbi(TCCR3A,WGMA1);
		cbi(TCCR3A,WGMA0);
	}
	else if( bitRes == 10 )
	{	// 10bit mode
		sbi(TCCR3A,WGMA1);
		sbi(TCCR3A,WGMA0);
	}
	else
	{	// default 8bit mode
		cbi(TCCR3A,WGMA1);
		sbi(TCCR3A,WGMA0);
	}

	// set clear-timer-on-compare-match
	//cbi(TCCR3B,CTC1);
	// clear output compare value A
	outb(OCR3AH, 0);
	outb(OCR3AL, 0);
	// clear output compare value B
	outb(OCR3BH, 0);
	outb(OCR3BL, 0);
	// clear output compare value B
	outb(OCR3CH, 0);
	outb(OCR3CL, 0);
}

void timer3PWMInitICR(u16 topcount)
{
	// set PWM mode with ICR top-count
	cbi(TCCR3A,WGM30);
	sbi(TCCR3A,WGM31);
	sbi(TCCR3B,WGM32);
	sbi(TCCR3B,WGM33);
	
	// set top count value
	ICR3H = (u08)(topcount>>8);
	ICR3L = (u08)topcount;
	
	// clear output compare value A
	outb(OCR3AH, 0);
	outb(OCR3AL, 0);
	// clear output compare value B
	outb(OCR3BH, 0);
	outb(OCR3BL, 0);
	// clear output compare value C
	outb(OCR3CH, 0);
	outb(OCR3CL, 0);
}

void timer3PWMOff(void)
{
	// turn off PWM mode on Timer3
	cbi(TCCR3A,WGMA1);
	cbi(TCCR3A,WGMA0);
	// clear (disable) clear-timer-on-compare-match
	//cbi(TCCR3B,CTC1);
	// set OC3A/B/C (OutputCompare action) to none
	timer3PWMAOff();
	timer3PWMBOff();
	timer3PWMCOff();
}

void timer3PWMAOn(void)
{
	// turn on channel A (OC3A) PWM output
	// set OC3A as non-inverted PWM
	sbi(TCCR3A,COMA1);
	cbi(TCCR3A,COMA0);
}

void timer3PWMBOn(void)
{
	// turn on channel B (OC3B) PWM output
	// set OC3B as non-inverted PWM
	sbi(TCCR3A,COMB1);
	cbi(TCCR3A,COMB0);
}

void timer3PWMCOn(void)
{
	// turn on channel C (OC3C) PWM output
	// set OC3C as non-inverted PWM
	sbi(TCCR3A,COMC1);
	cbi(TCCR3A,COMC0);
}

void timer3PWMAOff(void)
{
	// turn off channel A (OC3A) PWM output
	// set OC3A (OutputCompare action) to none
	cbi(TCCR3A,COMA1);
	cbi(TCCR3A,COMA0);
}

void timer3PWMBOff(void)
{
	// turn off channel B (OC3B) PWM output
	// set OC3B (OutputCompare action) to none
	cbi(TCCR3A,COMB1);
	cbi(TCCR3A,COMB0);
}

void timer3PWMCOff(void)
{
	// turn off channel C (OC3C) PWM output
	// set OC3C (OutputCompare action) to none
	cbi(TCCR3A,COMC1);
	cbi(TCCR3A,COMC0);
}

void timer3PWMASet(u16 pwmDuty)
{
	// set PWM (output compare) duty for channel A
	// this PWM output is generated on OC3A pin
	// NOTE:	pwmDuty should be in the range 0-255 for 8bit PWM
	//			pwmDuty should be in the range 0-511 for 9bit PWM
	//			pwmDuty should be in the range 0-1023 for 10bit PWM
	outb(OCR3AH, (pwmDuty>>8), );		// set the high 8bits of OCR3A
	outb(OCR3AL, (pwmDuty&0x00FF), );	// set the low 8bits of OCR3A
}

void timer3PWMBSet(u16 pwmDuty)
{
	// set PWM (output compare) duty for channel B
	// this PWM output is generated on OC3B pin
	// NOTE:	pwmDuty should be in the range 0-255 for 8bit PWM
	//			pwmDuty should be in the range 0-511 for 9bit PWM
	//			pwmDuty should be in the range 0-1023 for 10bit PWM
	outb(OCR3BH, (pwmDuty>>8));		// set the high 8bits of OCR3B
	outb(OCR3BL, (pwmDuty&0x00FF));	// set the low 8bits of OCR3B
}

void timer3PWMCSet(u16 pwmDuty)
{
	// set PWM (output compare) duty for channel B
	// this PWM output is generated on OC3C pin
	// NOTE:	pwmDuty should be in the range 0-255 for 8bit PWM
	//			pwmDuty should be in the range 0-511 for 9bit PWM
	//			pwmDuty should be in the range 0-1023 for 10bit PWM
	outb(OCR3CH, (pwmDuty>>8));		// set the high 8bits of OCR3C
	outb(OCR3CL, (pwmDuty&0x00FF));	// set the low 8bits of OCR3C
}


//! Interrupt handler for tcnt0 overflow interrupt
TIMER_INTERRUPT_HANDLER(SIG_OVERFLOW0)
{
	Timer0Reg0++;		// increment low-order counter
	if(!Timer0Reg0)		// if low-order counter rollover
		Timer0Reg1++;	// increment high-order counter	

	// if a user function is defined, execute it too
	if(TimerIntFunc[TIMER0OVERFLOW_INT])
		TimerIntFunc[TIMER0OVERFLOW_INT]();
}

//! Interrupt handler for Timer1 overflow interrupt
TIMER_INTERRUPT_HANDLER(SIG_OVERFLOW1)
{
	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER1OVERFLOW_INT])
		TimerIntFunc[TIMER1OVERFLOW_INT]();
}

//! Interrupt handler for Timer2 overflow interrupt
TIMER_INTERRUPT_HANDLER(SIG_OVERFLOW2)
{
	Timer2Reg0++;		// increment low-order counter
	if(!Timer2Reg0)		// if low-order counter rollover
		Timer2Reg1++;	// increment high-order counter	

	// increment pause counter
	TimerPauseReg++;

	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER2OVERFLOW_INT])
		TimerIntFunc[TIMER2OVERFLOW_INT]();
}

//! Interrupt handler for Timer3 overflow interrupt
TIMER_INTERRUPT_HANDLER(SIG_OVERFLOW3)
{
	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER3OVERFLOW_INT])
		TimerIntFunc[TIMER3OVERFLOW_INT]();
}

//! Interrupt handler for OutputCompare0 match (OC0) interrupt
TIMER_INTERRUPT_HANDLER(SIG_OUTPUT_COMPARE0)
{
	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER0OUTCOMPARE_INT])
		TimerIntFunc[TIMER0OUTCOMPARE_INT]();
}

//! Interrupt handler for OutputCompare1A match (OC1A) interrupt
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

//! Interrupt handler for OutputCompare1C match (OC1C) interrupt
TIMER_INTERRUPT_HANDLER(SIG_OUTPUT_COMPARE1C)
{
	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER1OUTCOMPAREC_INT])
		TimerIntFunc[TIMER1OUTCOMPAREC_INT]();
}

//! Interrupt handler for InputCapture1(IC1) interrupt
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

//! Interrupt handler for OutputCompare3A match (OC3A) interrupt
TIMER_INTERRUPT_HANDLER(SIG_OUTPUT_COMPARE3A)
{
	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER3OUTCOMPAREA_INT])
		TimerIntFunc[TIMER3OUTCOMPAREA_INT]();
}

//! Interrupt handler for OutputCompare3B match (OC3B) interrupt
TIMER_INTERRUPT_HANDLER(SIG_OUTPUT_COMPARE3B)
{
	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER3OUTCOMPAREB_INT])
		TimerIntFunc[TIMER3OUTCOMPAREB_INT]();
}

//! Interrupt handler for OutputCompare3C match (OC3C) interrupt
TIMER_INTERRUPT_HANDLER(SIG_OUTPUT_COMPARE3C)
{
	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER3OUTCOMPAREC_INT])
		TimerIntFunc[TIMER3OUTCOMPAREC_INT]();
}

//! Interrupt handler for InputCapture3 (IC3) interrupt
TIMER_INTERRUPT_HANDLER(SIG_INPUT_CAPTURE3)
{
	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER3INPUTCAPTURE_INT])
		TimerIntFunc[TIMER3INPUTCAPTURE_INT]();
}
