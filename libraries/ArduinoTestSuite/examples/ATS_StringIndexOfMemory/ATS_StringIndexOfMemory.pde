//************************************************************************
//*	Arduino Test Example Skeleton
//*		(C) 2010 by Rick Anderson
//*		Open source as per standard Arduino code
//*
//************************************************************************
//*	Oct 16,	2010	<ROA> Started on String Test
//************************************************************************

#include	<ArduinoTestSuite.h>

//************************************************************************

void do_string_operations(int startMemoryUsage)
{
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
  int secondLastParagraph;
  int thirdLastParagraph;
  
            //           1111111111
            // 01234567890123456789
  stringOne = "<HTML><HEAD><BODY>";
  firstClosingBracket = stringOne.indexOf('>');
  ATS_PrintTestStatus("firstClosingBracket", firstClosingBracket == 5);

            //           1111111111
            // 01234567890123456789
  stringOne = "<HTML><HEAD><BODY>";
  secondOpeningBracket = firstClosingBracket + 1;
  secondClosingBracket = stringOne.indexOf('>', secondOpeningBracket );
  ATS_PrintTestStatus("secondClosingBracket", secondClosingBracket == 11);

  // you can also use indexOf() to search for Strings:
            //           1111111111
            // 01234567890123456789
  stringOne = "<HTML><HEAD><BODY>";
  bodyTag = stringOne.indexOf("<BODY>");
  ATS_PrintTestStatus("bodyTag", bodyTag == 12);

            //           111111111122222222223333333333
            // 0123456789012345678901234567890123456789
  stringOne = "<UL><LI>item<LI>item<LI>item</UL>";
  firstListItem = stringOne.indexOf("<LI>");
  secondListItem = stringOne.indexOf("<LI>", firstListItem + 1 );
  
  ATS_PrintTestStatus("firstListItem", firstListItem == 4);
  ATS_PrintTestStatus("secondListItem", secondListItem == 12);

  // lastIndexOf() gives you the last occurrence of a character or string:
  lastOpeningBracket = stringOne.lastIndexOf('<');
  ATS_PrintTestStatus("lastOpeningBracket", lastOpeningBracket == 28);
  
  lastListItem  = stringOne.lastIndexOf("<LI>");
  ATS_PrintTestStatus("lastListItem", lastListItem == 20);

  // lastIndexOf() can also search for a string:
            //           11111111112222222222333333333344444444445555555555
            // 012345678901234567890123456789012345678901234567890123456789
  stringOne = "<p>Lorem ipsum dolor sit amet</p><p>Ipsem</p><p>Quod</p>";
  lastParagraph = stringOne.lastIndexOf("<p");
  secondLastParagraph = stringOne.lastIndexOf("<p", lastParagraph - 1);
  thirdLastParagraph = stringOne.lastIndexOf("<p", secondLastParagraph - 1);
  ATS_PrintTestStatus("lastParagraph", lastParagraph == 45);
  ATS_PrintTestStatus("secondLastParagraph", secondLastParagraph == 33);
  ATS_PrintTestStatus("thirdLastParagraph", thirdLastParagraph == 0);
}

void setup()
{
  int startMemoryUsage=0;

  startMemoryUsage = ATS_GetFreeMemory();
  ATS_begin("Arduino", "String Memory Test");
  ATS_ReportMemoryUsage(startMemoryUsage);
  
  // Run all the string functions.  All string objects used are local variables,
  // so they go out of scope upon return.  Memory used should be fully released.
  do_string_operations(startMemoryUsage);
  
  ATS_ReportMemoryUsage(startMemoryUsage);
  ATS_end();
}


//************************************************************************
void loop()
{
}






