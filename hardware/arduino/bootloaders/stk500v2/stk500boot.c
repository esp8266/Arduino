/*****************************************************************************
Title:     STK500v2 compatible bootloader
           Modified for Wiring board ATMega128-16MHz
Author:    Peter Fleury <pfleury@gmx.ch>   http://jump.to/fleury
File:      $Id: stk500boot.c,v 1.11 2006/06/25 12:39:17 peter Exp $
Compiler:  avr-gcc 3.4.5 or 4.1 / avr-libc 1.4.3
Hardware:  All AVRs with bootloader support, tested with ATmega8
License:   GNU General Public License

Modified:  Worapoht Kornkaewwattanakul <dev@avride.com>   http://www.avride.com
Date:      17 October 2007
Update:    1st, 29 Dec 2007 : Enable CMD_SPI_MULTI but ignore unused command by return 0x00 byte response..
Compiler:  WINAVR20060421
Description: add timeout feature like previous Wiring bootloader

DESCRIPTION:
    This program allows an AVR with bootloader capabilities to
    read/write its own Flash/EEprom. To enter Programming mode
    an input pin is checked. If this pin is pulled low, programming mode
    is entered. If not, normal execution is done from $0000
    "reset" vector in Application area.
    Size fits into a 1024 word bootloader section
	when compiled with avr-gcc 4.1
	(direct replace on Wiring Board without fuse setting changed)

USAGE:
    - Set AVR MCU type and clock-frequency (F_CPU) in the Makefile.
    - Set baud rate below (AVRISP only works with 115200 bps)
    - compile/link the bootloader with the supplied Makefile
    - program the "Boot Flash section size" (BOOTSZ fuses),
      for boot-size 1024 words:  program BOOTSZ01
    - enable the BOOT Reset Vector (program BOOTRST)
    - Upload the hex file to the AVR using any ISP programmer
    - Program Boot Lock Mode 3 (program BootLock 11 and BootLock 12 lock bits) // (leave them)
    - Reset your AVR while keeping PROG_PIN pulled low // (for enter bootloader by switch)
    - Start AVRISP Programmer (AVRStudio/Tools/Program AVR)
    - AVRISP will detect the bootloader
    - Program your application FLASH file and optional EEPROM file using AVRISP

Note:
    Erasing the device without flashing, through AVRISP GUI button "Erase Device"
    is not implemented, due to AVRStudio limitations.
    Flash is always erased before programming.

	AVRdude:
	Please uncomment #define REMOVE_CMD_SPI_MULTI when using AVRdude.
	Comment #define REMOVE_PROGRAM_LOCK_BIT_SUPPORT to reduce code size
	Read Fuse Bits and Read/Write Lock Bits is not supported

NOTES:
    Based on Atmel Application Note AVR109 - Self-programming
    Based on Atmel Application Note AVR068 - STK500v2 Protocol

LICENSE:
    Copyright (C) 2006 Peter Fleury

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

*****************************************************************************/

//************************************************************************
//*	Edit History
//************************************************************************
//*	Jul  7,	2010	<MLS> = Mark Sproul msproul@skycharoit.com
//*	Jul  7,	2010	<MLS> Working on mega2560. No Auto-restart
//*	Jul  7,	2010	<MLS> Switched to 8K bytes (4K words) so that we have room for the monitor
//*	Jul  8,	2010	<MLS> Found older version of source that had auto restart, put that code back in
//*	Jul  8,	2010	<MLS> Adding monitor code
//*	Jul 11,	2010	<MLS> Added blinking LED while waiting for download to start
//*	Jul 11,	2010	<MLS> Added EEPROM test
//*	Jul 29,	2010	<MLS> Added recchar_timeout for timing out on bootloading
//*	Aug 23,	2010	<MLS> Added support for atmega2561
//*	Aug 26,	2010	<MLS> Removed support for BOOT_BY_SWITCH
//************************************************************************



#include	<inttypes.h>
#include	<avr/io.h>
#include	<avr/interrupt.h>
#include	<avr/boot.h>
#include	<avr/pgmspace.h>
#include	<util/delay.h>
#include	<avr/eeprom.h>
#include	<avr/common.h>
#include	<stdlib.h>
#include	"command.h"


#if defined(_MEGA_BOARD_) || defined(_BOARD_AMBER128_) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2561__)
	#define		ENABLE_MONITOR
	static void	RunMonitor(void);
#endif

//#define	_DEBUG_SERIAL_
//#define	_DEBUG_WITH_LEDS_


/*
 * Uncomment the following lines to save code space
 */
//#define	REMOVE_PROGRAM_LOCK_BIT_SUPPORT		// disable program lock bits
//#define	REMOVE_BOOTLOADER_LED				// no LED to show active bootloader
//#define	REMOVE_CMD_SPI_MULTI				// disable processing of SPI_MULTI commands, Remark this line for AVRDUDE <Worapoht>
//



//************************************************************************
//*	LED on pin "PROGLED_PIN" on port "PROGLED_PORT"
//*	indicates that bootloader is active
//*	PG2 -> LED on Wiring board
//************************************************************************
#define		BLINK_LED_WHILE_WAITING

#ifdef _MEGA_BOARD_
	#define PROGLED_PORT	PORTB
	#define PROGLED_DDR		DDRB
	#define PROGLED_PIN		PINB7
#elif defined( _BOARD_AMBER128_ )
	//*	this is for the amber 128 http://www.soc-robotics.com/
	//*	onbarod led is PORTE4
	#define PROGLED_PORT	PORTD
	#define PROGLED_DDR		DDRD
	#define PROGLED_PIN		PINE7
#elif defined( _CEREBOTPLUS_BOARD_ )
	//*	this is for the Cerebot 2560 board
	//*	onbarod leds are on PORTE4-7
	#define PROGLED_PORT	PORTE
	#define PROGLED_DDR		DDRE
	#define PROGLED_PIN		PINE7
#elif defined( _PENGUINO_ )
	//*	this is for the Penguino
	//*	onbarod led is PORTE4
	#define PROGLED_PORT	PORTC
	#define PROGLED_DDR		DDRC
	#define PROGLED_PIN		PINC6
#elif defined( _ANDROID_2561_ ) || defined( __AVR_ATmega2561__ )
	//*	this is for the Boston Android 2561
	//*	onbarod led is PORTE4
	#define PROGLED_PORT	PORTA
	#define PROGLED_DDR		DDRA
	#define PROGLED_PIN		PINA3
#else
	#define PROGLED_PORT	PORTG
	#define PROGLED_DDR		DDRG
	#define PROGLED_PIN		PING2
#endif



/*
 * define CPU frequency in Mhz here if not defined in Makefile
 */
#ifndef F_CPU
	#define F_CPU 16000000UL
#endif

/*
 * UART Baudrate, AVRStudio AVRISP only accepts 115200 bps
 */

#ifndef BAUDRATE
	#define BAUDRATE 115200
#endif

/*
 *  Enable (1) or disable (0) USART double speed operation
 */
#ifndef UART_BAUDRATE_DOUBLE_SPEED
	#if defined (__AVR_ATmega32__)
		#define UART_BAUDRATE_DOUBLE_SPEED 0
	#else
		#define UART_BAUDRATE_DOUBLE_SPEED 1
	#endif
#endif

/*
 * HW and SW version, reported to AVRISP, must match version of AVRStudio
 */
#define CONFIG_PARAM_BUILD_NUMBER_LOW	0
#define CONFIG_PARAM_BUILD_NUMBER_HIGH	0
#define CONFIG_PARAM_HW_VER				0x0F
#define CONFIG_PARAM_SW_MAJOR			2
#define CONFIG_PARAM_SW_MINOR			0x0A

/*
 * Calculate the address where the bootloader starts from FLASHEND and BOOTSIZE
 * (adjust BOOTSIZE below and BOOTLOADER_ADDRESS in Makefile if you want to change the size of the bootloader)
 */
//#define BOOTSIZE 1024
#if FLASHEND > 0x0F000
	#define BOOTSIZE 8192
#else
	#define BOOTSIZE 2048
#endif

#define APP_END  (FLASHEND -(2*BOOTSIZE) + 1)

/*
 * Signature bytes are not available in avr-gcc io_xxx.h
 */
#if defined (__AVR_ATmega8__)
	#define SIGNATURE_BYTES 0x1E9307
#elif defined (__AVR_ATmega16__)
	#define SIGNATURE_BYTES 0x1E9403
#elif defined (__AVR_ATmega32__)
	#define SIGNATURE_BYTES 0x1E9502
#elif defined (__AVR_ATmega8515__)
	#define SIGNATURE_BYTES 0x1E9306
#elif defined (__AVR_ATmega8535__)
	#define SIGNATURE_BYTES 0x1E9308
#elif defined (__AVR_ATmega162__)
	#define SIGNATURE_BYTES 0x1E9404
#elif defined (__AVR_ATmega128__)
	#define SIGNATURE_BYTES 0x1E9702
#elif defined (__AVR_ATmega1280__)
	#define SIGNATURE_BYTES 0x1E9703
#elif defined (__AVR_ATmega2560__)
	#define SIGNATURE_BYTES 0x1E9801
#elif defined (__AVR_ATmega2561__)
	#define SIGNATURE_BYTES 0x1e9802
#else
	#error "no signature definition for MCU available"
#endif


#if defined(__AVR_ATmega8__) || defined(__AVR_ATmega16__) || defined(__AVR_ATmega32__) \
	|| defined(__AVR_ATmega8515__) || defined(__AVR_ATmega8535__)
	/* ATMega8 with one USART */
	#define	UART_BAUD_RATE_LOW			UBRRL
	#define	UART_STATUS_REG				UCSRA
	#define	UART_CONTROL_REG			UCSRB
	#define	UART_ENABLE_TRANSMITTER		TXEN
	#define	UART_ENABLE_RECEIVER		RXEN
	#define	UART_TRANSMIT_COMPLETE		TXC
	#define	UART_RECEIVE_COMPLETE		RXC
	#define	UART_DATA_REG				UDR
	#define	UART_DOUBLE_SPEED			U2X

#elif defined(__AVR_ATmega64__) || defined(__AVR_ATmega128__) || defined(__AVR_ATmega162__) \
	 || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2561__)
	/* ATMega with two USART, use UART0 */
	#define	UART_BAUD_RATE_LOW			UBRR0L
	#define	UART_STATUS_REG				UCSR0A
	#define	UART_CONTROL_REG			UCSR0B
	#define	UART_ENABLE_TRANSMITTER		TXEN0
	#define	UART_ENABLE_RECEIVER		RXEN0
	#define	UART_TRANSMIT_COMPLETE		TXC0
	#define	UART_RECEIVE_COMPLETE		RXC0
	#define	UART_DATA_REG				UDR0
	#define	UART_DOUBLE_SPEED			U2X0
#else
	#error "no UART definition for MCU available"
#endif



/*
 * Macro to calculate UBBR from XTAL and baudrate
 */
#if defined(__AVR_ATmega32__) && UART_BAUDRATE_DOUBLE_SPEED
	#define UART_BAUD_SELECT(baudRate,xtalCpu) ((xtalCpu / 4 / baudRate - 1) / 2)
#elif defined(__AVR_ATmega32__)
	#define UART_BAUD_SELECT(baudRate,xtalCpu) ((xtalCpu / 8 / baudRate - 1) / 2)
#elif UART_BAUDRATE_DOUBLE_SPEED
	#define UART_BAUD_SELECT(baudRate,xtalCpu) (((float)(xtalCpu))/(((float)(baudRate))*8.0)-1.0+0.5)
#else
	#define UART_BAUD_SELECT(baudRate,xtalCpu) (((float)(xtalCpu))/(((float)(baudRate))*16.0)-1.0+0.5)
#endif


/*
 * States used in the receive state machine
 */
#define	ST_START		0
#define	ST_GET_SEQ_NUM	1
#define ST_MSG_SIZE_1	2
#define ST_MSG_SIZE_2	3
#define ST_GET_TOKEN	4
#define ST_GET_DATA		5
#define	ST_GET_CHECK	6
#define	ST_PROCESS		7

/*
 * use 16bit address variable for ATmegas with <= 64K flash
 */
#if defined(RAMPZ)
	typedef uint32_t address_t;
#else
	typedef uint16_t address_t;
#endif

/*
 * function prototypes
 */
static void sendchar(char c);
static unsigned char recchar(void);

/*
 * since this bootloader is not linked against the avr-gcc crt1 functions,
 * to reduce the code size, we need to provide our own initialization
 */
void __jumpMain	(void) __attribute__ ((naked)) __attribute__ ((section (".init9")));
#include <avr/sfr_defs.h>

//#define	SPH_REG	0x3E
//#define	SPL_REG	0x3D

//*****************************************************************************
void __jumpMain(void)
{
//*	July 17, 2010	<MLS> Added stack pointer initialzation
//*	the first line did not do the job on the ATmega128

	asm volatile ( ".set __stack, %0" :: "i" (RAMEND) );

//	ldi r16,high(RAMEND)
//	out SPH,r16 ; Set stack pointer to top of RAM

//	asm volatile ( "ldi	16, 0x10");
	asm volatile ( "ldi	16, %0" :: "i" (RAMEND >> 8) );
//	asm volatile ( "out 0x3E,16");
//	asm volatile ( "out %0,16" :: "i" (SPH_REG) );
	asm volatile ( "out %0,16" :: "i" (AVR_STACK_POINTER_HI_ADDR) );

//	asm volatile ( "ldi	16, 0x00");
	asm volatile ( "ldi	16, %0" :: "i" (RAMEND & 0x0ff) );
//	asm volatile ( "out 0x3d,16");
//	asm volatile ( "out %0,16" :: "i" (SPL_REG) );
	asm volatile ( "out %0,16" :: "i" (AVR_STACK_POINTER_LO_ADDR) );



	asm volatile ( "clr __zero_reg__" );									// GCC depends on register r1 set to 0
	asm volatile ( "out %0, __zero_reg__" :: "I" (_SFR_IO_ADDR(SREG)) );	// set SREG to 0
//	asm volatile ( "rjmp main");											// jump to main()
	asm volatile ( "jmp main");												// jump to main()
}


//*****************************************************************************
void delay_ms(unsigned int timedelay)
{
	unsigned int i;
	for (i=0;i<timedelay;i++)
	{
		_delay_ms(0.5);
	}
}


//*****************************************************************************
/*
 * send single byte to USART, wait until transmission is completed
 */
static void sendchar(char c)
{
	UART_DATA_REG	=	c;										// prepare transmission
	while (!(UART_STATUS_REG & (1 << UART_TRANSMIT_COMPLETE)));	// wait until byte sent
	UART_STATUS_REG |= (1 << UART_TRANSMIT_COMPLETE);			// delete TXCflag
}


//************************************************************************
static int	Serial_Available(void)
{
	return(UART_STATUS_REG & (1 << UART_RECEIVE_COMPLETE));	// wait for data
}


//*****************************************************************************
/*
 * Read single byte from USART, block if no data available
 */
static unsigned char recchar(void)
{
	while (!(UART_STATUS_REG & (1 << UART_RECEIVE_COMPLETE)))
	{
		// wait for data
	}
	return UART_DATA_REG;
}

#define	MAX_TIME_COUNT	(F_CPU >> 1)
//*****************************************************************************
static unsigned char recchar_timeout(void)
{
uint32_t count = 0;

	while (!(UART_STATUS_REG & (1 << UART_RECEIVE_COMPLETE)))
	{
		// wait for data
		count++;
		if (count > MAX_TIME_COUNT)
		{
		unsigned int	data;
		#if (FLASHEND > 0x0FFFF)
			data	=	pgm_read_word_far(0);	//*	get the first word of the user program
		#else
			data	=	pgm_read_word_near(0);	//*	get the first word of the user program
		#endif
			if (data != 0xffff)					//*	make sure its valid before jumping to it.
			{
				asm volatile(
						"clr	r30		\n\t"
						"clr	r31		\n\t"
						"ijmp	\n\t"
						);
			}
			count	=	0;
		}
	}
	return UART_DATA_REG;
}



//*****************************************************************************
int main(void)
{
	address_t		address			=	0;
	address_t		eraseAddress	=	0;
	unsigned char	msgParseState;
	unsigned int	ii				=	0;
	unsigned char	checksum		=	0;
	unsigned char	seqNum			=	0;
	unsigned int	msgLength		=	0;
	unsigned char	msgBuffer[285];
	unsigned char	c, *p;
	unsigned char   isLeave = 0;

	unsigned long	boot_timeout;
	unsigned long	boot_timer;
	unsigned int	boot_state;
#ifdef ENABLE_MONITOR
	unsigned int	exPointCntr	=	0;
#endif


	boot_timer	=	0;
	boot_state	=	0;

#ifdef BLINK_LED_WHILE_WAITING
	boot_timeout	=	 20000;		//*	should be about 1 second
//	boot_timeout	=	170000;
#else
	boot_timeout	=	3500000; // 7 seconds , approx 2us per step when optimize "s"
#endif
	/*
	 * Branch to bootloader or application code ?
	 */

#ifndef REMOVE_BOOTLOADER_LED
	/* PROG_PIN pulled low, indicate with LED that bootloader is active */
	PROGLED_DDR		|=	(1<<PROGLED_PIN);
//	PROGLED_PORT	&=	~(1<<PROGLED_PIN);	// active low LED ON
	PROGLED_PORT	|=	(1<<PROGLED_PIN);	// active high LED ON

#ifdef _DEBUG_WITH_LEDS_
	for (ii=0; ii<3; ii++)
	{
		PROGLED_PORT	&=	~(1<<PROGLED_PIN);	// turn LED off
		delay_ms(100);
		PROGLED_PORT	|=	(1<<PROGLED_PIN);	// turn LED on
		delay_ms(100);
	}
#endif

#endif
	/*
	 * Init UART
	 * set baudrate and enable USART receiver and transmiter without interrupts
	 */
#if UART_BAUDRATE_DOUBLE_SPEED
	UART_STATUS_REG		|=	(1 <<UART_DOUBLE_SPEED);
#endif
	UART_BAUD_RATE_LOW	=	UART_BAUD_SELECT(BAUDRATE,F_CPU);
	UART_CONTROL_REG	=	(1 << UART_ENABLE_RECEIVER) | (1 << UART_ENABLE_TRANSMITTER);

	asm volatile ("nop");			// wait until port has changed

#ifdef _DEBUG_SERIAL_
//	delay_ms(500);

	sendchar('s');
	sendchar('t');
	sendchar('k');
//	sendchar('5');
//	sendchar('0');
//	sendchar('0');
	sendchar('v');
	sendchar('2');
	sendchar(0x0d);
	sendchar(0x0a);

	delay_ms(100);
#endif

	while (boot_state==0)
	{
		while ((!(Serial_Available())) && (boot_state == 0))		// wait for data
		{
			_delay_ms(0.001);
			boot_timer++;
			if (boot_timer > boot_timeout)
			{
				boot_state	=	1; // (after ++ -> boot_state=2 bootloader timeout, jump to main 0x00000 )
			}
		#ifdef BLINK_LED_WHILE_WAITING
			if ((boot_timer % 7000) == 0)
			{
				//*	toggle the LED
				PROGLED_PORT	^=	(1<<PROGLED_PIN);	// turn LED ON
			}
		#endif
		}
		boot_state++; // ( if boot_state=1 bootloader received byte from UART, enter bootloader mode)
	}


	if (boot_state==1)
	{
		//*	main loop
		while (!isLeave)
		{
			/*
			 * Collect received bytes to a complete message
			 */
			msgParseState	=	ST_START;
			while ( msgParseState != ST_PROCESS )
			{
				if (boot_state==1)
				{
					boot_state	=	0;
					c			=	UART_DATA_REG;
				}
				else
				{
				//	c	=	recchar();
					c	=	recchar_timeout();
				}

			#ifdef ENABLE_MONITOR
				if (c == '!')
				{
					exPointCntr++;
					if (exPointCntr == 3)
					{
						RunMonitor();
						exPointCntr		=	0;	//	reset back to zero so we dont get in an endless loop
						isLeave			=	1;
						msgParseState	=	99;	//*	we dont want it do anything
						break;
					}
				}
				else
				{
					exPointCntr	=	0;
				}
			#endif

				switch (msgParseState)
				{
					case ST_START:
						if ( c == MESSAGE_START )
						{
							msgParseState	=	ST_GET_SEQ_NUM;
							checksum		=	MESSAGE_START^0;
						}
						break;

					case ST_GET_SEQ_NUM:
						if ( (c == 1) || (c == seqNum) )
						{
							seqNum			=	c;
							msgParseState	=	ST_MSG_SIZE_1;
							checksum		^=	c;
						}
						else
						{
							msgParseState	=	ST_START;
						}
						break;

					case ST_MSG_SIZE_1:
						msgLength		=	c<<8;
						msgParseState	=	ST_MSG_SIZE_2;
						checksum		^=	c;
						break;

					case ST_MSG_SIZE_2:
						msgLength		|=	c;
						msgParseState	=	ST_GET_TOKEN;
						checksum		^=	c;
						break;

					case ST_GET_TOKEN:
						if ( c == TOKEN )
						{
							msgParseState	=	ST_GET_DATA;
							checksum		^=	c;
							ii				=	0;
						}
						else
						{
							msgParseState	=	ST_START;
						}
						break;

					case ST_GET_DATA:
						msgBuffer[ii++]	=	c;
						checksum		^=	c;
						if (ii == msgLength )
						{
							msgParseState	=	ST_GET_CHECK;
						}
						break;

					case ST_GET_CHECK:
						if ( c == checksum )
						{
							msgParseState	=	ST_PROCESS;
						}
						else
						{
							msgParseState	=	ST_START;
						}
						break;
				}	//	switch
			}	//	while(msgParseState)

			/*
			 * Now process the STK500 commands, see Atmel Appnote AVR068
			 */

			switch (msgBuffer[0])
			{
	#ifndef REMOVE_CMD_SPI_MULTI
				case CMD_SPI_MULTI:
					{
						unsigned char answerByte;
						unsigned char flag=0;

						if ( msgBuffer[4]== 0x30 )
						{
							unsigned char signatureIndex	=	msgBuffer[6];

							if ( signatureIndex == 0 )
								answerByte	=	(SIGNATURE_BYTES >>16) & 0x000000FF;
							else if ( signatureIndex == 1 )
								answerByte	=	(SIGNATURE_BYTES >> 8) & 0x000000FF;
							else
								answerByte	=	SIGNATURE_BYTES & 0x000000FF;
						}
						else if ( msgBuffer[4] & 0x50 )
						{
							answerByte	=	0; //read fuse/lock bits not implemented, return dummy value
						}
						else
						{
							answerByte	=	0; // for all others command are not implemented, return dummy value for AVRDUDE happy <Worapoht>
	//						flag	=	1; // Remark this line for AVRDUDE <Worapoht>
						}
						if ( !flag )
						{
							msgLength		=	7;
							msgBuffer[1]	=	STATUS_CMD_OK;
							msgBuffer[2]	=	0;
							msgBuffer[3]	=	msgBuffer[4];
							msgBuffer[4]	=	0;
							msgBuffer[5]	=	answerByte;
							msgBuffer[6]	=	STATUS_CMD_OK;
						}
					}
					break;
	#endif
				case CMD_SIGN_ON:
					msgLength		=	11;
					msgBuffer[1] 	=	STATUS_CMD_OK;
					msgBuffer[2] 	=	8;
					msgBuffer[3] 	=	'A';
					msgBuffer[4] 	=	'V';
					msgBuffer[5] 	=	'R';
					msgBuffer[6] 	=	'I';
					msgBuffer[7] 	=	'S';
					msgBuffer[8] 	=	'P';
					msgBuffer[9] 	=	'_';
					msgBuffer[10]	=	'2';
					break;

				case CMD_GET_PARAMETER:
					{
						unsigned char value;

						switch(msgBuffer[1])
						{
						case PARAM_BUILD_NUMBER_LOW:
							value	=	CONFIG_PARAM_BUILD_NUMBER_LOW;
							break;
						case PARAM_BUILD_NUMBER_HIGH:
							value	=	CONFIG_PARAM_BUILD_NUMBER_HIGH;
							break;
						case PARAM_HW_VER:
							value	=	CONFIG_PARAM_HW_VER;
							break;
						case PARAM_SW_MAJOR:
							value	=	CONFIG_PARAM_SW_MAJOR;
							break;
						case PARAM_SW_MINOR:
							value	=	CONFIG_PARAM_SW_MINOR;
							break;
						default:
							value	=	0;
							break;
						}
						msgLength		=	3;
						msgBuffer[1]	=	STATUS_CMD_OK;
						msgBuffer[2]	=	value;
					}
					break;

				case CMD_LEAVE_PROGMODE_ISP:
					isLeave	=	1;
					//*	fall thru

				case CMD_SET_PARAMETER:
				case CMD_ENTER_PROGMODE_ISP:
					msgLength		=	2;
					msgBuffer[1]	=	STATUS_CMD_OK;
					break;

				case CMD_READ_SIGNATURE_ISP:
					{
						unsigned char signatureIndex	=	msgBuffer[4];
						unsigned char signature;

						if ( signatureIndex == 0 )
							signature	=	(SIGNATURE_BYTES >>16) & 0x000000FF;
						else if ( signatureIndex == 1 )
							signature	=	(SIGNATURE_BYTES >> 8) & 0x000000FF;
						else
							signature	=	SIGNATURE_BYTES & 0x000000FF;

						msgLength		=	4;
						msgBuffer[1]	=	STATUS_CMD_OK;
						msgBuffer[2]	=	signature;
						msgBuffer[3]	=	STATUS_CMD_OK;
					}
					break;

				case CMD_READ_LOCK_ISP:
					msgLength		=	4;
					msgBuffer[1]	=	STATUS_CMD_OK;
					msgBuffer[2]	=	boot_lock_fuse_bits_get( GET_LOCK_BITS );
					msgBuffer[3]	=	STATUS_CMD_OK;
					break;

				case CMD_READ_FUSE_ISP:
					{
						unsigned char fuseBits;

						if ( msgBuffer[2] == 0x50 )
						{
							if ( msgBuffer[3] == 0x08 )
								fuseBits	=	boot_lock_fuse_bits_get( GET_EXTENDED_FUSE_BITS );
							else
								fuseBits	=	boot_lock_fuse_bits_get( GET_LOW_FUSE_BITS );
						}
						else
						{
							fuseBits	=	boot_lock_fuse_bits_get( GET_HIGH_FUSE_BITS );
						}
						msgLength		=	4;
						msgBuffer[1]	=	STATUS_CMD_OK;
						msgBuffer[2]	=	fuseBits;
						msgBuffer[3]	=	STATUS_CMD_OK;
					}
					break;

	#ifndef REMOVE_PROGRAM_LOCK_BIT_SUPPORT
				case CMD_PROGRAM_LOCK_ISP:
					{
						unsigned char lockBits	=	msgBuffer[4];

						lockBits	=	(~lockBits) & 0x3C;	// mask BLBxx bits
						boot_lock_bits_set(lockBits);		// and program it
						boot_spm_busy_wait();

						msgLength		=	3;
						msgBuffer[1]	=	STATUS_CMD_OK;
						msgBuffer[2]	=	STATUS_CMD_OK;
					}
					break;
	#endif
				case CMD_CHIP_ERASE_ISP:
					eraseAddress	=	0;
					msgLength		=	2;
					msgBuffer[1]	=	STATUS_CMD_OK;
					break;

				case CMD_LOAD_ADDRESS:
	#if defined(RAMPZ)
					address	=	( ((address_t)(msgBuffer[1])<<24)|((address_t)(msgBuffer[2])<<16)|((address_t)(msgBuffer[3])<<8)|(msgBuffer[4]) )<<1;
	#else
					address	=	( ((msgBuffer[3])<<8)|(msgBuffer[4]) )<<1;		//convert word to byte address
	#endif
					msgLength		=	2;
					msgBuffer[1]	=	STATUS_CMD_OK;
					break;

				case CMD_PROGRAM_FLASH_ISP:
				case CMD_PROGRAM_EEPROM_ISP:
					{
						unsigned int	size	=	((msgBuffer[1])<<8) | msgBuffer[2];
						unsigned char	*p	=	msgBuffer+10;
						unsigned int	data;
						unsigned char	highByte, lowByte;
						address_t		tempaddress	=	address;


						if ( msgBuffer[0] == CMD_PROGRAM_FLASH_ISP )
						{
							// erase only main section (bootloader protection)
							if (eraseAddress < APP_END )
							{
								boot_page_erase(eraseAddress);	// Perform page erase
								boot_spm_busy_wait();		// Wait until the memory is erased.
								eraseAddress += SPM_PAGESIZE;	// point to next page to be erase
							}

							/* Write FLASH */
							do {
								lowByte		=	*p++;
								highByte 	=	*p++;

								data		=	(highByte << 8) | lowByte;
								boot_page_fill(address,data);

								address	=	address + 2;	// Select next word in memory
								size	-=	2;				// Reduce number of bytes to write by two
							} while (size);					// Loop until all bytes written

							boot_page_write(tempaddress);
							boot_spm_busy_wait();
							boot_rww_enable();				// Re-enable the RWW section
						}
						else
						{
						#if (!defined(__AVR_ATmega1280__) && !defined(__AVR_ATmega2560__)  && !defined(__AVR_ATmega2561__))
							/* write EEPROM */
							do {
								EEARL	=	address;			// Setup EEPROM address
								EEARH	=	(address >> 8);
								address++;						// Select next EEPROM byte

								EEDR	=	*p++;				// get byte from buffer
								EECR	|=	(1<<EEMWE);			// Write data into EEPROM
								EECR	|=	(1<<EEWE);

								while (EECR & (1<<EEWE));	// Wait for write operation to finish
								size--;						// Decrease number of bytes to write
							} while (size);					// Loop until all bytes written
						#endif
						}
							msgLength	=	2;
						msgBuffer[1]	=	STATUS_CMD_OK;
					}
					break;

				case CMD_READ_FLASH_ISP:
				case CMD_READ_EEPROM_ISP:
					{
						unsigned int	size	=	((msgBuffer[1])<<8) | msgBuffer[2];
						unsigned char	*p		=	msgBuffer+1;
						msgLength				=	size+3;

						*p++	=	STATUS_CMD_OK;
						if (msgBuffer[0] == CMD_READ_FLASH_ISP )
						{
							unsigned int data;

							// Read FLASH
							do {
	#if defined(RAMPZ)
								data	=	pgm_read_word_far(address);
	#else
								data	=	pgm_read_word_near(address);
	#endif
								*p++	=	(unsigned char)data;		//LSB
								*p++	=	(unsigned char)(data >> 8);	//MSB
								address	+=	2;							// Select next word in memory
								size	-=	2;
							}while (size);
						}
						else
						{
							/* Read EEPROM */
							do {
								EEARL	=	address;			// Setup EEPROM address
								EEARH	=	((address >> 8));
								address++;					// Select next EEPROM byte
								EECR	|=	(1<<EERE);			// Read EEPROM
								*p++	=	EEDR;				// Send EEPROM data
								size--;
							} while (size);
						}
						*p++	=	STATUS_CMD_OK;
					}
					break;

				default:
					msgLength		=	2;
					msgBuffer[1]	=	STATUS_CMD_FAILED;
					break;
			}

			/*
			 * Now send answer message back
			 */
			sendchar(MESSAGE_START);
			checksum	=	MESSAGE_START^0;

			sendchar(seqNum);
			checksum	^=	seqNum;

			c			=	((msgLength>>8)&0xFF);
			sendchar(c);
			checksum	^=	c;

			c			=	msgLength&0x00FF;
			sendchar(c);
			checksum ^= c;

			sendchar(TOKEN);
			checksum ^= TOKEN;

			p	=	msgBuffer;
			while ( msgLength )
			{
				c	=	*p++;
				sendchar(c);
				checksum ^=c;
				msgLength--;
			}
			sendchar(checksum);
			seqNum++;
	
		#ifndef REMOVE_BOOTLOADER_LED
			//*	<MLS>	toggle the LED
			PROGLED_PORT	^=	(1<<PROGLED_PIN);	// active high LED ON
		#endif

		}
	}

#ifdef _DEBUG_WITH_LEDS_
	//*	this is for debugging it can be removed
	for (ii=0; ii<10; ii++)
	{
		PROGLED_PORT	&=	~(1<<PROGLED_PIN);	// turn LED off
		delay_ms(200);
		PROGLED_PORT	|=	(1<<PROGLED_PIN);	// turn LED on
		delay_ms(200);
	}
	PROGLED_PORT	&=	~(1<<PROGLED_PIN);	// turn LED off
#endif

#ifdef _DEBUG_SERIAL_
	sendchar('j');
//	sendchar('u');
//	sendchar('m');
//	sendchar('p');
//	sendchar(' ');
//	sendchar('u');
//	sendchar('s');
//	sendchar('r');
	sendchar(0x0d);
	sendchar(0x0a);

	delay_ms(100);
#endif


#ifndef REMOVE_BOOTLOADER_LED
	PROGLED_DDR		&=	~(1<<PROGLED_PIN);	// set to default
	PROGLED_PORT	&=	~(1<<PROGLED_PIN);	// active low LED OFF
//	PROGLED_PORT	|=	(1<<PROGLED_PIN);	// active high LED OFf
	delay_ms(100);							// delay after exit
#endif


	asm volatile ("nop");			// wait until port has changed

	/*
	 * Now leave bootloader
	 */

	UART_STATUS_REG	&=	0xfd;
	boot_rww_enable();				// enable application section


	asm volatile(
			"clr	r30		\n\t"
			"clr	r31		\n\t"
			"ijmp	\n\t"
			);
//	asm volatile ( "push r1" "\n\t"		// Jump to Reset vector in Application Section
//					"push r1" "\n\t"
//					"ret"	 "\n\t"
//					::);

	 /*
	 * Never return to stop GCC to generate exit return code
	 * Actually we will never reach this point, but the compiler doesn't
	 * understand this
	 */
	for(;;);
}

/*
base address = f800

avrdude: Device signature = 0x1e9703
avrdude: safemode: lfuse reads as FF
avrdude: safemode: hfuse reads as DA
avrdude: safemode: efuse reads as F5
avrdude>


base address = f000
avrdude: Device signature = 0x1e9703
avrdude: safemode: lfuse reads as FF
avrdude: safemode: hfuse reads as D8
avrdude: safemode: efuse reads as F5
avrdude>
*/

//************************************************************************
#ifdef ENABLE_MONITOR
#include	<math.h>

unsigned long	gRamIndex;
unsigned long	gFlashIndex;
unsigned long	gEepromIndex;


#define	true	1
#define	false	0

#if defined(__AVR_ATmega128__)
	#define	kCPU_NAME	"ATmega128"
#elif defined(__AVR_ATmega1280__)
	#define	kCPU_NAME	"ATmega1280"
#elif defined(__AVR_ATmega1281__)
	#define	kCPU_NAME	"ATmega1281"
#elif defined(__AVR_ATmega2560__)
	#define	kCPU_NAME	"ATmega2560"
#elif defined(__AVR_ATmega2561__)
	#define	kCPU_NAME	"ATmega2561"
#endif

#ifdef _VECTORS_SIZE
	#define	kInterruptVectorCount (_VECTORS_SIZE / 4)
#else
	#define	kInterruptVectorCount 23
#endif


void	PrintDecInt(int theNumber, int digitCnt);

#ifdef kCPU_NAME
	prog_char	gTextMsg_CPU_Name[]			PROGMEM	=	kCPU_NAME;
#else
	prog_char	gTextMsg_CPU_Name[]			PROGMEM	=	"UNKNOWN";
#endif

	prog_char	gTextMsg_Explorer[]			PROGMEM	=	"Arduino explorer stk500V2 by MLS";
	prog_char	gTextMsg_Prompt[]			PROGMEM	=	"Bootloader>";
	prog_char	gTextMsg_HUH[]				PROGMEM	=	"Huh?";
	prog_char	gTextMsg_COMPILED_ON[]		PROGMEM	=	"Compiled on  = ";
	prog_char	gTextMsg_CPU_Type[]			PROGMEM	=	"CPU Type     = ";
	prog_char	gTextMsg_AVR_ARCH[]			PROGMEM	=	"__AVR_ARCH__ = ";
	prog_char	gTextMsg_AVR_LIBC[]			PROGMEM	=	"AVR LibC Ver = ";
	prog_char	gTextMsg_GCC_VERSION[]		PROGMEM	=	"GCC Version  = ";
	prog_char	gTextMsg_CPU_SIGNATURE[]	PROGMEM	=	"CPU signature= ";
	prog_char	gTextMsg_FUSE_BYTE_LOW[]	PROGMEM	=	"Low fuse     = ";
	prog_char	gTextMsg_FUSE_BYTE_HIGH[]	PROGMEM	=	"High fuse    = ";
	prog_char	gTextMsg_FUSE_BYTE_EXT[]	PROGMEM	=	"Ext fuse     = ";
	prog_char	gTextMsg_FUSE_BYTE_LOCK[]	PROGMEM	=	"Lock fuse    = ";
	prog_char	gTextMsg_GCC_DATE_STR[]		PROGMEM	=	__DATE__;
	prog_char	gTextMsg_AVR_LIBC_VER_STR[]	PROGMEM	=	__AVR_LIBC_VERSION_STRING__;
	prog_char	gTextMsg_GCC_VERSION_STR[]	PROGMEM	=	__VERSION__;
	prog_char	gTextMsg_VECTOR_HEADER[]	PROGMEM	=	"V#   ADDR   op code     instruction addr   Interrupt";
	prog_char	gTextMsg_noVector[]			PROGMEM	=	"no vector";
	prog_char	gTextMsg_rjmp[]				PROGMEM	=	"rjmp  ";
	prog_char	gTextMsg_jmp[]				PROGMEM	=	"jmp ";
	prog_char	gTextMsg_WHAT_PORT[]		PROGMEM	=	"What port:";
	prog_char	gTextMsg_PortNotSupported[]	PROGMEM	=	"Port not supported";
	prog_char	gTextMsg_MustBeLetter[]		PROGMEM	=	"Must be a letter";
	prog_char	gTextMsg_SPACE[]			PROGMEM	=	" ";
	prog_char	gTextMsg_WriteToEEprom[]	PROGMEM	=	"Writting EE";
	prog_char	gTextMsg_ReadingEEprom[]	PROGMEM	=	"Reading EE";
	prog_char	gTextMsg_EEPROMerrorCnt[]	PROGMEM	=	"eeprom error count=";
	prog_char	gTextMsg_PORT[]				PROGMEM	=	"PORT";


//************************************************************************
//*	Help messages
	prog_char	gTextMsg_HELP_MSG_0[]		PROGMEM	=	"0=Zero address ctrs";
	prog_char	gTextMsg_HELP_MSG_QM[]		PROGMEM	=	"?=CPU stats";
	prog_char	gTextMsg_HELP_MSG_AT[]		PROGMEM	=	"@=EEPROM test";
	prog_char	gTextMsg_HELP_MSG_B[]		PROGMEM	=	"B=Blink LED";
	prog_char	gTextMsg_HELP_MSG_E[]		PROGMEM	=	"E=Dump EEPROM";
	prog_char	gTextMsg_HELP_MSG_F[]		PROGMEM	=	"F=Dump FLASH";
	prog_char	gTextMsg_HELP_MSG_H[]		PROGMEM	=	"H=Help";
	prog_char	gTextMsg_HELP_MSG_L[]		PROGMEM	=	"L=List I/O Ports";
	prog_char	gTextMsg_HELP_MSG_Q[]		PROGMEM	=	"Q=Quit & jump to user pgm";
	prog_char	gTextMsg_HELP_MSG_R[]		PROGMEM	=	"R=Dump RAM";
	prog_char	gTextMsg_HELP_MSG_V[]		PROGMEM	=	"V=show interrupt Vectors";
	prog_char	gTextMsg_HELP_MSG_Y[]		PROGMEM	=	"Y=Port blink";

	prog_char	gTextMsg_END[]				PROGMEM	=	"*";


//************************************************************************
void	PrintFromPROGMEM(void *dataPtr, unsigned char offset)
{
uint8_t	ii;
char	theChar;

	ii			=	offset;
	theChar		=	1;

	while (theChar != 0)
	{
		theChar	=	pgm_read_byte_far((uint32_t)dataPtr + ii);
		if (theChar != 0)
		{
			sendchar(theChar);
		}
		ii++;
	}
}

//************************************************************************
void	PrintNewLine(void)
{
	sendchar(0x0d);
	sendchar(0x0a);
}


//************************************************************************
void	PrintFromPROGMEMln(void *dataPtr, unsigned char offset)
{
	PrintFromPROGMEM(dataPtr, offset);

	PrintNewLine();
}


//************************************************************************
void	PrintString(char *textString)
{
char	theChar;
int		ii;

	theChar		=	1;
	ii			=	0;
	while (theChar != 0)
	{
		theChar	=	textString[ii];
		if (theChar != 0)
		{
			sendchar(theChar);
		}
		ii++;
	}
}

//************************************************************************
void	PrintHexByte(unsigned char theByte)
{
char	theChar;

	theChar	=	0x30 + ((theByte >> 4) & 0x0f);
	if (theChar > 0x39)
	{
		theChar	+=	7;
	}
	sendchar(theChar );

	theChar	=	0x30 + (theByte & 0x0f);
	if (theChar > 0x39)
	{
		theChar	+=	7;
	}
	sendchar(theChar );
}

//************************************************************************
void	PrintDecInt(int theNumber, int digitCnt)
{
int	theChar;
int	myNumber;

	myNumber	=	theNumber;

	if ((myNumber > 100) || (digitCnt >= 3))
	{
		theChar		=	0x30 + myNumber / 100;
		sendchar(theChar );
	}

	if ((myNumber > 10) || (digitCnt >= 2))
	{
		theChar	=	0x30  + ((myNumber % 100) / 10 );
		sendchar(theChar );
	}
	theChar	=	0x30 + (myNumber % 10);
	sendchar(theChar );
}




//************************************************************************
static void	PrintCPUstats(void)
{
unsigned char fuseByte;

	PrintFromPROGMEMln(gTextMsg_Explorer, 0);

	PrintFromPROGMEM(gTextMsg_COMPILED_ON, 0);
	PrintFromPROGMEMln(gTextMsg_GCC_DATE_STR, 0);

	PrintFromPROGMEM(gTextMsg_CPU_Type, 0);
	PrintFromPROGMEMln(gTextMsg_CPU_Name, 0);

	PrintFromPROGMEM(gTextMsg_AVR_ARCH, 0);
	PrintDecInt(__AVR_ARCH__, 1);
	PrintNewLine();

	PrintFromPROGMEM(gTextMsg_GCC_VERSION, 0);
	PrintFromPROGMEMln(gTextMsg_GCC_VERSION_STR, 0);

	//*	these can be found in avr/version.h
	PrintFromPROGMEM(gTextMsg_AVR_LIBC, 0);
	PrintFromPROGMEMln(gTextMsg_AVR_LIBC_VER_STR, 0);

#if defined(SIGNATURE_0)
	PrintFromPROGMEM(gTextMsg_CPU_SIGNATURE, 0);
	//*	these can be found in avr/iomxxx.h
	PrintHexByte(SIGNATURE_0);
	PrintHexByte(SIGNATURE_1);
	PrintHexByte(SIGNATURE_2);
	PrintNewLine();
#endif


#if defined(GET_LOW_FUSE_BITS)
	//*	fuse settings
	PrintFromPROGMEM(gTextMsg_FUSE_BYTE_LOW, 0);
	fuseByte	=	boot_lock_fuse_bits_get(GET_LOW_FUSE_BITS);
	PrintHexByte(fuseByte);
	PrintNewLine();

	PrintFromPROGMEM(gTextMsg_FUSE_BYTE_HIGH, 0);
	fuseByte	=	boot_lock_fuse_bits_get(GET_HIGH_FUSE_BITS);
	PrintHexByte(fuseByte);
	PrintNewLine();

	PrintFromPROGMEM(gTextMsg_FUSE_BYTE_EXT, 0);
	fuseByte	=	boot_lock_fuse_bits_get(GET_EXTENDED_FUSE_BITS);
	PrintHexByte(fuseByte);
	PrintNewLine();

	PrintFromPROGMEM(gTextMsg_FUSE_BYTE_LOCK, 0);
	fuseByte	=	boot_lock_fuse_bits_get(GET_LOCK_BITS);
	PrintHexByte(fuseByte);
	PrintNewLine();

#endif

}

#ifndef sbi
	#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

//************************************************************************
int analogRead(uint8_t pin)
{
uint8_t low, high;

	// set the analog reference (high two bits of ADMUX) and select the
	// channel (low 4 bits).  this also sets ADLAR (left-adjust result)
	// to 0 (the default).
//	ADMUX	=	(analog_reference << 6) | (pin & 0x07);
	ADMUX	=	(1 << 6) | (pin & 0x07);

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
	// the MUX5 bit of ADCSRB selects whether we're reading from channels
	// 0 to 7 (MUX5 low) or 8 to 15 (MUX5 high).
	ADCSRB	=	(ADCSRB & ~(1 << MUX5)) | (((pin >> 3) & 0x01) << MUX5);
#endif

	// without a delay, we seem to read from the wrong channel
	//delay(1);

	// start the conversion
	sbi(ADCSRA, ADSC);

	// ADSC is cleared when the conversion finishes
	while (bit_is_set(ADCSRA, ADSC));

	// we have to read ADCL first; doing so locks both ADCL
	// and ADCH until ADCH is read.  reading ADCL second would
	// cause the results of each conversion to be discarded,
	// as ADCL and ADCH would be locked when it completed.
	low		=	ADCL;
	high	=	ADCH;

	// combine the two bytes
	return (high << 8) | low;
}

//************************************************************************
static void BlinkLED(void)
{
	PROGLED_DDR		|=	(1<<PROGLED_PIN);
	PROGLED_PORT	|=	(1<<PROGLED_PIN);	// active high LED ON

	while (!Serial_Available())
	{
		PROGLED_PORT	&=	~(1<<PROGLED_PIN);	// turn LED off
		delay_ms(100);
		PROGLED_PORT	|=	(1<<PROGLED_PIN);	// turn LED on
		delay_ms(100);
	}
	recchar();	//	get the char out of the buffer
}

enum
{
	kDUMP_FLASH	=	0,
	kDUMP_EEPROM,
	kDUMP_RAM
};

//************************************************************************
static void	DumpHex(unsigned char dumpWhat, unsigned long startAddress, unsigned char numRows)
{
unsigned long	myAddressPointer;
uint8_t			ii;
unsigned char	theValue;
char			asciiDump[18];
unsigned char	*ramPtr;


	ramPtr				=	0;
	theValue			=	0;
	myAddressPointer	=	startAddress;
	while (numRows > 0)
	{
		if (myAddressPointer > 0x10000)
		{
			PrintHexByte((myAddressPointer >> 16) & 0x00ff);
		}
		PrintHexByte((myAddressPointer >> 8) & 0x00ff);
		PrintHexByte(myAddressPointer & 0x00ff);
		sendchar(0x20);
		sendchar('-');
		sendchar(0x20);

		asciiDump[0]		=	0;
		for (ii=0; ii<16; ii++)
		{
			switch(dumpWhat)
			{
				case kDUMP_FLASH:
					theValue	=	pgm_read_byte_far(myAddressPointer);
					break;

				case kDUMP_EEPROM:
					theValue	=	eeprom_read_byte((void *)myAddressPointer);
					break;

				case kDUMP_RAM:
					theValue	=	ramPtr[myAddressPointer];
					break;

			}
			PrintHexByte(theValue);
			sendchar(0x20);
			if ((theValue >= 0x20) && (theValue < 0x7f))
			{
				asciiDump[ii % 16]	=	theValue;
			}
			else
			{
				asciiDump[ii % 16]	=	'.';
			}

			myAddressPointer++;
		}
		asciiDump[16]	=	0;
		PrintString(asciiDump);
		PrintNewLine();

		numRows--;
	}
}



//************************************************************************
//*	returns amount of extended memory
static void	EEPROMtest(void)
{
int		ii;
char	theChar;
char	theEEPROMchar;
int		errorCount;

	PrintFromPROGMEMln(gTextMsg_WriteToEEprom, 0);
	PrintNewLine();
	ii			=	0;
	while (((theChar = pgm_read_byte_far(gTextMsg_Explorer + ii)) != '*') && (ii < 512))
	{
		eeprom_write_byte((uint8_t *)ii, theChar);
		if (theChar == 0)
		{
			PrintFromPROGMEM(gTextMsg_SPACE, 0);
		}
		else
		{
			sendchar(theChar);
		}
		ii++;
	}

	//*	no go back through and test
	PrintNewLine();
	PrintNewLine();
	PrintFromPROGMEMln(gTextMsg_ReadingEEprom, 0);
	PrintNewLine();
	errorCount	=	0;
	ii			=	0;
	while (((theChar = pgm_read_byte_far(gTextMsg_Explorer + ii)) != '*') && (ii < 512))
	{
		theEEPROMchar	=	eeprom_read_byte((uint8_t *)ii);
		if (theEEPROMchar == 0)
		{
			PrintFromPROGMEM(gTextMsg_SPACE, 0);
		}
		else
		{
			sendchar(theEEPROMchar);
		}
		if (theEEPROMchar != theChar)
		{
			errorCount++;
		}
		ii++;
	}
	PrintNewLine();
	PrintNewLine();
	PrintFromPROGMEM(gTextMsg_EEPROMerrorCnt, 0);
	PrintDecInt(errorCount, 1);
	PrintNewLine();
	PrintNewLine();

	gEepromIndex	=	0;	//*	set index back to zero for next eeprom dump

}



#if (FLASHEND > 0x08000)
	#include	"avrinterruptnames.h"
	#ifndef _INTERRUPT_NAMES_DEFINED_
		#warning Interrupt vectors not defined
	#endif
#endif

//************************************************************************
static void	VectorDisplay(void)
{
unsigned long	byte1;
unsigned long	byte2;
unsigned long	byte3;
unsigned long	byte4;
unsigned long	word1;
unsigned long	word2;
int				vectorIndex;
unsigned long	myMemoryPtr;
unsigned long	wordMemoryAddress;
unsigned long	realitiveAddr;
unsigned long	myFullAddress;
unsigned long	absoluteAddr;
#if defined(_INTERRUPT_NAMES_DEFINED_)
	long		stringPointer;
#endif

	myMemoryPtr		=	0;
	vectorIndex		=	0;
	PrintFromPROGMEMln(gTextMsg_CPU_Name, 0);
	PrintFromPROGMEMln(gTextMsg_VECTOR_HEADER, 0);
	//					 V#   ADDR   op code
	//					  1 - 0000 = C3 BB 00 00 rjmp 03BB >000776 RESET
	while (vectorIndex < kInterruptVectorCount)
	{
		wordMemoryAddress	=	myMemoryPtr / 2;
		//					 01 - 0000 = 12 34
		PrintDecInt(vectorIndex + 1, 2);
		sendchar(0x20);
		sendchar('-');
		sendchar(0x20);
		PrintHexByte((wordMemoryAddress >> 8) & 0x00ff);
		PrintHexByte((wordMemoryAddress) & 0x00ff);
		sendchar(0x20);
		sendchar('=');
		sendchar(0x20);

	
		//*	the AVR is LITTLE ENDIAN, swap the byte order
		byte1	=	pgm_read_byte_far(myMemoryPtr++);
		byte2	=	pgm_read_byte_far(myMemoryPtr++);
		word1	=	(byte2 << 8) + byte1;

		byte3	=	pgm_read_byte_far(myMemoryPtr++);
		byte4	=	pgm_read_byte_far(myMemoryPtr++);
		word2	=	(byte4 << 8) + byte3;


		PrintHexByte(byte2);
		sendchar(0x20);
		PrintHexByte(byte1);
		sendchar(0x20);
		PrintHexByte(byte4);
		sendchar(0x20);
		PrintHexByte(byte3);
		sendchar(0x20);
	
		if (word1 == 0xffff)
		{
			PrintFromPROGMEM(gTextMsg_noVector, 0);
		}
		else if ((word1 & 0xc000) == 0xc000)
		{
			//*	rjmp instruction
			realitiveAddr	=	word1 & 0x3FFF;
			absoluteAddr	=	wordMemoryAddress + realitiveAddr;	//*	add the offset to the current address
			absoluteAddr	=	absoluteAddr << 1;					//*	multiply by 2 for byte address

			PrintFromPROGMEM(gTextMsg_rjmp, 0);
			PrintHexByte((realitiveAddr >> 8) & 0x00ff);
			PrintHexByte((realitiveAddr) & 0x00ff);
			sendchar(0x20);
			sendchar('>');
			PrintHexByte((absoluteAddr >> 16) & 0x00ff);
			PrintHexByte((absoluteAddr >> 8) & 0x00ff);
			PrintHexByte((absoluteAddr) & 0x00ff);
	
		}
		else if ((word1 & 0xfE0E) == 0x940c)
		{
			//*	jmp instruction, this is REALLY complicated, refer to the instruction manual (JMP)
			myFullAddress	=	((byte1 & 0x01) << 16) +
								((byte1 & 0xf0) << 17) +
								((byte2 & 0x01) << 21) +
								word2;
							
			absoluteAddr	=	myFullAddress << 1;
							
			PrintFromPROGMEM(gTextMsg_jmp, 0);
			PrintHexByte((myFullAddress >> 16) & 0x00ff);
			PrintHexByte((myFullAddress >> 8) & 0x00ff);
			PrintHexByte((myFullAddress) & 0x00ff);
			sendchar(0x20);
			sendchar('>');
			PrintHexByte((absoluteAddr >> 16) & 0x00ff);
			PrintHexByte((absoluteAddr >> 8) & 0x00ff);
			PrintHexByte((absoluteAddr) & 0x00ff);
		}

	#if defined(_INTERRUPT_NAMES_DEFINED_)
		sendchar(0x20);
		stringPointer	=	pgm_read_word_far(&(gInterruptNameTable[vectorIndex]));
		PrintFromPROGMEM((char *)stringPointer, 0);
	#endif
		PrintNewLine();

		vectorIndex++;
	}
}

//************************************************************************
static void	PrintAvailablePort(char thePortLetter)
{
	PrintFromPROGMEM(gTextMsg_PORT, 0);
	sendchar(thePortLetter);
	PrintNewLine();
}

//************************************************************************
static void	ListAvailablePorts(void)
{

#ifdef DDRA
	PrintAvailablePort('A');
#endif

#ifdef DDRB
	PrintAvailablePort('B');
#endif

#ifdef DDRC
	PrintAvailablePort('C');
#endif

#ifdef DDRD
	PrintAvailablePort('D');
#endif

#ifdef DDRE
	PrintAvailablePort('E');
#endif

#ifdef DDRF
	PrintAvailablePort('F');
#endif

#ifdef DDRG
	PrintAvailablePort('G');
#endif

#ifdef DDRH
	PrintAvailablePort('H');
#endif

#ifdef DDRI
	PrintAvailablePort('I');
#endif

#ifdef DDRJ
	PrintAvailablePort('J');
#endif

#ifdef DDRK
	PrintAvailablePort('K');
#endif

#ifdef DDRL
	PrintAvailablePort('L');
#endif

}

//************************************************************************
static void	AVR_PortOutput(void)
{
char	portLetter;
char	getCharFlag;

	PrintFromPROGMEM(gTextMsg_WHAT_PORT, 0);

	portLetter	=	recchar();
	portLetter	=	portLetter & 0x5f;
	sendchar(portLetter);
	PrintNewLine();

	if ((portLetter >= 'A') && (portLetter <= 'Z'))
	{
		getCharFlag	=	true;
		switch(portLetter)
		{
		#ifdef DDRA
			case 'A':
				DDRA	=	0xff;
				while (!Serial_Available())
				{
					PORTA	^=	0xff;
					delay_ms(200);
				}
				PORTA	=	0;
				break;
		#endif

		#ifdef DDRB
			case 'B':
				DDRB	=	0xff;
				while (!Serial_Available())
				{
					PORTB	^=	0xff;
					delay_ms(200);
				}
				PORTB	=	0;
				break;
		#endif

		#ifdef DDRC
			case 'C':
				DDRC	=	0xff;
				while (!Serial_Available())
				{
					PORTC	^=	0xff;
					delay_ms(200);
				}
				PORTC	=	0;
				break;
		#endif

		#ifdef DDRD
			case 'D':
				DDRD	=	0xff;
				while (!Serial_Available())
				{
					PORTD	^=	0xff;
					delay_ms(200);
				}
				PORTD	=	0;
				break;
		#endif

		#ifdef DDRE
			case 'E':
				DDRE	=	0xff;
				while (!Serial_Available())
				{
					PORTE	^=	0xff;
					delay_ms(200);
				}
				PORTE	=	0;
				break;
		#endif

		#ifdef DDRF
			case 'F':
				DDRF	=	0xff;
				while (!Serial_Available())
				{
					PORTF	^=	0xff;
					delay_ms(200);
				}
				PORTF	=	0;
				break;
		#endif

		#ifdef DDRG
			case 'G':
				DDRG	=	0xff;
				while (!Serial_Available())
				{
					PORTG	^=	0xff;
					delay_ms(200);
				}
				PORTG	=	0;
				break;
		#endif

		#ifdef DDRH
			case 'H':
				DDRH	=	0xff;
				while (!Serial_Available())
				{
					PORTH	^=	0xff;
					delay_ms(200);
				}
				PORTH	=	0;
				break;
		#endif

		#ifdef DDRI
			case 'I':
				DDRI	=	0xff;
				while (!Serial_Available())
				{
					PORTI	^=	0xff;
					delay_ms(200);
				}
				PORTI	=	0;
				break;
		#endif

		#ifdef DDRJ
			case 'J':
				DDRJ	=	0xff;
				while (!Serial_Available())
				{
					PORTJ	^=	0xff;
					delay_ms(200);
				}
				PORTJ	=	0;
				break;
		#endif

		#ifdef DDRK
			case 'K':
				DDRK	=	0xff;
				while (!Serial_Available())
				{
					PORTK	^=	0xff;
					delay_ms(200);
				}
				PORTK	=	0;
				break;
		#endif

		#ifdef DDRL
			case 'L':
				DDRL	=	0xff;
				while (!Serial_Available())
				{
					PORTL	^=	0xff;
					delay_ms(200);
				}
				PORTL	=	0;
				break;
		#endif

			default:
				PrintFromPROGMEMln(gTextMsg_PortNotSupported, 0);
				getCharFlag	=	false;
				break;
		}
		if (getCharFlag)
		{
			recchar();
		}
	}
	else
	{
		PrintFromPROGMEMln(gTextMsg_MustBeLetter, 0);
	}
}


//*******************************************************************
static void PrintHelp(void)
{
	PrintFromPROGMEMln(gTextMsg_HELP_MSG_0, 0);
	PrintFromPROGMEMln(gTextMsg_HELP_MSG_QM, 0);
	PrintFromPROGMEMln(gTextMsg_HELP_MSG_AT, 0);
	PrintFromPROGMEMln(gTextMsg_HELP_MSG_B, 0);
	PrintFromPROGMEMln(gTextMsg_HELP_MSG_E, 0);
	PrintFromPROGMEMln(gTextMsg_HELP_MSG_F, 0);
	PrintFromPROGMEMln(gTextMsg_HELP_MSG_H, 0);

	PrintFromPROGMEMln(gTextMsg_HELP_MSG_L, 0);
	PrintFromPROGMEMln(gTextMsg_HELP_MSG_Q, 0);
	PrintFromPROGMEMln(gTextMsg_HELP_MSG_R, 0);
	PrintFromPROGMEMln(gTextMsg_HELP_MSG_V, 0);
	PrintFromPROGMEMln(gTextMsg_HELP_MSG_Y, 0);
}

//************************************************************************
static void	RunMonitor(void)
{
char			keepGoing;
unsigned char	theChar;
int				ii, jj;

	for (ii=0; ii<5; ii++)
	{
		for (jj=0; jj<25; jj++)
		{
			sendchar('!');
		}
		PrintNewLine();
	}

	gRamIndex			=	0;
	gFlashIndex			=	0;
	gEepromIndex		=	0;

	PrintFromPROGMEMln(gTextMsg_Explorer, 0);

	keepGoing	=	1;
	while (keepGoing)
	{
		PrintFromPROGMEM(gTextMsg_Prompt, 0);
		theChar	=	recchar();
		if (theChar >= 0x60)
		{
			theChar	=	theChar & 0x5F;
		}
	#if defined( _CEREBOTPLUS_BOARD_ )
		if (theChar == 0x5F)
		{
			
		}
		else
	#endif
		if (theChar >= 0x20)
		{
			sendchar(theChar);
			sendchar(0x20);
		}

		switch(theChar)
		{
			case '0':
				PrintFromPROGMEMln(gTextMsg_HELP_MSG_0, 2);
				gFlashIndex		=	0;
				gRamIndex		=	0;
				gEepromIndex	=	0;
				break;

			case '?':
				PrintFromPROGMEMln(gTextMsg_HELP_MSG_QM, 2);
				PrintCPUstats();
				break;

			case '@':
				PrintFromPROGMEMln(gTextMsg_HELP_MSG_AT, 2);
				EEPROMtest();
				break;

			case 'B':
				PrintFromPROGMEMln(gTextMsg_HELP_MSG_B, 2);
				BlinkLED();
				break;

			case 'E':
				PrintFromPROGMEMln(gTextMsg_HELP_MSG_E, 2);
				DumpHex(kDUMP_EEPROM, gEepromIndex, 16);
				gEepromIndex	+=	256;
				if (gEepromIndex > E2END)
				{
					gEepromIndex	=	0;
				}
				break;
		
			case 'F':
				PrintFromPROGMEMln(gTextMsg_HELP_MSG_F, 2);
				DumpHex(kDUMP_FLASH, gFlashIndex, 16);
				gFlashIndex	+=	256;
				break;

			case 'H':
				PrintFromPROGMEMln(gTextMsg_HELP_MSG_H, 2);
				PrintHelp();
				break;

			case 'L':
				PrintFromPROGMEMln(gTextMsg_HELP_MSG_L, 2);
				ListAvailablePorts();
				break;

			case 'Q':
				PrintFromPROGMEMln(gTextMsg_HELP_MSG_Q, 2);
				keepGoing	=	false;
				break;

			case 'R':
				PrintFromPROGMEMln(gTextMsg_HELP_MSG_R, 2);
				DumpHex(kDUMP_RAM, gRamIndex, 16);
				gRamIndex	+=	256;
				break;

			case 'V':
				PrintFromPROGMEMln(gTextMsg_HELP_MSG_V, 2);
				VectorDisplay();
				break;

			case 'Y':
				PrintFromPROGMEMln(gTextMsg_HELP_MSG_Y, 2);
				AVR_PortOutput();
				break;
			
		#if defined( _CEREBOTPLUS_BOARD_ )
			case 0x5F:
				//*	do nothing
				break;
	 	#endif
	 	
			default:
				PrintFromPROGMEMln(gTextMsg_HUH, 0);
				break;
		}
	}
}

#endif

