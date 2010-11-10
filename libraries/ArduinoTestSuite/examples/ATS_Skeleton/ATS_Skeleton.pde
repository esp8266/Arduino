//************************************************************************
//*	Arduino Test Example Skeleton
//*		(C) 2010 by Rick Anderson
//*		Open source as per standard Arduino code
//*
//************************************************************************
//*	Oct 16,	2010	<ROA> Started on String Test
//************************************************************************

#include	"WProgram.h"
#include	"HardwareSerial.h"
#include	<ArduinoTestSuite.h>

//************************************************************************
void setup()
{
  int		startMemoryUsage;
  
  //startMemoryUsage must be set directly before ATS_begin
  startMemoryUsage	=	ATS_GetFreeMemory();
  ATS_begin("Arduino", "Skeleton Test");
  /*
   * Test Run Start
   * Test one passes because result is set to true
   * Test two fails becuase result is set to false
   * You can test memory for any set of tests by using the ATS_ReportMemoryUsage test
   * There is also a way to print current memeory for debugging
   */
  ATS_PrintTestStatus("1. Test of true test status", true);
  
  ATS_PrintTestStatus("2. Test of false test status, this will fail.", false);

  ATS_ReportMemoryUsage(startMemoryUsage);
  /*
   * Test Run End
   */

  ATS_end();

}


//************************************************************************
void loop()
{


}




