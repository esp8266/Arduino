//************************************************************************
//*	Arduino Test of Arduino Constants 
//*		(C) 2010 by Rick Anderson
//*		Open source as per standard Arduino code
//*
//************************************************************************
//*	Oct 16,	2010	<ROA>  Test of Arduino Constants
//************************************************************************

#include	"WProgram.h"
#include	"HardwareSerial.h"
#include	<ArduinoTestSuite.h>

//************************************************************************
void setup()
{
  int		startMemoryUsage;

 //Start memory usage must be site prior to ATS_begin
  startMemoryUsage	=	ATS_GetFreeMemory();
  ATS_begin("Arduino", "Test of Arduino Constants");
  /*
   * Test Run Start
   */


  //test true constant
  ATS_PrintTestStatus("1. Test of true constant", true == 1);

  //test false consts
  ATS_PrintTestStatus( "2. Test of false constant", false == 0);

  //Test of HIGH == 1
  ATS_PrintTestStatus( "3. Test of HIGH == 1", HIGH == 1);

  //Test of LOW ==  0
  ATS_PrintTestStatus( "4. Test of LOW ==  0", LOW == 0);

  //Test of INPUT == 1
  ATS_PrintTestStatus( "5. Test of INPUT == 1", HIGH == 1);

  //Test of OUTPUT ==  0
  ATS_PrintTestStatus( "6. Test of OUTPUT ==  0", LOW == 0);

  //test decimal
  ATS_PrintTestStatus( "7. Test of decimal constant", 101 == ((1 * pow(10,2)) + (0 * pow(10,1)) + 1));

  //test binary
  ATS_PrintTestStatus( "8. Test of binary constant", B101 == 5);

  //test octal
  ATS_PrintTestStatus( "9. Test of octal constant", 0101 == 65);

  //test hexadecimal
  ATS_PrintTestStatus( "7. Test of hexadecimal constant", (0x101 == 257));

  /*
  * Test Run End
   */
  ATS_ReportMemoryUsage(startMemoryUsage);
  ATS_end();

}


//************************************************************************
void loop()
{


}





