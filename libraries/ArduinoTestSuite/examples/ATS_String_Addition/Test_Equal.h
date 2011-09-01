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
