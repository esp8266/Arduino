/*
  Comparing Strings 
 
 Examples of how to compare strings using the comparison operators
 
 created 27 July 2010
 modified 2 Apr 2012
 by Tom Igoe
 
 http://arduino.cc/en/Tutorial/StringComparisonOperators
 
 This example code is in the public domain.
 */

String stringOne, stringTwo;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }


  stringOne = String("this");
  stringTwo = String("that");
  // send an intro:
  Serial.println("\n\nComparing Strings:");
  Serial.println();

}

void loop() {
  // two strings equal:
  if (stringOne == "this") {
    Serial.println("StringOne == \"this\""); 
  }
  // two strings not equal:
  if (stringOne != stringTwo) {
    Serial.println(stringOne + " =! " + stringTwo);
  }

  // two strings not equal (case sensitivity matters):
  stringOne = "This";
  stringTwo = "this";
  if (stringOne != stringTwo) {
    Serial.println(stringOne + " =! " + stringTwo);
  }
  // you can also use equals() to see if two strings are the same:
  if (stringOne.equals(stringTwo)) {
    Serial.println(stringOne + " equals " + stringTwo);
  } 
  else {
    Serial.println(stringOne + " does not equal " + stringTwo);
  }

  // or perhaps you want to ignore case:
  if (stringOne.equalsIgnoreCase(stringTwo)) {
    Serial.println(stringOne + " equals (ignoring case) " + stringTwo);
  } 
  else {
    Serial.println(stringOne + " does not equal (ignoring case) " + stringTwo);
  }

  // a numeric string compared to the number it represents:
  stringOne = "1";
  int numberOne = 1;
  if (stringOne.toInt() == numberOne) {
    Serial.println(stringOne + " = " + numberOne);
  }



  // two numeric strings compared:
  stringOne = "2";
  stringTwo = "1";
  if (stringOne >= stringTwo) {
    Serial.println(stringOne + " >= " + stringTwo);
  }

  // comparison operators can be used to compare strings for alphabetic sorting too:
  stringOne = String("Brown");
  if (stringOne < "Charles") {
    Serial.println(stringOne + " < Charles"); 
  }

  if (stringOne > "Adams") {
    Serial.println(stringOne + " > Adams"); 
  }

  if (stringOne <= "Browne") {
    Serial.println(stringOne + " <= Browne"); 
  }


  if (stringOne >= "Brow") {
    Serial.println(stringOne + " >= Brow"); 
  }

  // the compareTo() operator also allows you to compare strings
  // it evaluates on the first character that's different.
  // if the first character of the string you're comparing to
  // comes first in alphanumeric order, then compareTo() is greater than 0:
  stringOne = "Cucumber";
  stringTwo = "Cucuracha";
  if (stringOne.compareTo(stringTwo) < 0 ) {
    Serial.println(stringOne + " comes before " + stringTwo); 
  } 
  else {
    Serial.println(stringOne + " comes after " + stringTwo);    
  }

  delay(10000);  // because the next part is a loop:

  // compareTo() is handy when you've got strings with numbers in them too:

  while (true) {
    stringOne = "Sensor: ";
    stringTwo= "Sensor: ";

    stringOne += analogRead(A0); 
    stringTwo += analogRead(A5);

    if (stringOne.compareTo(stringTwo) < 0 ) {
      Serial.println(stringOne + " comes before " + stringTwo); 
    } 
    else {
      Serial.println(stringOne + " comes after " + stringTwo); 

    }
  }
}
