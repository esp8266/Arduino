/*
  Comparing Strings 
 
 Examples of how to compare strings using the comparison operators
 
 The circuit:  No external hardware needed.
 
 created 27 July 2010
 by Tom Igoe
 
 This example code is in the public domain.
 
 */
String stringOne, stringTwo;

void setup() {
  Serial.begin(9600);
  stringOne = String("this");
  stringTwo = String("that");
  Serial.println("\n\nAdding strings together (concatenation):");

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

  // a numeric string compared to the number it represents:
  stringOne = "1";
  int numberOne = 1;
  if (stringOne == numberOne) {
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

  // do nothing while true:
  while(true);
}






