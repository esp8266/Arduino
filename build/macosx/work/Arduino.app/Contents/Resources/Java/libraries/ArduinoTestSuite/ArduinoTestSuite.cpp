//************************************************************************
//*	Arduino Test Suite
//*		(C) 2010 by Mark Sproul
//*		Open source as per standard Arduino code
//*	
//*	  This library is free software; you can redistribute it and/or
//*	  modify it under the terms of the GNU Lesser General Public
//*	  License as published by the Free Software Foundation; either
//*	  version 2.1 of the License, or (at your option) any later version.
//*	
//*	  This library is distributed in the hope that it will be useful,
//*	  but WITHOUT ANY WARRANTY; without even the implied warranty of
//*	  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//*	  Lesser General Public License for more details.
//************************************************************************
//*	Aug 31,	2010	<MLS> Started on TestArduino
//*	Oct 18,	2010	<MLS> Added memory testing
//************************************************************************

#include	<avr/pgmspace.h>
#include	<avr/io.h>
#include	<avr/eeprom.h>



#include	"ArduinoTestSuite.h"


#include	"WProgram.h"
#include	"HardwareSerial.h"
#include	"pins_arduino.h"


#include	"avr_cpunames.h"

#if defined(USART3_RX_vect)
	#define	SERIAL_PORT_COUNT		4
#elif  defined(USART1_RX_vect)
	#define	SERIAL_PORT_COUNT		2
#else
	#define	SERIAL_PORT_COUNT		1
#endif




//************************************************************************
enum 
{
	ATS_Manufacturer	=	1,
	ATS_CPU,
	ATS_GCC_version,
	ATS_LIBC_version,
	ATS_CompiledDate,
	ATS_TestSuiteName,
	ATS_FreeMemory,
	
	
};
unsigned long	gTestStartTime;
short			gTagIndent;
int				gYotalErrors;
int				gTestCount;



prog_char	gTextMsg_Manufacturer[]			PROGMEM	=	"MANUFACTURER";
prog_char	gTextMsg_CPUname[]				PROGMEM	=	"CPU-NAME";
prog_char	gTextMsg_GCC_VERSION[]			PROGMEM	=	"GCC-Version";
prog_char	gTextMsg_AVR_LIBC[]				PROGMEM	=	"AVR-LibC-Ver";
prog_char	gTextMsg_COMPILED_DATE[]		PROGMEM	=	"Compiled-date";
prog_char	gTextMsg_TEST_SUITE_NAME[]		PROGMEM	=	"Test-Suite-Name";
prog_char	gTextMsg_memoryUsage[]			PROGMEM	=	"Free-memory";
prog_char	gTextMsg_dotdotdot[]			PROGMEM	=	"... ";
prog_char	gTextMsg_ok[]					PROGMEM	=	"ok";
prog_char	gTextMsg_FAIL[]					PROGMEM	=	"FAIL";
prog_char	gTextMsg_spaceEqual[]			PROGMEM	=	" = ";
prog_char	gTextMsg_info[]					PROGMEM	=	"info.";
prog_char	gTextMsg_dashLine[]				PROGMEM	=	"--------------------------";
prog_char	gTextMsg_DigitalRW[]			PROGMEM	=	"DigitalReadWrite_";
prog_char	gTextMsg_PWMoutput[]			PROGMEM	=	"PWMoutput_";
prog_char	gTextMsg_AnalogInput[]			PROGMEM	=	"AnalogInput_";

//************************************************************************
void Serial_print_P(prog_char *flashMemStr)
{
char	theChar;
int		ii;

	ii		=	0;
#if (FLASHEND > 0x10000)
	while (theChar	=	pgm_read_byte_far(flashMemStr + ii++))
#else
	while (theChar	=	pgm_read_byte_near(flashMemStr + ii++))
#endif
	{
		Serial.print(theChar);
	}
}

//************************************************************************
void Serial_println_P(prog_char *flashMemStr)
{
	Serial_print_P(flashMemStr);
	Serial.println();
}

//************************************************************************
//*	this is for internal use only, not made pubic to the API
static void	ATS_PrintProperty(	int		propertyTagNum,
								char	*propertyName,
								char	*propertyValue)
{
char	lineBuffer[64];

	strcpy_P(lineBuffer, gTextMsg_info);
	switch(propertyTagNum)
	{
		case 0:
			strcat(lineBuffer, propertyName);
			break;
			
		case ATS_Manufacturer:
			strcat_P(lineBuffer, gTextMsg_Manufacturer);
			break;

		case ATS_CPU:
			strcat_P(lineBuffer, gTextMsg_CPUname);
			break;

		case ATS_GCC_version:
			strcat_P(lineBuffer, gTextMsg_GCC_VERSION);
			break;

		case ATS_LIBC_version:
			strcat_P(lineBuffer, gTextMsg_AVR_LIBC);
			break;

		case ATS_CompiledDate:
			strcat_P(lineBuffer, gTextMsg_COMPILED_DATE);
			break;

		case ATS_TestSuiteName:
			strcat_P(lineBuffer, gTextMsg_TEST_SUITE_NAME);
			break;

		case ATS_FreeMemory:
			strcat_P(lineBuffer, gTextMsg_memoryUsage);
			break;
	}

	while (strlen(lineBuffer) < 20)
	{
		strcat(lineBuffer, " ");
	}
	
	strcat_P(lineBuffer, gTextMsg_spaceEqual);
	if (propertyValue != 0)
	{
		strcat(lineBuffer, propertyValue);
	}
	Serial.println(lineBuffer);

}




//************************************************************************
void	ATS_begin(char *manufName, char *testSuiteName)
{
int		freeMemory;
char	memoryMsg[48];

	gYotalErrors	=	0;
	gTestCount		=	0;

	Serial.begin(9600);
	delay(1000);
	
	gTestStartTime	=	millis();

	Serial.println();
	Serial.println();
	Serial.println();

	ATS_PrintProperty(ATS_Manufacturer,		0,	manufName);
	ATS_PrintProperty(ATS_CPU,				0,	_AVR_CPU_NAME_);
	ATS_PrintProperty(ATS_GCC_version,		0,	__VERSION__);
	ATS_PrintProperty(ATS_LIBC_version,		0,	__AVR_LIBC_VERSION_STRING__);
	ATS_PrintProperty(ATS_CompiledDate,		0,	__DATE__);
	ATS_PrintProperty(ATS_TestSuiteName,	0,	testSuiteName);

	freeMemory	=	ATS_GetFreeMemory();
	sprintf(memoryMsg, "%d bytes", freeMemory);
	ATS_PrintProperty(ATS_FreeMemory,	0,	memoryMsg);

	randomSeed(analogRead(0));

}

//************************************************************************
void	ATS_end()
{
long	seconds;
long	milliSecs;


	Serial_println_P(gTextMsg_dashLine);
	
	//	Ran 4 tests in 0.000s
	Serial.print("Ran ");
	Serial.print(gTestCount);
	Serial.print(" tests in ");
	
	seconds		=	millis() / 1000;
	milliSecs	=	millis() % 1000;
	Serial.print(seconds);
	Serial.print('.');
	Serial.print(milliSecs);
	Serial.print('s');
	Serial.println();
	Serial.println();

	if (gYotalErrors == 0)
	{
		Serial.print("OK");
	}
	else
	{
		Serial.print("FAILED (failures=");
		Serial.print(gYotalErrors);
		Serial.print(")");
	}
	Serial.println();
	
	//*	send control D to terminate (End Of File)
	Serial.write(0x04);
}



//************************************************************************
void ATS_PrintTestStatus(char *testString, boolean passed)
{
int	sLen;

	Serial.print(testString);
	sLen	=	strlen(testString);
	while (sLen < 60)
	{
		Serial.print(' ');
		sLen++;
	}
	Serial_print_P(gTextMsg_dotdotdot);
	if (passed)
	{
		Serial_print_P(gTextMsg_ok);
	}
	else
	{
		Serial_print_P(gTextMsg_FAIL);
		gYotalErrors++;
	}
	Serial.println();
	
	gTestCount++;
}



//************************************************************************
//*	returns true if no errors, false if there is an error
int	ATS_Test_DigitalPinWithHelper(uint8_t digitalPinToTest, uint8_t helperpin)
{
boolean	passedOK;
int		pinValue;
char	testName[64];
char	numString[32];

	strcpy_P(testName, gTextMsg_DigitalRW);
	sprintf(numString, "%02d", digitalPinToTest);
	strcat(testName, numString);

	passedOK	=	true;
	
	//*	test senario 1
	pinMode(digitalPinToTest, OUTPUT);
	pinMode(helperpin, INPUT);

	digitalWrite(digitalPinToTest, HIGH);
	pinValue	=	digitalRead(helperpin);
	if (pinValue != HIGH)
	{
		passedOK	=	false;
	}

	digitalWrite(digitalPinToTest, LOW);
	pinValue	=	digitalRead(helperpin);
	if (pinValue != LOW)
	{
		passedOK	=	false;
	}

	
	//*	now reverse the input/output
	pinMode(digitalPinToTest, INPUT);
	pinMode(helperpin, OUTPUT);
	
	digitalWrite(helperpin, HIGH);
	pinValue	=	digitalRead(digitalPinToTest);
	if (pinValue != HIGH)
	{
		passedOK	=	false;
	}

	digitalWrite(helperpin, LOW);
	pinValue	=	digitalRead(digitalPinToTest);
	if (pinValue != LOW)
	{
		passedOK	=	false;
	}


	if (! passedOK)
	{
		sprintf(numString, " (helper pin=%02d)", helperpin);
		strcat(testName, numString);
	}
	ATS_PrintTestStatus(testName, passedOK);
	return(passedOK);
}

//************************************************************************
boolean	ATS_Test_DigitalPin(uint8_t digitalPinToTest)
{
boolean	passedOK;
uint8_t helperpin;

	if ((digitalPinToTest % 2) == 0)
	{
		//*	if its EVEN, add 1
		helperpin	=	digitalPinToTest + 1;
	}
	else
	{
		//*	if its ODD
		helperpin	=	digitalPinToTest - 1;
	}
	passedOK	=	ATS_Test_DigitalPinWithHelper(digitalPinToTest, helperpin);
	return(passedOK);
}



//************************************************************************
//*	returns true if no errors, false if there is an error
int	ATS_TestTimer(	uint8_t	timerPinNumber,
					uint8_t	inputPin,
					char *statusString,
					char *errorString)
{
boolean			passedOK;
unsigned long	loopCounter;
unsigned long	lowCount;
unsigned long	highCount;
unsigned long	startTime;
int				percentLow;
int				percentHigh;
int				pinValue;
char			numString[48];
int				pwmValue;

	pwmValue	=	128;
	loopCounter	=	0;
	lowCount	=	0;
	highCount	=	0;
	passedOK	=	true;
	
	startTime	=	millis();
	pinMode(inputPin, INPUT);
	analogWrite(timerPinNumber, pwmValue);
	while ((millis() - startTime) < 500)
	{
		pinValue	=	digitalRead(inputPin);
		if (pinValue == HIGH)
		{
			highCount++;
		}
		else
		{
			lowCount++;
		}
	}
	analogWrite(timerPinNumber, 0);

	//*	the difference should be about 50%
	percentLow	=	lowCount / ((lowCount + highCount) / 100);
	percentHigh	=	highCount / ((lowCount + highCount) / 100);
	if ((percentLow > 45) && (percentLow < 55))
	{
		passedOK	=	true;
	}
	else
	{
		passedOK	=	false;
		strcat(errorString, " PWM ERROR");
	}
	sprintf(numString, "  (PWM=%02d %d%% LOW %d%% HIGH)", pwmValue, percentLow, percentHigh);
	strcat(statusString, numString);

	return(passedOK);
}


//************************************************************************
//*	returns true if no errors, false if there is an error
boolean	ATS_Test_PWMPinWithHelper(uint8_t pwmPinToTest, uint8_t helperpin)
{
boolean	passedOK;
char	testName[64];
char	errorString[48];
char	numString[8];
uint8_t	timerNumber;



	strcpy_P(testName, gTextMsg_PWMoutput);
	sprintf(numString, "%02d", pwmPinToTest);
	strcat(testName, numString);

	passedOK		=	true;
	errorString[0]	=	0;
	

	//*	is pin1 a timer?
	timerNumber	=	digitalPinToTimer(pwmPinToTest);
	if (timerNumber != NOT_ON_TIMER)
	{
		passedOK	=	ATS_TestTimer(pwmPinToTest, helperpin, testName, errorString);
	}
	else
	{
		//*	we should not get here
		passedOK	=	false;
	}

	ATS_PrintTestStatus(testName, passedOK);

	
	return(passedOK);
}

//************************************************************************
boolean	ATS_Test_PWM_Pin(uint8_t pwmPinToTest)
{
boolean	passedOK;
uint8_t helperpin;

	if ((pwmPinToTest % 2) == 0)
	{
		//*	if its EVEN, add 1
		helperpin	=	pwmPinToTest + 1;
	}
	else
	{
		//*	if its ODD
		helperpin	=	pwmPinToTest - 1;
	}
	passedOK	=	ATS_Test_PWMPinWithHelper(pwmPinToTest, helperpin);
	return(passedOK);
}


#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
	#define	kAnalogPinOffset	54
#else
	#define	kAnalogPinOffset	14
#endif


//************************************************************************
boolean	ATS_Test_AnalogInputWithHelper(uint8_t analogPintoTest, uint8_t helperPin)
{
boolean	passedOK;
char	testName[64];
char	infoString[48];
int		analogValueHigh;
int		analogValueLow;


	//*	first we have to set the ANALOG pin to INPUT
	pinMode(analogPintoTest + kAnalogPinOffset, INPUT);
	
	passedOK	=	true;
	
	strcpy_P(testName, gTextMsg_AnalogInput);
	sprintf(infoString, "%02d", analogPintoTest);
	strcat(testName, infoString);


	pinMode(helperPin, OUTPUT);

	digitalWrite(helperPin, LOW);
	analogValueLow	=	analogRead(analogPintoTest);
	if (analogValueLow > 100)
	{
		passedOK	=	false;
	}


	digitalWrite(helperPin, HIGH);
	analogValueHigh	=	analogRead(analogPintoTest);
	if (analogValueHigh < 1000)
	{
		passedOK	=	false;
	}


	sprintf(infoString, " (Low=%4d High=%4d helper pin=%d)", analogValueLow, analogValueHigh, helperPin);
	strcat(testName, infoString);
	
	ATS_PrintTestStatus(testName, passedOK);

	return(passedOK);
}


//************************************************************************
boolean	ATS_Test_AnalogInput(uint8_t analogPinToTest)
{
boolean	passedOK;
uint8_t helperpin;

	if ((analogPinToTest % 2) == 0)
	{
		//*	if its EVEN, add 1
		helperpin	=	kAnalogPinOffset + analogPinToTest + 1;
	}
	else
	{
		//*	if its ODD
		helperpin	=	kAnalogPinOffset + analogPinToTest - 1;
	}
	passedOK	=	ATS_Test_AnalogInputWithHelper(analogPinToTest, helperpin);
	return(passedOK);
}


#define	kSerialTestBaudRate	9600
#define	kSerialTestDelay	3


#if (SERIAL_PORT_COUNT > 1) && !defined(__AVR_ATmega32U4__)
//************************************************************************
//*	retunrs 0 if no errors, 1 if an error occured
short	ATS_TestSerialLoopback(HardwareSerial *theSerialPort, char *serialPortName)
{
char	xmitChar;
char	rcvChar;
short	ii;
short	serialErrCt;
short	timeOutLoopCtr;


	serialErrCt	=	1;
	if (theSerialPort != 0)
	{
		serialErrCt	=	0;
		theSerialPort->begin(kSerialTestBaudRate);
		
		for (ii=0; ii<150; ii++)
		{
			xmitChar	=	ii;
			theSerialPort->print(xmitChar);
			
			timeOutLoopCtr	=	0;
			//*	wait for data to come back or timeout
			while (!theSerialPort->available() && (timeOutLoopCtr < kSerialTestDelay))
			{
				delay(1);
				timeOutLoopCtr++;
			}
			
			if (theSerialPort->available())
			{
				//*	get the char
				rcvChar	=	theSerialPort->read();
				if (rcvChar != xmitChar)
				{
					serialErrCt	=	1;
				}
			}
			else
			{
				serialErrCt	=	1;
			}
		}
		theSerialPort->end();

		if (serialErrCt == 0)
		{
			ATS_PrintTestStatus(serialPortName, PASSED);
		}
		else
		{
			ATS_PrintTestStatus(serialPortName, FAILED);
		}
	}
	
	return(serialErrCt);
}
#endif


//************************************************************************
boolean	ATS_Test_EEPROM(void)
{
boolean		passedOK;
uint8_t 	dataByte;
uint8_t		dataByteRead;
uint16_t	dataWord;
uint16_t	dataWordRead;
uint32_t	dataLongWord;
uint32_t	dataLongWordRead;
int			addressPtr;
char		reportString[48];

	passedOK		=	true;
	//*	test BYTE read/write
	addressPtr		=	random(E2END);
	dataByte		=	0x5A;
	eeprom_write_byte((uint8_t *)addressPtr, dataByte);
	dataByteRead	=	eeprom_read_byte((uint8_t *)addressPtr);

	sprintf(reportString, "EEPROM_byte_rw  (addr= 0x%04X)", addressPtr);
	if (dataByteRead == dataByte)
	{
		ATS_PrintTestStatus(reportString, PASSED);
	}
	else
	{
		ATS_PrintTestStatus(reportString, FAILED);
		passedOK		=	false;
	}


	//*	test WORD read/write
	addressPtr		=	random(E2END);
	dataWord		=	0xA55A;
	eeprom_write_word((uint16_t *)addressPtr, dataWord);
	dataWordRead	=	eeprom_read_word((uint16_t *)addressPtr);

	sprintf(reportString, "EEPROM_word_rw  (addr= 0x%04X)", addressPtr);
	if (dataWordRead == dataWord)
	{
		ATS_PrintTestStatus(reportString, PASSED);
	}
	else
	{
		ATS_PrintTestStatus(reportString, FAILED);
		passedOK		=	false;
	}


	//*	test Long WORD read/write
	addressPtr		=	random(E2END);
	dataLongWord	=	0x5AA5A55A;
	eeprom_write_dword((uint32_t *)addressPtr, dataLongWord);
	dataLongWordRead	=	eeprom_read_dword((uint32_t *)addressPtr);

	sprintf(reportString, "EEPROM_dword_rw (addr= 0x%04X)", addressPtr);
	if (dataLongWordRead == dataLongWord)
	{
		ATS_PrintTestStatus(reportString, PASSED);
	}
	else
	{
		ATS_PrintTestStatus(reportString, FAILED);
		passedOK		=	false;
	}


	return(passedOK);
}



//************************************************************************
extern unsigned int __data_start;
extern unsigned int __data_end;
extern unsigned int __bss_start;
extern unsigned int __bss_end;
extern unsigned int __heap_start;
extern void *__brkval;



//************************************************************************
int	ATS_GetFreeMemory()
{
int free_memory;

	if((int)__brkval == 0)
	{
		free_memory = ((int)&free_memory) - ((int)&__bss_end);
	}
	else
	{
		free_memory = ((int)&free_memory) - ((int)__brkval);
	}
	return free_memory;
}


