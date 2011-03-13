#include <ArduinoTestSuite.h>

void Test_Equal(char *testString, char *expected, const String &actual)
{
  char buf[100]; actual.toCharArray(buf, 100);
  boolean b = (strcmp(buf, expected) == 0);
  ATS_PrintTestStatus(testString, b);
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
  ATS_begin("Arduino", "String Addition Test");

  String stringOne = String("string");
  String stringTwo = String("other");
  String stringThree = stringOne + stringTwo;
  
  Test_Equal("Add strings", "stringother", stringThree);
  Test_Equal("Adding strings doesn't change them", "string", stringOne);
  Test_Equal("Adding strings doesn't change them", "other", stringTwo);  
  Test_Equal("Add strings", "stringotherstringstringstringother", stringOne + stringTwo + stringOne + stringOne + stringOne + stringTwo);
  Test_Equal("Add integer to string", "string123", stringOne + 123);
  Test_Equal("Add integer to string", "123string", 123 + stringOne);
  Test_Equal("Add integers to string", "string123456", stringOne + 123 + 456);
  Test_Equal("Add integer sum to string", "string579", stringOne + (123 + 456));
  Test_Equal("Add integers to string", "string123456789", stringOne + 123 + 456 + 789);
  Test_Equal("Add integers to string", "123string456", 123 + stringOne + 456);
  Test_Equal("Add long to string", "string123456789", stringOne + 123456789);
  Test_Equal("Add char", "stringA", stringOne + 'A');
  Test_Equal("Add char", "Astring", 'A' + stringOne);
  Test_Equal("Add \"string\"", "stringabc", stringOne +  "abc");
  Test_Equal("Add multiple types", "string123A123456789", stringOne + 123 + 'A' + 123456789);
  
  ATS_end();
}

void loop() {}












