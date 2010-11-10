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
  char testName[64];
  int		startMemoryUsage;
  /*
    * Create variable for the tests. 
   */


  String stringOne;
  int firstClosingBracket;
  int firstOpeningBracket;
  int secondOpeningBracket;
  int secondClosingBracket;
  int bodyTag;
  int firstListItem;
  int secondListItem;
  int lastOpeningBracket;
  int lastListItem;
  int lastParagraph;
  int secondLastGraf;

  /*;
   * initiate the test run
   */
  startMemoryUsage	=	ATS_GetFreeMemory();
  ATS_begin("Arduino", "String Memory Test");
  // indexOf() returns the position (i.e. index) of a particular character
  // in a string. For example, if you were parsing HTML tags, you could use it:
  stringOne = "<HTML><HEAD><BODY>";
  firstClosingBracket = stringOne.indexOf('>');
  Serial.println("The index of > in the string " + stringOne + " is " + firstClosingBracket);

  stringOne = "<HTML><HEAD><BODY>";
  secondOpeningBracket = firstClosingBracket + 1;
  secondClosingBracket = stringOne.indexOf('>', secondOpeningBracket );
  Serial.println("The index of  the second > in the string " + stringOne + " is " + secondClosingBracket);

  // you can also use indexOf() to search for Strings:
  stringOne = "<HTML><HEAD><BODY>";
  bodyTag = stringOne.indexOf("<BODY>");
  Serial.println("The index of the body tag in the string " + stringOne + " is " + bodyTag);

  stringOne = "<UL><LI>item<LI>item<LI>item</UL>";
  firstListItem = stringOne.indexOf("<LI>");
  secondListItem = stringOne.indexOf("item", firstListItem + 1 );
  Serial.println("The index of the second list item in the string " + stringOne + " is " + secondClosingBracket);

  // lastIndexOf() gives you the last occurrence of a character or string:
  lastOpeningBracket = stringOne.lastIndexOf('<');
  Serial.println("The index of the last < in the string " + stringOne + " is " + lastOpeningBracket);

  lastListItem  = stringOne.lastIndexOf("<LI>");
  Serial.println("The index of the last list item in the string " + stringOne + " is " + lastListItem);


  // lastIndexOf() can also search for a string:
  stringOne = "<p>Lorem ipsum dolor sit amet</p><p>Ipsem</p><p>Quod</p>";
  lastParagraph = stringOne.lastIndexOf("<p");
  secondLastGraf = stringOne.lastIndexOf("<p", lastParagraph - 1);
  Serial.println("The index of the second last paragraph tag " + stringOne + " is " + secondLastGraf);


  ATS_ReportMemoryUsage(startMemoryUsage);
  


  /*
   * Test complete
   */

  ATS_end();

}


//************************************************************************
void loop()
{


}






