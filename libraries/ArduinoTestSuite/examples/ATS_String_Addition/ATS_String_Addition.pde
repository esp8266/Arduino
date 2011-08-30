#include <ArduinoTestSuite.h>
#include "Test_Equal.h"

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
  Test_Equal("Add string to integer", "string12345", stringOne + 12345);
  Test_Equal("Add string to negative integer", "string-12345", stringOne + -12345);
  Test_Equal("Add integer to string", "123string", 123 + stringOne);
  Test_Equal("Add string to integers", "string123456789", stringOne + 123 + 456 + 789);
  Test_Equal("Add integer to string", "123string456789", 123 + stringOne + 456 + 789);
  Test_Equal("Add string to long", "string123456789", stringOne + 123456789L);
  Test_Equal("Add string to negative long", "string-123456789", stringOne + -123456789L);
  Test_Equal("Add string to unsigned long", "string123456789", stringOne + 123456789UL);
  Test_Equal("Add string to byte", "string123", stringOne + byte(123));
  Test_Equal("Add char", "stringA", stringOne + 'A');
  Test_Equal("Add char", "Astring", 'A' + stringOne);
  Test_Equal("Add \"string\"", "stringabc", stringOne + "abc");
  Test_Equal("Add \"string\"", "abcstring", "abc" + stringOne);
  Test_Equal("Add multiple \"string\"", "stringabcdef", stringOne + "abc" + "def");
  Test_Equal("Add multiple \"string\"", "abcstringdef", "abc" + stringOne + "def");
  Test_Equal("Add \"string\" and int", "bc", "abc" + 1);
  
  ATS_end();
}

void loop() {}












