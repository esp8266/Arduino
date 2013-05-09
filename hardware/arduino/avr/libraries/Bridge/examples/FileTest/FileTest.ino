
#include <FileIO.h>

void setup() {
  Bridge.begin();
  SD.begin();

  boolean r;
  r=SD.exists("/arduino/test");
  if (r) error("1");
  r=SD.exists("/arduino");
  if (!r) error("2");

  r=SD.mkdir("/arduino/test");
  if (!r) error("3");
  r=SD.exists("/arduino/test");
  if (!r) error("4");

  File f = SD.open("/arduino/test/bla", FILE_WRITE);
  if (!f) error("5");
  f.println("CIAO!");
  f.close();

  delay(10000);

  r=SD.rmdir("/arduino/test");
  if (r) error("6");
  r=SD.remove("/arduino/test");
  if (r) error("7");
  
  r=SD.remove("/arduino/test/bla");
  if (!r) error("8");
  r=SD.rmdir("/arduino/test");
  if (!r) error("9");
  r=SD.exists("/arduino/test");
  if (r) error("10");
}


void error(const char *s) {
  Bridge.print("# ERROR ");
  Bridge.println(s);
  while (true);
}

void loop() {
}



