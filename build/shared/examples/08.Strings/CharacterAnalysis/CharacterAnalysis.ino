/*
  Character analysis operators 
 
 Examples using the character analysis operators.
 Send any byte and the sketch will tell you about it.
 
 created 29 Nov 2010
 modified 2 Apr 2012
 by Tom Igoe
 
 This example code is in the public domain.
 */

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  // send an intro:
  Serial.println("send any byte and I'll tell you everything I can about it");
  Serial.println();
}

void loop() {
  // get any incoming bytes:
  if (Serial.available() > 0) {
    int thisChar = Serial.read();

    // say what was sent:
    Serial.print("You sent me: \'");
    Serial.write(thisChar);
    Serial.print("\'  ASCII Value: ");
    Serial.println(thisChar);

    // analyze what was sent:
    if(isAlphaNumeric(thisChar)) {
      Serial.println("it's alphanumeric");
    }
    if(isAlpha(thisChar)) {
      Serial.println("it's alphabetic");
    }
    if(isAscii(thisChar)) {
      Serial.println("it's ASCII");
    }
    if(isWhitespace(thisChar)) {
      Serial.println("it's whitespace");
    }
    if(isControl(thisChar)) {
      Serial.println("it's a control character");
    }
    if(isDigit(thisChar)) {
      Serial.println("it's a numeric digit");
    }
    if(isGraph(thisChar)) {
      Serial.println("it's a printable character that's not whitespace");
    }
    if(isLowerCase(thisChar)) {
      Serial.println("it's lower case");
    }
    if(isPrintable(thisChar)) {
      Serial.println("it's printable");
    }
    if(isPunct(thisChar)) {
      Serial.println("it's punctuation");
    }
    if(isSpace(thisChar)) {
      Serial.println("it's a space character");
    }
    if(isUpperCase(thisChar)) {
      Serial.println("it's upper case");
    }
    if (isHexadecimalDigit(thisChar)) {
      Serial.println("it's a valid hexadecimaldigit (i.e. 0 - 9, a - F, or A - F)");
    }

    // add some space and ask for another byte:
    Serial.println();
    Serial.println("Give me another byte:");
    Serial.println();
  }
}
