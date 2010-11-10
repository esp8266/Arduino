//************************************************************************
//*	Arduino Test Suite
//*		(C) 2010 by Mark Sproul
//*		Open source as per standard Arduino code
//*
//************************************************************************
//*	Aug 31,	2010	<MLS> Started on TestArduino
//*	Oct 18,	2010	<MLS> Added memory testing
//************************************************************************

#include	"WProgram.h"
#include	"HardwareSerial.h"
#include	"pins_arduino.h"
#include	<ArduinoTestSuite.h>
#include	"avr_cpunames.h"


#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__)
#define	kBoard_PinCount		20
#define	kBoard_AnalogCount	6
#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#define	kBoard_PinCount		70
#define	kBoard_AnalogCount	16
#endif




//************************************************************************
void setup()
{	
  short ii;
  uint8_t        timerNumber;
  int		startMemoryUsage;

  startMemoryUsage	=	ATS_GetFreeMemory();

  ATS_begin("Arduino", "general");

  //*	test digital pins
  //*	we start at 2 because 0/1 are RXD/TXD
  for (ii=2; ii<kBoard_PinCount; ii++)
  {
    ATS_Test_DigitalPin(ii);
  }


  //*	test PWM pins
  //*	we start at 2 because 0/1 are RXD/TXD
  for (ii=2; ii<kBoard_PinCount; ii++)
  {
    timerNumber	=	digitalPinToTimer(ii);
    if (timerNumber != NOT_ON_TIMER)
    {
      ATS_Test_PWM_Pin(ii);
    }
  }

  for (ii=0; ii<kBoard_AnalogCount; ii++)
  {
    ATS_Test_AnalogInput(ii);
  }

#if (SERIAL_PORT_COUNT > 1)
  ATS_TestSerialLoopback(&Serial1, "Serial1");
#endif
#if (SERIAL_PORT_COUNT > 2)
  ATS_TestSerialLoopback(&Serial2, "Serial2");
#endif
#if (SERIAL_PORT_COUNT > 3)
  ATS_TestSerialLoopback(&Serial3, "Serial3");
#endif

  ATS_Test_EEPROM();


  ATS_ReportMemoryUsage(startMemoryUsage);

  ATS_end();

}


//************************************************************************
void loop()
{


}





