#include <ArduinoTestSuite.h>

void Test_Equal(long actual, long expected)
{
  char buf[100];
  boolean b = expected == actual;
  ATS_PrintTestStatus("", b);
  if (!b) {
    Serial.print("expected '");
    Serial.print(expected);
    Serial.print("', actual '");
    Serial.print(actual);
    Serial.println("'");
  }
}

void setup()
{
  byte buf[5] = { 65, 66, 67, 0, 69 };
  ATS_begin("Arduino", "Write & Print Return Values Test");

  Test_Equal(Serial.write('a'), 1);
  Test_Equal(Serial.write(byte(0)), 1);
  Test_Equal(Serial.write("abc"), 3);
  Test_Equal(Serial.write(""), 0);
  Test_Equal(Serial.write(buf, 5), 5);
  Test_Equal(Serial.print(0), 1);
  Test_Equal(Serial.print(""), 0);
  Test_Equal(Serial.print("abc"), 3);
  Test_Equal(Serial.print(0), 1);
  Test_Equal(Serial.print(1), 1);
  Test_Equal(Serial.print(11), 2);
  Test_Equal(Serial.print(12345), 5);
  Test_Equal(Serial.print(-1), 2);
  Test_Equal(Serial.print(-123), 4);
  Test_Equal(Serial.println(), 2);
  Test_Equal(Serial.println(""), 2);
  Test_Equal(Serial.println("abc"), 5);
  Test_Equal(Serial.println(0), 3);
  Test_Equal(Serial.println(1), 3);
  Test_Equal(Serial.println(11), 4);
  Test_Equal(Serial.println(12345), 7);
  Test_Equal(Serial.println(-1), 4);
  Test_Equal(Serial.println(-123), 6);
  
  ATS_end();
}

void loop() {}












