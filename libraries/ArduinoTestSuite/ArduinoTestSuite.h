//************************************************************************
//************************************************************************
//*	Aug 31,	2010	<MLS> Started on TestArduino
//************************************************************************

#ifndef _AVR_IO_H_
	#include	<avr/io.h>
#endif

#ifndef WProgram_h
	#include	"WProgram.h"
#endif
#ifndef HardwareSerial_h
	#include	"HardwareSerial.h"
#endif


#if defined(USART3_RX_vect)
	#define	SERIAL_PORT_COUNT		4
#elif  defined(USART1_RX_vect)
	#define	SERIAL_PORT_COUNT		2
#else
	#define	SERIAL_PORT_COUNT		1
#endif


void	ATS_begin(char *manufName, char *testSuiteName);
void	ATS_end();

void	ATS_PrintTestStatus(char *testString, boolean passed);
boolean	ATS_Test_DigitalPin(uint8_t digitalPinToTest);
boolean	ATS_Test_PWM_Pin(uint8_t digitalPinToTest);
boolean	ATS_Test_AnalogInput(uint8_t analogPintoTest);
boolean	ATS_Test_EEPROM(void);

short	ATS_TestSerialLoopback(HardwareSerial *theSerialPort, char *serialPortName);


int		ATS_GetFreeMemory();

//************************************************************************
//*	this has to be an inline function because calling subroutines affects free memory
inline void ATS_ReportMemoryUsage(int _memoryUsageAtStart)
{
int		freeMemoryAtEnd;
int		lostMemory;
boolean	memoryOK;
char	memoryUsage[48];

	freeMemoryAtEnd	=	ATS_GetFreeMemory();
	lostMemory	=	_memoryUsageAtStart - freeMemoryAtEnd;
	if (lostMemory == 0)
	{
		strcpy(memoryUsage, "Memory Usage");
		memoryOK	=	true;
	}
	else
	{
		sprintf(memoryUsage, "Memory Usage (lost %d bytes)", lostMemory);
		memoryOK	=	false;
	}
	ATS_PrintTestStatus(memoryUsage, memoryOK);
}



extern	unsigned long	gTestStartTime;
extern	int				gYotalErrors;
extern	int				gTestCount;


#define	PASSED	true
#define	FAILED	false

