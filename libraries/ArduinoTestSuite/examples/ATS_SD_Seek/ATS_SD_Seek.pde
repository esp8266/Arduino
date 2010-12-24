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
  
  ATS_PrintTestStatus("initial position", f.position() == 0);
  ATS_PrintTestStatus("initial size", f.size() == 0);

  f.print("0123456789");
  
  ATS_PrintTestStatus("position after writing", f.position() == 10);
  ATS_PrintTestStatus("size after writing", f.size() == 10);
  
  f.seek(0);

  ATS_PrintTestStatus("size after seek", f.size() == 10);
  ATS_PrintTestStatus("position after seek", f.position() == 0);
  
  f.seek(7);

  ATS_PrintTestStatus("position after seek", f.position() == 7);
  ATS_PrintTestStatus("reading after seek", f.read() == '7');
  ATS_PrintTestStatus("position after reading after seeking", f.position() == 8);
  ATS_PrintTestStatus("reading after reading after seeking", f.read() == '8');

  f.seek(3);

  ATS_PrintTestStatus("position after seeking", f.position() == 3);
  ATS_PrintTestStatus("peeking after seeking", f.peek() == '3');
  ATS_PrintTestStatus("position after peeking after seeking", f.position() == 3);
  ATS_PrintTestStatus("peeking after peeking after seeking", f.peek() == '3');
  ATS_PrintTestStatus("position after peeking after seeking", f.position() == 3);
  ATS_PrintTestStatus("peeking after peeking after seeking", f.read() == '3');
  ATS_PrintTestStatus("position after peeking after seeking", f.position() == 4);
  
  f.seek(1);

  ATS_PrintTestStatus("position after seeking", f.position() == 1);
  ATS_PrintTestStatus("peeking after seeking", f.peek() == '1');
  
  f.seek(4);

  ATS_PrintTestStatus("position after seeking", f.position() == 4);
  ATS_PrintTestStatus("peeking after seeking", f.peek() == '4');

  f.seek(7);

  ATS_PrintTestStatus("position()", f.position() == 7);
  ATS_PrintTestStatus("read()", f.read() == '7');
  
  f.seek(0);
  f.peek();
  f.print("AB");
  
  ATS_PrintTestStatus("position()", f.position() == 2);
  ATS_PrintTestStatus("size()", f.size() == 10);
  ATS_PrintTestStatus("read()", f.read() == '2');

  f.seek(0);

  ATS_PrintTestStatus("read()", f.read() == 'A');
  ATS_PrintTestStatus("read()", f.read() == 'B');
  ATS_PrintTestStatus("read()", f.read() == '2');

  f.close();

  f = SD.open("test.txt");
  ATS_PrintTestStatus("SD.open()", f);
  if (!f) goto done;
  
  ATS_PrintTestStatus("position()", f.position() == 0);
  ATS_PrintTestStatus("size()", f.size() == 10);
  ATS_PrintTestStatus("peek()", f.peek() == 'A');
  ATS_PrintTestStatus("read()", f.read() == 'A');

  f.seek(4);
  
  ATS_PrintTestStatus("position()", f.position() == 4);
  ATS_PrintTestStatus("size()", f.size() == 10);
  ATS_PrintTestStatus("peek()", f.peek() == '4');
  ATS_PrintTestStatus("read()", f.read() == '4');
  
  f.close();

done:
  ATS_ReportMemoryUsage(startMemoryUsage);
  ATS_end();

}

void loop() {}
