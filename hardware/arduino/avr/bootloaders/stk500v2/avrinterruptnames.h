//**************************************************************************************************
//*
//*	interrupt vector names
//*
//*		It is important to note that the vector numbers listed here
//*		are the ATMEL documentation numbers. The Arduino numbers are 1 less
//*		This is because the Atmel docs start numbering the interrupts at 1
//*		when it is actually vector #0 in the table.
//**************************************************************************************************
//*	Jun  1,	2010	<MLS> Added support for ATmega1281
//*	Jun 30,	2010	<MLS> Putting in more ifdefs to conserve space
//*	Jul  3,	2010	<MLS> More #ifdefs to conserve space and testing on most of my boards
//*	Jul  4,	2010	<MLS> Started using vector defs for #ifdefs as defined in <avr/io.h>
//*	Jul 13,	2010	<MLS> Added support for __AVR_ATmega128__
//*	Aug 26,	2010	<MLS> Added support for __AVR_ATmega2561__
//*	Sep 13,	2010	<MLS> Added support for __AVR_AT90CAN32__ __AVR_AT90CAN64__ __AVR_AT90CAN128__
//**************************************************************************************************

//#include	"avrinterruptnames.h"

//**************************************************************************************************
//*	this defines the interrupt vectors and allows us to compile ONLY those strings that are actually 
//*	in the target CPU. This way we do not have to keep making changes based on cpu, it will be 
//*	automatic even if we add a new CPU
#ifndef _AVR_IO_H_
	#include	<avr/io.h>
#endif
//**************************************************************************************************

#ifdef __MWERKS__
	#define	prog_char	char
	#define	PGM_P		char *
#endif

	prog_char	gAvrInt_RESET[]			PROGMEM	=	"RESET";
#ifdef INT0_vect
	prog_char	gAvrInt_INT0[]			PROGMEM	=	"INT0";
#endif
#ifdef INT1_vect
	prog_char	gAvrInt_INT1[]			PROGMEM	=	"INT1";
#endif
#ifdef INT2_vect
	prog_char	gAvrInt_INT2[]			PROGMEM	=	"INT2";
#endif
#ifdef INT3_vect
	prog_char	gAvrInt_INT3[]			PROGMEM	=	"INT3";
#endif
#ifdef INT4_vect
	prog_char	gAvrInt_INT4[]			PROGMEM	=	"INT4";
#endif
#ifdef INT5_vect
	prog_char	gAvrInt_INT5[]			PROGMEM	=	"INT5";
#endif
#ifdef INT6_vect
	prog_char	gAvrInt_INT6[]			PROGMEM	=	"INT6";
#endif
#ifdef INT7_vect
	prog_char	gAvrInt_INT7[]			PROGMEM	=	"INT7";
#endif
#ifdef PCINT0_vect
	prog_char	gAvrInt_PCINT0[]		PROGMEM	=	"PCINT0";
#endif
#ifdef PCINT1_vect
	prog_char	gAvrInt_PCINT1[]		PROGMEM	=	"PCINT1";
#endif
#ifdef PCINT2_vect
	prog_char	gAvrInt_PCINT2[]		PROGMEM	=	"PCINT2";
#endif
#ifdef PCINT3_vect
	prog_char	gAvrInt_PCINT3[]		PROGMEM	=	"PCINT3";
#endif
#ifdef WDT_vect
	prog_char	gAvrInt_WDT[]			PROGMEM	=	"WDT";
#endif
#ifdef TIMER0_COMP_vect
	prog_char	gAvrInt_TIMER0_COMP[]	PROGMEM	=	"TIMER0 COMP";
#endif
#ifdef TIMER0_COMPA_vect
	prog_char	gAvrInt_TIMER0_COMPA[]	PROGMEM	=	"TIMER0 COMPA";
#endif
#ifdef TIMER0_COMPB_vect
	prog_char	gAvrInt_TIMER0_COMPB[]	PROGMEM	=	"TIMER0 COMPB";
#endif
#ifdef TIMER0_OVF_vect
	prog_char	gAvrInt_TIMER0_OVF[]	PROGMEM	=	"TIMER0 OVF";
#endif
#ifdef TIMER1_CAPT_vect
	prog_char	gAvrInt_TIMER1_CAPT[]	PROGMEM	=	"TIMER1 CAPT";
#endif
#ifdef TIMER1_COMPA_vect
	prog_char	gAvrInt_TIMER1_COMPA[]	PROGMEM	=	"TIMER1 COMPA";
#endif
#ifdef TIMER1_COMPB_vect
	prog_char	gAvrInt_TIMER1_COMPB[]	PROGMEM	=	"TIMER1 COMPB";
#endif
#ifdef TIMER1_COMPC_vect
	prog_char	gAvrInt_TIMER1_COMPC[]	PROGMEM	=	"TIMER1 COMPC";
#endif
#ifdef TIMER1_OVF_vect
	prog_char	gAvrInt_TIMER1_OVF[]	PROGMEM	=	"TIMER1 OVF";
#endif
#ifdef TIMER2_COMP_vect
	prog_char	gAvrInt_TIMER2_COMP[]	PROGMEM	=	"TIMER2 COMP";
#endif
#ifdef TIMER2_COMPA_vect
	prog_char	gAvrInt_TIMER2_COMPA[]	PROGMEM	=	"TIMER2 COMPA";
#endif
#ifdef TIMER2_COMPB_vect
	prog_char	gAvrInt_TIMER2_COMPB[]	PROGMEM	=	"TIMER2 COMPB";
#endif
#ifdef TIMER2_OVF_vect
	prog_char	gAvrInt_TIMER2_OVF[]	PROGMEM	=	"TIMER2 OVF";
#endif
#ifdef TIMER3_CAPT_vect
	prog_char	gAvrInt_TIMER3_CAPT[]	PROGMEM	=	"TIMER3 CAPT";
#endif
#ifdef TIMER3_COMPA_vect
	prog_char	gAvrInt_TIMER3_COMPA[]	PROGMEM	=	"TIMER3 COMPA";
#endif
#ifdef TIMER3_COMPB_vect
	prog_char	gAvrInt_TIMER3_COMPB[]	PROGMEM	=	"TIMER3 COMPB";
#endif
#ifdef TIMER3_COMPC_vect
	prog_char	gAvrInt_TIMER3_COMPC[]	PROGMEM	=	"TIMER3 COMPC";
#endif
#ifdef TIMER3_OVF_vect
	prog_char	gAvrInt_TIMER3_OVF[]	PROGMEM	=	"TIMER3 OVF";
#endif
#ifdef TIMER4_CAPT_vect
	prog_char	gAvrInt_TIMER4_CAPT[]	PROGMEM	=	"TIMER4 CAPT";
#endif
#ifdef TIMER4_COMPA_vect
	prog_char	gAvrInt_TIMER4_COMPA[]	PROGMEM	=	"TIMER4 COMPA";
#endif
#ifdef TIMER4_COMPB_vect
	prog_char	gAvrInt_TIMER4_COMPB[]	PROGMEM	=	"TIMER4 COMPB";
#endif
#ifdef TIMER4_COMPC_vect
	prog_char	gAvrInt_TIMER4_COMPC[]	PROGMEM	=	"TIMER4 COMPC";
#endif
#ifdef TIMER4_COMPD_vect
	prog_char	gAvrInt_TIMER4_COMPD[]	PROGMEM	=	"TIMER4 COMPD";
#endif
#ifdef TIMER4_OVF_vect
	prog_char	gAvrInt_TIMER4_OVF[]	PROGMEM	=	"TIMER4 OVF";
#endif
#ifdef TIMER4_FPF_vect
	prog_char	gAvrInt_TIMER4_FPF[]	PROGMEM	=	"TIMER4 Fault Protection";
#endif
#ifdef TIMER5_CAPT_vect
	prog_char	gAvrInt_TIMER5_CAPT[]	PROGMEM	=	"TIMER5 CAPT";
#endif
#ifdef TIMER5_COMPA_vect
	prog_char	gAvrInt_TIMER5_COMPA[]	PROGMEM	=	"TIMER5 COMPA";
#endif
#ifdef TIMER5_COMPB_vect
	prog_char	gAvrInt_TIMER5_COMPB[]	PROGMEM	=	"TIMER5 COMPB";
#endif
#ifdef TIMER5_COMPC_vect
	prog_char	gAvrInt_TIMER5_COMPC[]	PROGMEM	=	"TIMER5 COMPC";
#endif
#ifdef TIMER5_OVF_vect
	prog_char	gAvrInt_TIMER5_OVF[]	PROGMEM	=	"TIMER5 OVF";
#endif

//*	when there is only 1 usart
#if defined(USART_RX_vect) || defined(USART_RXC_vect)
	prog_char	gAvrInt_USART_RX[]		PROGMEM	=	"USART RX";
#endif
#if defined(USART_UDRE_vect)
	prog_char	gAvrInt_USART_UDRE[]	PROGMEM	=	"USART UDRE";
#endif
#if defined(USART_TX_vect) || defined(USART_TXC_vect)
	prog_char	gAvrInt_USART_TX[]		PROGMEM	=	"USART TX";
#endif


//*	usart 0
#if defined(USART0_RX_vect)
	prog_char	gAvrInt_USART0_RX[]		PROGMEM	=	"USART0 RX";
#endif
#if defined(USART0_UDRE_vect)
	prog_char	gAvrInt_USART0_UDRE[]	PROGMEM	=	"USART0 UDRE";
#endif
#if defined(USART0_TX_vect)
	prog_char	gAvrInt_USART0_TX[]		PROGMEM	=	"USART0 TX";
#endif


//*	usart 1
#ifdef USART1_RX_vect
	prog_char	gAvrInt_USART1_RX[]		PROGMEM	=	"USART1 RX";
#endif
#ifdef USART1_UDRE_vect
	prog_char	gAvrInt_USART1_UDRE[]	PROGMEM	=	"USART1 UDRE";
#endif
#ifdef USART1_TX_vect
	prog_char	gAvrInt_USART1_TX[]		PROGMEM	=	"USART1 TX";
#endif

//*	usart 2
#ifdef USART2_RX_vect
	prog_char	gAvrInt_USART2_RX[]		PROGMEM	=	"USART2 RX";
#endif
#ifdef USART2_UDRE_vect
	prog_char	gAvrInt_USART2_UDRE[]	PROGMEM	=	"USART2 UDRE";
#endif
#ifdef USART2_TX_vect
	prog_char	gAvrInt_USART2_TX[]		PROGMEM	=	"USART2 TX";
#endif

//*	usart 3
#ifdef USART3_RX_vect
	prog_char	gAvrInt_USART3_RX[]		PROGMEM	=	"USART3 RX";
#endif
#ifdef USART3_UDRE_vect
	prog_char	gAvrInt_USART3_UDRE[]	PROGMEM	=	"USART3 UDRE";
#endif
#ifdef USART3_TX_vect
	prog_char	gAvrInt_USART3_TX[]		PROGMEM	=	"USART3 TX";
#endif
#ifdef SPI_STC_vect
	prog_char	gAvrInt_SPI_STC[]		PROGMEM	=	"SPI STC";
#endif
#ifdef ADC_vect
	prog_char	gAvrInt_ADC[]			PROGMEM	=	"ADC";
#endif
#if defined(ANALOG_COMP_vect) || defined(ANA_COMP_vect)
	prog_char	gAvrInt_ANALOG_COMP[]	PROGMEM	=	"ANALOG COMP";
#endif
#if defined(EE_READY_vect) || defined(EE_RDY_vect)
	prog_char	gAvrInt_EE_READY[]		PROGMEM	=	"EE READY";
#endif
#ifdef TWI_vect
	prog_char	gAvrInt_TWI[]			PROGMEM	=	"TWI";
#endif
#if defined(SPM_READY_vect) || defined(SPM_RDY_vect)
	prog_char	gAvrInt_SPM_READY[]		PROGMEM	=	"SPM READY";
#endif
#ifdef USI_START_vect
	prog_char	gAvrInt_USI_START[]		PROGMEM	=	"USI START";
#endif
#ifdef USI_OVERFLOW_vect
	prog_char	gAvrInt_USI_OVERFLOW[]	PROGMEM	=	"USI OVERFLOW";
#endif
#ifdef USB_GEN_vect
	prog_char	gAvrInt_USB_General[]	PROGMEM	=	"USB General";
#endif
#ifdef USB_COM_vect
	prog_char	gAvrInt_USB_Endpoint[]	PROGMEM	=	"USB Endpoint";
#endif

#ifdef LCD_vect
	prog_char	gAvrInt_LCD_StartFrame[]	PROGMEM	=	"LCD Start of Frame";
#endif

//*	these are for the chips with CAN bus support
#ifdef CANIT_vect
	prog_char	gAvrInt_CAN_TrafnsferCE[]	PROGMEM	=	"CAN Transfer Complete or Error";
#endif
#ifdef OVRIT_vect
	prog_char	gAvrInt_CAN_TimerOverRun[]	PROGMEM	=	"CAN Timer Overrun";
#endif

//*	these are for __AVR_ATmega128RFA1__
#ifdef TRX24_PLL_LOCK_vect
	prog_char	gAvrInt_TRN_PLL_LOCK[]		PROGMEM	=	"TRX24_PLL_LOCK";
#endif
#ifdef TRX24_PLL_UNLOCK_vect
	prog_char	gAvrInt_TRN_PLL_UNLOCK[]	PROGMEM	=	"TRX24_PLL_UNLOCK";
#endif
#ifdef TRX24_RX_START_vect
	prog_char	gAvrInt_TRN_RX_START[]		PROGMEM	=	"TRX24_RX_START";
#endif
#ifdef TRX24_RX_END_vect
	prog_char	gAvrInt_TRN_RX_END[]		PROGMEM	=	"TRX24_RX_END";
#endif
#ifdef TRX24_CCA_ED_DONE_vect
	prog_char	gAvrInt_TRN_CAAED_DONE[]	PROGMEM	=	"TRX24_CCA_ED_DONE";
#endif
#ifdef TRX24_XAH_AMI_vect
	prog_char	gAvrInt_TRN_FRAME_MATCH[]	PROGMEM	=	"TRX24_FRAME_ADDRESS_MATCH";
#endif
#ifdef TRX24_TX_END_vect
	prog_char	gAvrInt_TRN_TX_END[]		PROGMEM	=	"TRX24_TX_END";
#endif
#ifdef TRX24_AWAKE_vect
	prog_char	gAvrInt_TRN_AWAKE[]			PROGMEM	=	"TRX24_AWAKE";
#endif
#ifdef SCNT_CMP1_vect
	prog_char	gAvrInt_SCNT_CMP1[]			PROGMEM	=	"SCNT_CMP1";
#endif
#ifdef SCNT_CMP2_vect
	prog_char	gAvrInt_SCNT_CMP2[]			PROGMEM	=	"SCNT_CMP2";
#endif
#ifdef SCNT_CMP3_vect
	prog_char	gAvrInt_SCNT_CMP3[]			PROGMEM	=	"SCNT_CMP3";
#endif
#ifdef SCNT_OVFL_vect
	prog_char	gAvrInt_SCNT_OVFL[]			PROGMEM	=	"SCNT_OVFL";
#endif
#ifdef SCNT_BACKOFF_vect
	prog_char	gAvrInt_SCNT_BACKOFF[]		PROGMEM	=	"SCNT_BACKOFF";
#endif
#ifdef AES_READY_vect
	prog_char	gAvrInt_AES_READY[]			PROGMEM	=	"AES_READY";
#endif
#ifdef BAT_LOW_vect
	prog_char	gAvrInt_BAT_LOW[]			PROGMEM	=	"BAT_LOW";
#endif



//**************************************************************************************************
//*	these do not have vector defs and have to be done by CPU type
#if defined(__AVR_ATmega645__ ) || defined(__AVR_ATmega1281__) || defined(__AVR_ATmega2561__)
	prog_char	gAvrInt_NOT_USED[]		PROGMEM	=	"NOT_USED";
#endif
#if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega128RFA1__)
	prog_char	gAvrInt_RESERVED[]		PROGMEM	=	"Reserved";
#endif

	prog_char	gAvrInt_END[]			PROGMEM	=	"*";





//**************************************************************************************************
#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__)
#pragma mark __AVR_ATmega168__ / __AVR_ATmega328P__

#define	_INTERRUPT_NAMES_DEFINED_

PGM_P gInterruptNameTable[]	PROGMEM =
{

	gAvrInt_RESET,			//	1
	gAvrInt_INT0,			//	2
	gAvrInt_INT1,			//	3
	gAvrInt_PCINT0,			//	4
	gAvrInt_PCINT1,			//	5
	gAvrInt_PCINT2,			//	6
	gAvrInt_WDT,			//	7
	gAvrInt_TIMER2_COMPA,	//	8
	gAvrInt_TIMER2_COMPB,	//	9
	gAvrInt_TIMER2_OVF,		//	10
	gAvrInt_TIMER1_CAPT,	//	11
	gAvrInt_TIMER1_COMPA,	//	12
	gAvrInt_TIMER1_COMPB,	//	13
	gAvrInt_TIMER1_OVF,		//	14
	gAvrInt_TIMER0_COMPA,	//	15
	gAvrInt_TIMER0_COMPB,	//	16
	gAvrInt_TIMER0_OVF,		//	17
	gAvrInt_SPI_STC,		//	18
	gAvrInt_USART_RX,		//	19
	gAvrInt_USART_UDRE,	//	20
	gAvrInt_USART_TX,		//	21
	gAvrInt_ADC,			//	22
	gAvrInt_EE_READY,		//	23
	gAvrInt_ANALOG_COMP,	//	24
	gAvrInt_TWI,			//	25
	gAvrInt_SPM_READY,		//	26
};

#endif

//**************************************************************************************************
#if defined(__AVR_ATmega169__)
#pragma mark __AVR_ATmega169__

#define	_INTERRUPT_NAMES_DEFINED_

PGM_P gInterruptNameTable[]	PROGMEM =
{

	gAvrInt_RESET,			//	1
	gAvrInt_INT0,			//	2
	gAvrInt_PCINT0,			//	3
	gAvrInt_PCINT1,			//	4
	gAvrInt_TIMER2_COMP,	//	5
	gAvrInt_TIMER2_OVF,		//	6
	gAvrInt_TIMER1_CAPT,	//	7
	gAvrInt_TIMER1_COMPA,	//	8
	gAvrInt_TIMER1_COMPB,	//	9
	gAvrInt_TIMER1_OVF,		//	10
	gAvrInt_TIMER0_COMP,	//	11
	gAvrInt_TIMER0_OVF,		//	12
	gAvrInt_SPI_STC,		//	13
	gAvrInt_USART0_RX,		//	14
	gAvrInt_USART0_UDRE,	//	15
	gAvrInt_USART0_TX,		//	16
	gAvrInt_USI_START,		//	17
	gAvrInt_USI_OVERFLOW,	//	18
	gAvrInt_ANALOG_COMP,	//	19
	gAvrInt_ADC,			//	20
	gAvrInt_EE_READY,		//	21
	gAvrInt_SPM_READY,		//	22
	gAvrInt_LCD_StartFrame,	//	23

};

#endif


//**************************************************************************************************
#if defined(__AVR_ATmega640__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega1281__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2561__)
#pragma mark __AVR_ATmega640__ __AVR_ATmega1280__ __AVR_ATmega1281__ __AVR_ATmega2560__ __AVR_ATmega2561__

#define	_INTERRUPT_NAMES_DEFINED_

PGM_P gInterruptNameTable[]	PROGMEM =
{

	gAvrInt_RESET,			//	1
	gAvrInt_INT0,			//	2
	gAvrInt_INT1,			//	3
	gAvrInt_INT2,			//	4
	gAvrInt_INT3,			//	5
	gAvrInt_INT4,			//	6
	gAvrInt_INT5,			//	7
	gAvrInt_INT6,			//	8
	gAvrInt_INT7,			//	9
	gAvrInt_PCINT0,			//	10
	gAvrInt_PCINT1,			//	11
#if defined(__AVR_ATmega640__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
	gAvrInt_PCINT2,			//	12
#else
	gAvrInt_NOT_USED,		//	12
#endif
	gAvrInt_WDT,			//	13
	gAvrInt_TIMER2_COMPA,	//	14
	gAvrInt_TIMER2_COMPB,	//	15
	gAvrInt_TIMER2_OVF,		//	16
	gAvrInt_TIMER1_CAPT,	//	17
	gAvrInt_TIMER1_COMPA,	//	18
	gAvrInt_TIMER1_COMPB,	//	19
	gAvrInt_TIMER1_COMPC,	//	20
	gAvrInt_TIMER1_OVF,		//	21
	gAvrInt_TIMER0_COMPA,	//	22
	gAvrInt_TIMER0_COMPB,	//	23
	gAvrInt_TIMER0_OVF,		//	24
	gAvrInt_SPI_STC,		//	25

	gAvrInt_USART0_RX,		//	26
	gAvrInt_USART0_UDRE,	//	27
	gAvrInt_USART0_TX,		//	28
	gAvrInt_ANALOG_COMP,	//	29
	gAvrInt_ADC,			//	30
	gAvrInt_EE_READY,		//	31

	gAvrInt_TIMER3_CAPT,	//	32
	gAvrInt_TIMER3_COMPA,	//	33
	gAvrInt_TIMER3_COMPB,	//	34
	gAvrInt_TIMER3_COMPC,	//	35
	gAvrInt_TIMER3_OVF,		//	36

	gAvrInt_USART1_RX,		//	37
	gAvrInt_USART1_UDRE,	//	38
	gAvrInt_USART1_TX,		//	39
	gAvrInt_TWI,			//	40
	gAvrInt_SPM_READY,		//	41
#if defined(__AVR_ATmega640__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
	gAvrInt_TIMER4_CAPT,	//	42
#else
	gAvrInt_NOT_USED,		//	42
#endif
	gAvrInt_TIMER4_COMPA,	//	43
	gAvrInt_TIMER4_COMPB,	//	44
	gAvrInt_TIMER4_COMPC,	//	45
	gAvrInt_TIMER4_OVF,		//	46
#if defined(__AVR_ATmega640__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
	gAvrInt_TIMER5_CAPT,	//	47
#else
	gAvrInt_NOT_USED,		//	47
#endif
	gAvrInt_TIMER5_COMPA,	//	48
	gAvrInt_TIMER5_COMPB,	//	49
	gAvrInt_TIMER5_COMPC,	//	50
	gAvrInt_TIMER5_OVF,		//	51

#if defined(__AVR_ATmega640__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
	gAvrInt_USART2_RX,		//	52
	gAvrInt_USART2_UDRE,	//	53
	gAvrInt_USART2_TX,		//	54

	gAvrInt_USART3_RX,		//	55
	gAvrInt_USART3_UDRE,	//	56
	gAvrInt_USART3_TX,		//	57
#endif

};

#endif



//**************************************************************************************************
#if defined(__AVR_ATmega324P__ ) || defined(__AVR_ATmega644__ ) || defined(__AVR_ATmega644P__)
#pragma mark __AVR_ATmega324P__  __AVR_ATmega644__ __AVR_ATmega644P__ 

#define	_INTERRUPT_NAMES_DEFINED_

PGM_P gInterruptNameTable[]	PROGMEM =
{

	gAvrInt_RESET,			//	1
	gAvrInt_INT0,			//	2
	gAvrInt_INT1,			//	3
	gAvrInt_INT2,			//	4
	gAvrInt_PCINT0,			//	5
	gAvrInt_PCINT1,			//	6
	gAvrInt_PCINT2,			//	7
	gAvrInt_PCINT3,			//	8
	gAvrInt_WDT,			//	9
	gAvrInt_TIMER2_COMPA,	//	10
	gAvrInt_TIMER2_COMPB,	//	11
	gAvrInt_TIMER2_OVF,		//	12
	gAvrInt_TIMER1_CAPT,	//	13
	gAvrInt_TIMER1_COMPA,	//	14
	gAvrInt_TIMER1_COMPB,	//	15
	gAvrInt_TIMER1_OVF,		//	16
	gAvrInt_TIMER0_COMPA,	//	17
	gAvrInt_TIMER0_COMPB,	//	18
	gAvrInt_TIMER0_OVF,		//	19
	gAvrInt_SPI_STC,		//	20
	gAvrInt_USART0_RX,		//	21
	gAvrInt_USART0_UDRE,	//	22
	gAvrInt_USART0_TX,		//	23
	gAvrInt_ANALOG_COMP,	//	24
	gAvrInt_ADC,			//	25
	gAvrInt_EE_READY,		//	26
	gAvrInt_TWI,			//	27
	gAvrInt_SPM_READY,		//	28

#if defined(__AVR_ATmega324P__ ) || defined(__AVR_ATmega644P__)
	gAvrInt_USART1_RX,		//	29
	gAvrInt_USART1_UDRE,	//	30
	gAvrInt_USART1_TX,		//	31
#endif

};


#endif

//**************************************************************************************************
#if defined(__AVR_ATmega1284P__ )
#pragma mark __AVR_ATmega1284P__

#define	_INTERRUPT_NAMES_DEFINED_

PGM_P gInterruptNameTable[]	PROGMEM =
{

	gAvrInt_RESET,			//	1
	gAvrInt_INT0,			//	2
	gAvrInt_INT1,			//	3
	gAvrInt_INT2,			//	4
	gAvrInt_PCINT0,			//	5
	gAvrInt_PCINT1,			//	6
	gAvrInt_PCINT2,			//	7
	gAvrInt_PCINT3,			//	8
	gAvrInt_WDT,			//	9
	gAvrInt_TIMER2_COMPA,	//	10
	gAvrInt_TIMER2_COMPB,	//	11
	gAvrInt_TIMER2_OVF,		//	12
	gAvrInt_TIMER1_CAPT,	//	13
	gAvrInt_TIMER1_COMPA,	//	14
	gAvrInt_TIMER1_COMPB,	//	15
	gAvrInt_TIMER1_OVF,		//	16
	gAvrInt_TIMER0_COMPA,	//	17
	gAvrInt_TIMER0_COMPB,	//	18
	gAvrInt_TIMER0_OVF,		//	19
	gAvrInt_SPI_STC,		//	20
	gAvrInt_USART0_RX,		//	21
	gAvrInt_USART0_UDRE,	//	22
	gAvrInt_USART0_TX,		//	23
	gAvrInt_ANALOG_COMP,	//	24
	gAvrInt_ADC,			//	25
	gAvrInt_EE_READY,		//	26
	gAvrInt_TWI,			//	27
	gAvrInt_SPM_READY,		//	28

	gAvrInt_USART1_RX,		//	29
	gAvrInt_USART1_UDRE,	//	30
	gAvrInt_USART1_TX,		//	31
	//*	these are NOT documented in  doc8272.pdf 
	//*	they are in iom1284p.h
	gAvrInt_TIMER3_CAPT,	//	32
	gAvrInt_TIMER3_COMPA,	//	33
	gAvrInt_TIMER3_COMPB,	//	34
	gAvrInt_TIMER3_OVF,		//	35
	

};


#endif


//**************************************************************************************************
#if defined(__AVR_ATmega645__ )
#pragma mark __AVR_ATmega645__

#define	_INTERRUPT_NAMES_DEFINED_

PGM_P gInterruptNameTable[]	PROGMEM =
{

	gAvrInt_RESET,			//	1
	gAvrInt_INT0,			//	2
	gAvrInt_PCINT0,			//	3
	gAvrInt_PCINT1,			//	4
	gAvrInt_TIMER2_COMP,	//	5
	gAvrInt_TIMER2_OVF,		//	6
	gAvrInt_TIMER1_CAPT,	//	7
	gAvrInt_TIMER1_COMPA,	//	8
	gAvrInt_TIMER1_COMPB,	//	9
	gAvrInt_TIMER1_OVF,		//	10
	gAvrInt_TIMER0_COMP,	//	11
	gAvrInt_TIMER0_OVF,		//	12
	gAvrInt_SPI_STC,		//	13
	gAvrInt_USART0_RX,		//	14
	gAvrInt_USART0_UDRE,	//	15
	gAvrInt_USART0_TX,		//	16
	gAvrInt_USI_START,		//	17
	gAvrInt_USI_OVERFLOW,	//	18
	gAvrInt_ANALOG_COMP,	//	19
	gAvrInt_ADC,			//	20
	gAvrInt_EE_READY,		//	21
	gAvrInt_SPM_READY,		//	22
	gAvrInt_NOT_USED,		//	23

#if defined(__AVR_ATmega3250__) || defined(__AVR_ATmega6450__)
	gAvrInt_PCINT2,			//	24
	gAvrInt_PCINT3,			//	25
#endif
};


#endif

//**************************************************************************************************
#if defined(__AVR_ATmega16__ )
#pragma mark __AVR_ATmega16__

#define	_INTERRUPT_NAMES_DEFINED_

PGM_P gInterruptNameTable[]	PROGMEM =
{

	gAvrInt_RESET,			//	1
	gAvrInt_INT0,			//	2
	gAvrInt_INT1,			//	3
	gAvrInt_TIMER2_COMP,	//	4
	gAvrInt_TIMER2_OVF,		//	5
	gAvrInt_TIMER1_CAPT,	//	6
	gAvrInt_TIMER1_COMPA,	//	7
	gAvrInt_TIMER1_COMPB,	//	8
	gAvrInt_TIMER1_OVF,		//	9
	gAvrInt_TIMER0_OVF,		//	10
	gAvrInt_SPI_STC,		//	11
	gAvrInt_USART_RX,		//	12
	gAvrInt_USART_UDRE,		//	13
	gAvrInt_USART_TX,		//	14
	gAvrInt_ADC,			//	15
	gAvrInt_EE_READY,		//	16
	gAvrInt_ANALOG_COMP,	//	17
	gAvrInt_TWI,			//	18
	gAvrInt_INT2,			//	19
	gAvrInt_TIMER0_COMP,	//	20
	gAvrInt_SPM_READY,		//	21

};


#endif

//**************************************************************************************************
#if defined(__AVR_ATmega32__ )
#pragma mark __AVR_ATmega32__

#define	_INTERRUPT_NAMES_DEFINED_

PGM_P gInterruptNameTable[]	PROGMEM =
{

	gAvrInt_RESET,			//	1
	gAvrInt_INT0,			//	2
	gAvrInt_INT1,			//	3
	gAvrInt_INT2,			//	4
	gAvrInt_TIMER2_COMP,	//	5
	gAvrInt_TIMER2_OVF,		//	6
	gAvrInt_TIMER1_CAPT,	//	7
	gAvrInt_TIMER1_COMPA,	//	8
	gAvrInt_TIMER1_COMPB,	//	9
	gAvrInt_TIMER1_OVF,		//	10
	gAvrInt_TIMER0_COMP,	//	11
	gAvrInt_TIMER0_OVF,		//	12
	gAvrInt_SPI_STC,		//	13
	gAvrInt_USART_RX,		//	14
	gAvrInt_USART_UDRE,		//	15
	gAvrInt_USART_TX,		//	16
	gAvrInt_ADC,			//	17
	gAvrInt_EE_READY,		//	18
	gAvrInt_ANALOG_COMP,	//	19
	gAvrInt_TWI,			//	20
	gAvrInt_SPM_READY,		//	21

};


#endif

//**************************************************************************************************
#if defined(__AVR_ATmega32U4__)
#pragma mark __AVR_ATmega32U4__
//*	teensy 2.0
//*	http://www.pjrc.com/teensy/pinout.html
#define	_INTERRUPT_NAMES_DEFINED_


PGM_P gInterruptNameTable[]	PROGMEM =
{

	gAvrInt_RESET,			//	1
	gAvrInt_INT0,			//	2
	gAvrInt_INT1,			//	3
	gAvrInt_INT2,			//	4
	gAvrInt_INT3,			//	5
	gAvrInt_RESERVED,		//	6
	gAvrInt_RESERVED,		//	7
	gAvrInt_INT6,			//	8
	gAvrInt_RESERVED,		//	9
	gAvrInt_PCINT0,			//	10
	gAvrInt_USB_General,	//	11
	gAvrInt_USB_Endpoint,	//	12
	gAvrInt_WDT,			//	13
	gAvrInt_RESERVED,		//	14
	gAvrInt_RESERVED,		//	15
	gAvrInt_RESERVED,		//	16
	gAvrInt_TIMER1_CAPT,	//	17
	gAvrInt_TIMER1_COMPA,	//	18
	gAvrInt_TIMER1_COMPB,	//	19
	gAvrInt_TIMER1_COMPC,	//	20
	gAvrInt_TIMER1_OVF,		//	21
	gAvrInt_TIMER0_COMPA,	//	22
	gAvrInt_TIMER0_COMPB,	//	23
	gAvrInt_TIMER0_OVF,		//	24
	gAvrInt_SPI_STC,		//	25

	gAvrInt_USART1_RX,		//	26
	gAvrInt_USART1_UDRE,	//	27
	gAvrInt_USART1_TX,		//	28
	gAvrInt_ANALOG_COMP,	//	29
	
	gAvrInt_ADC,			//	30
	gAvrInt_EE_READY,		//	31

	gAvrInt_TIMER3_CAPT,	//	32
	gAvrInt_TIMER3_COMPA,	//	33
	gAvrInt_TIMER3_COMPB,	//	34
	gAvrInt_TIMER3_COMPC,	//	35
	gAvrInt_TIMER3_OVF,		//	36
	gAvrInt_TWI,			//	37
	gAvrInt_SPM_READY,		//	38

	gAvrInt_TIMER4_COMPA,	//	39
	gAvrInt_TIMER4_COMPB,	//	40
	gAvrInt_TIMER4_COMPD,	//	41
	gAvrInt_TIMER4_OVF,		//	42
	gAvrInt_TIMER4_FPF,		//	43
};

#endif

//**************************************************************************************************
#if defined(__AVR_AT90USB1286__) ||  defined(__AVR_AT90USB1287__)
#pragma mark __AVR_AT90USB1286__
//*	teensy++ 2.0
//*	http://www.pjrc.com/teensy/pinout.html
#define	_INTERRUPT_NAMES_DEFINED_


PGM_P gInterruptNameTable[]	PROGMEM =
{

	gAvrInt_RESET,			//	1
	gAvrInt_INT0,			//	2
	gAvrInt_INT1,			//	3
	gAvrInt_INT2,			//	4
	gAvrInt_INT3,			//	5
	gAvrInt_INT4,			//	6
	gAvrInt_INT5,			//	7
	gAvrInt_INT6,			//	8
	gAvrInt_INT7,			//	9
	gAvrInt_PCINT0,			//	10
	gAvrInt_USB_General,	//	11
	gAvrInt_USB_Endpoint,	//	12
	gAvrInt_WDT,			//	13
	gAvrInt_TIMER2_COMPA,	//	14
	gAvrInt_TIMER2_COMPB,	//	15
	gAvrInt_TIMER2_OVF,		//	16
	gAvrInt_TIMER1_CAPT,	//	17
	gAvrInt_TIMER1_COMPA,	//	18
	gAvrInt_TIMER1_COMPB,	//	19
	gAvrInt_TIMER1_COMPC,	//	20
	gAvrInt_TIMER1_OVF,		//	21
	gAvrInt_TIMER0_COMPA,	//	22
	gAvrInt_TIMER0_COMPB,	//	23
	gAvrInt_TIMER0_OVF,		//	24
	gAvrInt_SPI_STC,		//	25

	gAvrInt_USART1_RX,		//	26
	gAvrInt_USART1_UDRE,	//	27
	gAvrInt_USART1_TX,		//	28
	gAvrInt_ANALOG_COMP,	//	29
	
	gAvrInt_ADC,			//	30
	gAvrInt_EE_READY,		//	31

	gAvrInt_TIMER3_CAPT,	//	32
	gAvrInt_TIMER3_COMPA,	//	33
	gAvrInt_TIMER3_COMPB,	//	34
	gAvrInt_TIMER3_COMPC,	//	35
	gAvrInt_TIMER3_OVF,		//	36
	gAvrInt_TWI,			//	37
	gAvrInt_SPM_READY,		//	38

};

#endif




//**************************************************************************************************
#if defined(__AVR_ATmega128__) || defined(__AVR_ATmega64__)
#pragma mark __AVR_ATmega64__ __AVR_ATmega128__
#define	_INTERRUPT_NAMES_DEFINED_


PGM_P gInterruptNameTable[]	PROGMEM =
{

	gAvrInt_RESET,			//	1
	gAvrInt_INT0,			//	2
	gAvrInt_INT1,			//	3
	gAvrInt_INT2,			//	4
	gAvrInt_INT3,			//	5
	gAvrInt_INT4,			//	6
	gAvrInt_INT5,			//	7
	gAvrInt_INT6,			//	8
	gAvrInt_INT7,			//	9
	gAvrInt_TIMER2_COMP,	//	10
	gAvrInt_TIMER2_OVF,		//	11
	gAvrInt_TIMER1_CAPT,	//	12
	gAvrInt_TIMER1_COMPA,	//	13
	gAvrInt_TIMER1_COMPB,	//	14
	gAvrInt_TIMER1_OVF,		//	15
	gAvrInt_TIMER0_COMP,	//	16
	gAvrInt_TIMER0_OVF,		//	17
	gAvrInt_SPI_STC,		//	18
	gAvrInt_USART0_RX,		//	19
	gAvrInt_USART0_UDRE,	//	20
	gAvrInt_USART0_TX,		//	21
	gAvrInt_ADC,			//	22
	gAvrInt_EE_READY,		//	23
	gAvrInt_ANALOG_COMP,	//	24
	gAvrInt_TIMER1_COMPC,	//	25
	gAvrInt_TIMER3_CAPT,	//	26
	gAvrInt_TIMER3_COMPA,	//	27
	gAvrInt_TIMER3_COMPB,	//	28
	gAvrInt_TIMER3_COMPC,	//	29
	gAvrInt_TIMER3_OVF,		//	30
	gAvrInt_USART1_RX,		//	31
	gAvrInt_USART1_UDRE,	//	32
	gAvrInt_USART1_TX,		//	33
	gAvrInt_TWI,			//	34
	gAvrInt_SPM_READY,		//	35

};

#endif

//**************************************************************************************************
#if defined(__AVR_AT90CAN32__) || defined(__AVR_AT90CAN64__) || defined(__AVR_AT90CAN128__)
#pragma mark __AVR_AT90CAN32__ __AVR_AT90CAN64__ __AVR_AT90CAN128__

#define	_INTERRUPT_NAMES_DEFINED_

PGM_P gInterruptNameTable[]	PROGMEM =
{

	gAvrInt_RESET,				//	1
	gAvrInt_INT0,				//	2
	gAvrInt_INT1,				//	3
	gAvrInt_INT2,				//	4
	gAvrInt_INT3,				//	5
	gAvrInt_INT4,				//	6
	gAvrInt_INT5,				//	7
	gAvrInt_INT6,				//	8
	gAvrInt_INT7,				//	9
	gAvrInt_TIMER2_COMP,		//	10
	gAvrInt_TIMER2_OVF,			//	11
	gAvrInt_TIMER1_CAPT,		//	12
	gAvrInt_TIMER1_COMPA,		//	13
	gAvrInt_TIMER1_COMPB,		//	14
	gAvrInt_TIMER1_COMPC,		//	15
	gAvrInt_TIMER1_OVF,			//	16
	gAvrInt_TIMER0_COMP,		//	17
	gAvrInt_TIMER0_OVF,			//	18
	gAvrInt_CAN_TrafnsferCE,	//	19
	gAvrInt_CAN_TimerOverRun,	//	20
	gAvrInt_SPI_STC,			//	21
	gAvrInt_USART0_RX,			//	22
	gAvrInt_USART0_UDRE,		//	23
	gAvrInt_USART0_TX,			//	24
	gAvrInt_ANALOG_COMP,		//	25
	gAvrInt_ADC,				//	26
	gAvrInt_EE_READY,			//	27
	gAvrInt_TIMER3_CAPT,		//	28
	gAvrInt_TIMER3_COMPA,		//	29
	gAvrInt_TIMER3_COMPB,		//	30
	gAvrInt_TIMER3_COMPC,		//	31
	gAvrInt_TIMER3_OVF,			//	32
	gAvrInt_USART1_RX,			//	33
	gAvrInt_USART1_UDRE,		//	34
	gAvrInt_USART1_TX,			//	35
	gAvrInt_TWI,				//	36
	gAvrInt_SPM_READY,			//	37
};

#endif

//**************************************************************************************************
#if defined (__AVR_ATmega128RFA1__)
#pragma mark __AVR_ATmega128RFA1__
#define	_INTERRUPT_NAMES_DEFINED_

PGM_P gInterruptNameTable[]	PROGMEM =
{
	//*	Atmel changed the number scheme for interrupt vectors
	gAvrInt_RESET,			//	0
	gAvrInt_INT0,			//	1
	gAvrInt_INT1,			//	2
	gAvrInt_INT2,			//	3
	gAvrInt_INT3,			//	4
	gAvrInt_INT4,			//	5
	gAvrInt_INT5,			//	6
	gAvrInt_INT6,			//	7
	gAvrInt_INT7,			//	8
	gAvrInt_PCINT0,			//	9
	gAvrInt_PCINT1,			//	10
	gAvrInt_PCINT2,			//	11
	gAvrInt_WDT,			//	12
	gAvrInt_TIMER2_COMPA,	//	13
	gAvrInt_TIMER2_COMPB,	//	14
	gAvrInt_TIMER2_OVF,		//	15
	gAvrInt_TIMER1_CAPT,	//	16
	gAvrInt_TIMER1_COMPA,	//	17
	gAvrInt_TIMER1_COMPB,	//	18
	gAvrInt_TIMER1_COMPC,	//	19
	gAvrInt_TIMER1_OVF,		//	20
	gAvrInt_TIMER0_COMPA,	//	21
	gAvrInt_TIMER0_COMPB,	//	22
	gAvrInt_TIMER0_OVF,		//	23
	gAvrInt_SPI_STC,		//	24
	gAvrInt_USART0_RX,		//	25
	gAvrInt_USART0_UDRE,	//	26
	gAvrInt_USART0_TX,		//	27
	gAvrInt_ANALOG_COMP,	//	28
	gAvrInt_ADC,			//	29
	gAvrInt_EE_READY,		//	30
	gAvrInt_TIMER3_CAPT,	//	31
	gAvrInt_TIMER3_COMPA,	//	32
	gAvrInt_TIMER3_COMPB,	//	33
	gAvrInt_TIMER3_COMPC,	//	34
	gAvrInt_TIMER3_OVF,		//	35
	gAvrInt_USART1_RX,		//	36
	gAvrInt_USART1_UDRE,	//	37
	gAvrInt_USART1_TX,		//	38
	gAvrInt_TWI,			//	39
	gAvrInt_SPM_READY,		//	40
	gAvrInt_TIMER4_CAPT,	//	41
	gAvrInt_TIMER4_COMPA,	//	42
	gAvrInt_TIMER4_COMPB,	//	43
	gAvrInt_TIMER4_COMPC,	//	44
	gAvrInt_TIMER4_OVF,		//	45
	gAvrInt_TIMER5_CAPT,	//	46
	gAvrInt_TIMER5_COMPA,	//	47
	gAvrInt_TIMER5_COMPB,	//	48
	gAvrInt_TIMER5_COMPC,	//	49
	gAvrInt_TIMER5_OVF,		//	50
#if 1
	gAvrInt_RESERVED,		//	51
	gAvrInt_RESERVED,		//	52
	gAvrInt_RESERVED,		//	53

	gAvrInt_RESERVED,		//	54
	gAvrInt_RESERVED,		//	55
	gAvrInt_RESERVED,		//	56

#else
	gAvrInt_USART2_RX,		//	51
	gAvrInt_USART2_UDRE,	//	52
	gAvrInt_USART2_TX,		//	53

	gAvrInt_USART3_RX,		//	54
	gAvrInt_USART3_UDRE,	//	55
	gAvrInt_USART3_TX,		//	56
#endif
	gAvrInt_TRN_PLL_LOCK,	//	57
	gAvrInt_TRN_PLL_UNLOCK,	//	58	
	gAvrInt_TRN_RX_START,	//	59
	gAvrInt_TRN_RX_END,		//	60
	gAvrInt_TRN_CAAED_DONE,	//	61
	gAvrInt_TRN_FRAME_MATCH,//	62
	gAvrInt_TRN_TX_END,		//	63
	gAvrInt_TRN_AWAKE,		//	64
	
	gAvrInt_SCNT_CMP1,		//	65
	gAvrInt_SCNT_CMP2,		//	66
	gAvrInt_SCNT_CMP3,		//	67
	gAvrInt_SCNT_OVFL,		//	68
	gAvrInt_SCNT_BACKOFF,	//	69
	gAvrInt_AES_READY,		//	70
	gAvrInt_BAT_LOW,		//	71
	
	
};

#endif


#if !defined(_INTERRUPT_NAMES_DEFINED_)
	#warning	No interrupt string defs for this cpu
#endif

