// Tests writing to and reading from a file, in particular the
// the Stream implementation (e.g. read() and peek()).

#include <SD.h>
#include <ArduinoTestSuite.h>

void setup()
{
  int startMemoryUsage = ATS_GetFreeMemory();
  boolean b;
  File f;
  
  ATS_begin("Arduino", "SD Test");
  
  ATS_PrintTestStatus("SD.begin()", b = SD.begin(4));
  if (!b) goto done;
  
  SD.remove("test.txt");
  
  f = SD.open("test.txt", FILE_WRITE);
  ATS_PrintTestStatus("SD.open()", f);
  if (!f) goto done;
  
  f.print("abc");
  f.print("de");
  f.close();
  
  f = SD.open("test.txt", FILE_WRITE);
  ATS_PrintTestStatus("SD.open()", f);
  if (!f) goto done;
  
  f.print("fgh");
  f.close();
  
  f = SD.open("test.txt");
  ATS_PrintTestStatus("SD.open()", f);
  if (!f) goto done;

  ATS_PrintTestStatus("read()", f.read() == 'a');
  ATS_PrintTestStatus("peek()", f.peek() == 'b');
  ATS_PrintTestStatus("read()", f.read() == 'b');
  ATS_PrintTestStatus("read()", f.read() == 'c');
  ATS_PrintTestStatus("peek()", f.peek() == 'd');
  ATS_PrintTestStatus("peek()", f.peek() == 'd');
  ATS_PrintTestStatus("peek()", f.peek() == 'd');
  ATS_PrintTestStatus("peek()", f.peek() == 'd');
  ATS_PrintTestStatus("read()", f.read() == 'd');
  ATS_PrintTestStatus("available()", f.available() != 0);
  ATS_PrintTestStatus("read()", f.read() == 'e');
  ATS_PrintTestStatus("available()", f.available() != 0);
  ATS_PrintTestStatus("peek()", f.peek() == 'f');
  ATS_PrintTestStatus("read()", f.read() == 'f');
  ATS_PrintTestStatus("peek()", f.peek() == 'g');
  ATS_PrintTestStatus("available()", f.available() != 0);
  ATS_PrintTestStatus("peek()", f.peek() == 'g');
  ATS_PrintTestStatus("read()", f.read() == 'g');
  ATS_PrintTestStatus("available()", f.available() != 0);
  ATS_PrintTestStatus("available()", f.available() != 0);
  ATS_PrintTestStatus("available()", f.available() != 0);
  ATS_PrintTestStatus("peek()", f.peek() == 'h');
  ATS_PrintTestStatus("read()", f.read() == 'h');
  ATS_PrintTestStatus("available()", f.available() == 0);
  ATS_PrintTestStatus("peek()", f.peek() == -1);
  ATS_PrintTestStatus("read()", f.read() == -1);
  ATS_PrintTestStatus("peek()", f.peek() == -1);
  ATS_PrintTestStatus("read()", f.read() == -1);
  
  f.close();  
  
  SD.remove("test2.txt");

  f = SD.open("test2.txt", FILE_WRITE);
  ATS_PrintTestStatus("SD.open()", f);
  if (!f) goto done;
  
  f.print("ABC");
  f.close();

  f = SD.open("test.txt");
  ATS_PrintTestStatus("SD.open()", f);
  if (!f) goto done;
  
  ATS_PrintTestStatus("peek()", f.peek() == 'a');
  
  f.close();
  
  f = SD.open("test2.txt");
  ATS_PrintTestStatus("SD.open()", f);
  if (!f) goto done;
  
  ATS_PrintTestStatus("peek()", f.peek() == 'A');
  ATS_PrintTestStatus("read()", f.read() == 'A');

  f.close();

done:
  ATS_ReportMemoryUsage(startMemoryUsage);
  ATS_end();

}

void loop() {}




