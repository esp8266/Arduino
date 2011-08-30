#include <SD.h>
#include <ArduinoTestSuite.h>

void setup()
{
  int startMemoryUsage = ATS_GetFreeMemory();
  boolean b;
  File f;
  
  ATS_begin("Arduino", "SD Files Test");
  
  ATS_PrintTestStatus("SD.begin()", b = SD.begin(4));
  if (!b) goto done;
  
  ATS_PrintTestStatus("!SD.exists()", !SD.exists("asdf.txt"));
  ATS_PrintTestStatus("SD.open()", f = SD.open("asdf.txt", FILE_WRITE)); f.close();
  ATS_PrintTestStatus("SD.exists()", SD.exists("asdf.txt"));
  ATS_PrintTestStatus("SD.exists()", SD.exists("/asdf.txt"));
  ATS_PrintTestStatus("SD.remove()", SD.remove("asdf.txt"));
  ATS_PrintTestStatus("!SD.exists()", !SD.exists("asdf.txt"));
  
  ATS_PrintTestStatus("!SD.exists()", !SD.exists("asdf"));
  ATS_PrintTestStatus("SD.mkdir()", SD.mkdir("asdf"));
  ATS_PrintTestStatus("SD.exists()", SD.exists("asdf"));  
  ATS_PrintTestStatus("SD.exists()", SD.exists("/asdf"));  
  ATS_PrintTestStatus("SD.exists()", SD.exists("asdf/"));  
  ATS_PrintTestStatus("SD.rmdir()", SD.rmdir("asdf"));
  ATS_PrintTestStatus("!SD.exists()", !SD.exists("asdf"));

  ATS_PrintTestStatus("SD.mkdir()", SD.mkdir("x/y/z"));
  ATS_PrintTestStatus("SD.exists()", SD.exists("x"));  
  ATS_PrintTestStatus("SD.exists()", SD.exists("x/"));  
  ATS_PrintTestStatus("SD.exists()", SD.exists("x/y"));  
  ATS_PrintTestStatus("SD.exists()", SD.exists("x/y/"));  
  ATS_PrintTestStatus("SD.exists()", SD.exists("x/y/z"));  
  ATS_PrintTestStatus("SD.exists()", SD.exists("x/y/z/"));  
  ATS_PrintTestStatus("SD.exists()", SD.exists("/x/y/z/"));  
  ATS_PrintTestStatus("SD.rmdir()", SD.rmdir("x/y/z"));
  ATS_PrintTestStatus("SD.exists()", SD.exists("x"));  
  ATS_PrintTestStatus("SD.exists()", SD.exists("x/y"));  
  ATS_PrintTestStatus("!SD.exists()", !SD.exists("x/y/z"));  
  ATS_PrintTestStatus("SD.rmdir()", SD.rmdir("x/y/"));
  ATS_PrintTestStatus("SD.exists()", SD.exists("x"));  
  ATS_PrintTestStatus("!SD.exists()", !SD.exists("x/y"));  
  ATS_PrintTestStatus("!SD.exists()", !SD.exists("x/y/z"));  
  ATS_PrintTestStatus("SD.rmdir()", SD.rmdir("/x"));
  ATS_PrintTestStatus("!SD.exists()", !SD.exists("x"));  
  ATS_PrintTestStatus("!SD.exists()", !SD.exists("x/y"));  
  ATS_PrintTestStatus("!SD.exists()", !SD.exists("x/y/z"));  
  
  ATS_PrintTestStatus("!SD.open()", !(f = SD.open("asdf/asdf.txt", FILE_WRITE))); f.close();
  ATS_PrintTestStatus("!SD.exists()", !SD.exists("asdf"));
  ATS_PrintTestStatus("!SD.exists()", !SD.exists("asdf.txt"));
  ATS_PrintTestStatus("!SD.exists()", !SD.exists("asdf/asdf.txt"));
  ATS_PrintTestStatus("SD.mkdir()", SD.mkdir("asdf"));
  ATS_PrintTestStatus("SD.exists()", SD.exists("asdf"));
  ATS_PrintTestStatus("SD.open()", f = SD.open("asdf/asdf.txt", FILE_WRITE)); f.close();
  ATS_PrintTestStatus("SD.exists()", SD.exists("asdf/asdf.txt"));
  ATS_PrintTestStatus("!SD.rmdir()", !SD.rmdir("asdf"));
  ATS_PrintTestStatus("SD.exists()", SD.exists("asdf"));
  ATS_PrintTestStatus("SD.exists()", SD.exists("asdf/asdf.txt"));
  ATS_PrintTestStatus("SD.remove()", SD.remove("asdf/asdf.txt"));
  ATS_PrintTestStatus("!SD.exists()", !SD.exists("asdf/asdf.txt"));
  ATS_PrintTestStatus("SD.exists()", SD.exists("asdf"));
  ATS_PrintTestStatus("SD.rmdir()", SD.rmdir("asdf"));
  ATS_PrintTestStatus("!SD.exists()", !SD.exists("asdf"));
  
done:
  ATS_ReportMemoryUsage(startMemoryUsage);
  ATS_end();

}

void loop() {}




