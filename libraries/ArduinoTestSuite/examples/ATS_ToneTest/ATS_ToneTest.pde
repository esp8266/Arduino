//************************************************************************
//*	Arduino Test Suite
//*	ATS_ToneTest
//*	
//*	Copyright (c) 2010 Mark Sproul  All right reserved.
//*	 
//*	This library is free software; you can redistribute it and/or
//*	modify it under the terms of the GNU Lesser General Public
//*	License as published by the Free Software Foundation; either
//*	version 2.1 of the License, or (at your option) any later version.
//*	
//*	This library is distributed in the hope that it will be useful,
//*	but WITHOUT ANY WARRANTY; without even the implied warranty of
//*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//*	Lesser General Public License for more details.
//*	
//*	You should have received a copy of the GNU Lesser General Public
//*	License along with this library; if not, write to the Free Software
//*	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//************************************************************************
//*	Aug 31,	2010	<MLS> Started on TestArduino
//*	Oct 23,	2010	<MLS> Started on ToneTest
//************************************************************************





#include	"WProgram.h"
#include	"HardwareSerial.h"

#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__)
	#define	kBoard_PinCount		20
	#define	kBoard_AnalogCount	6
#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
	#define	kBoard_PinCount		70
	#define	kBoard_AnalogCount	16
#endif

#include	<ArduinoTestSuite.h>

//************************************************************************
void	TestTonePin(uint8_t toneOutputPinNumber)
{
uint8_t 		helperpin;
unsigned long	startMilliSecs;
unsigned long	highCount, lowCount;
int				previousState;
int				currentState;
char			testNameString[80];
long			outputFreq;
long			measuredFreq;
boolean			passed;
long			percentError;
long			deltaFreq;

	if ((toneOutputPinNumber % 2) == 0)
	{
		//*	if its EVEN, add 1
		helperpin	=	toneOutputPinNumber + 1;
	}
	else
	{
		//*	if its ODD
		helperpin	=	toneOutputPinNumber - 1;
	}

	//*	dont set the mode of the OUTPUT pin, the tone command does that
	
	pinMode(helperpin, INPUT);
	
	previousState	=	digitalRead(helperpin);
	startMilliSecs	=	millis();
	highCount		=	0;
	lowCount		=	0;
	measuredFreq	=	0;
	//*	we are going to watch for one second
	outputFreq	=	random(200, 2000);
	
	tone(toneOutputPinNumber, outputFreq);
	while ((millis() - startMilliSecs) < 1000)
	{
		currentState	=	digitalRead(helperpin);
		if (currentState == HIGH)
		{
			highCount++;
		}
		else
		{
			lowCount++;
		}
		//*	check to see if it changed state
		if ((currentState == HIGH) && (previousState == LOW))
		{
			measuredFreq++;
		}
		
		previousState	=	currentState;
	}
	noTone(toneOutputPinNumber);
	
	deltaFreq		=	abs(measuredFreq - outputFreq);
	
	percentError	=	100 - abs(((outputFreq - deltaFreq) * 100) / outputFreq);
	
	sprintf(testNameString, "ToneTest.%02d (out freq= %4ld measured freq= %4ld err= %ld%%)", toneOutputPinNumber, outputFreq, measuredFreq, percentError);
	if (percentError < 5)
	{
		passed	=	true;
	}
	else
	{
		passed	=	false;
	}

	ATS_PrintTestStatus(testNameString, passed);
}


//************************************************************************
//*	this test to make sure the duration option works
void	TestToneDuration(uint8_t toneOutputPinNumber)
{
uint8_t 		helperpin;
unsigned long	startMilliSecs;
unsigned long	highCount, lowCount;
int				previousState;
int				currentState;
char			testNameString[80];
long			outputFreq;
long			measuredFreq;
boolean			passed;
long			percentError;
long			deltaFreq;
long			durationTime;

	if ((toneOutputPinNumber % 2) == 0)
	{
		//*	if its EVEN, add 1
		helperpin	=	toneOutputPinNumber + 1;
	}
	else
	{
		//*	if its ODD
		helperpin	=	toneOutputPinNumber - 1;
	}

	//*	dont set the mode of the OUTPUT pin, the tone command does that
	
	pinMode(helperpin, INPUT);
	
	previousState	=	digitalRead(helperpin);
	startMilliSecs	=	millis();
	highCount		=	0;
	lowCount		=	0;
	measuredFreq	=	0;
	durationTime	=	0;
	//*	we are going to watch for one second
	outputFreq	=	random(500, 2000);
	
	tone(toneOutputPinNumber, outputFreq, 1000);
	while ((millis() - startMilliSecs) < 2000)
	{
		currentState	=	digitalRead(helperpin);
		if (currentState == HIGH)
		{
			highCount++;
		}
		else
		{
			lowCount++;
		}
		//*	count the freq
		if ((currentState == HIGH) && (previousState == LOW))
		{
			measuredFreq++;
		}

		//*	check to see if it changed state
		if (currentState != previousState)
		{
			durationTime	=	millis() - startMilliSecs;
		}
		
		previousState	=	currentState;
	}
	
	deltaFreq		=	abs(measuredFreq - outputFreq);
	
	percentError	=	100 - abs(((outputFreq - deltaFreq) * 100) / outputFreq);
	
	sprintf(testNameString, "ToneTesDurationt.%02d (durationTime =%4ld/1000 freq err= %ld%%)", toneOutputPinNumber, durationTime, percentError);
	if ((durationTime > 990) && (durationTime < 1010) &&  (percentError < 5))
	{
		passed	=	true;
	}
	else
	{
		passed	=	false;
	}
	noTone(toneOutputPinNumber);

	ATS_PrintTestStatus(testNameString, passed);
}



//************************************************************************
void setup()
{
short	ii;
uint8_t	timerNumber;
int		startMemoryUsage;

	startMemoryUsage	=	ATS_GetFreeMemory();

	ATS_begin("Arduino", "ToneTest");
	

	//*	we start at 2 because 0/1 are RXD/TXD
	for (ii=2; ii<kBoard_PinCount; ii++)
	{
		TestTonePin(ii);
	}


	//*	we dont need to test every pin	
	for (ii=2; ii<kBoard_PinCount; ii += 5)
	{
		TestToneDuration(ii);
	}


	ATS_ReportMemoryUsage(startMemoryUsage);

	ATS_end();

}


//************************************************************************
void loop()
{


}




