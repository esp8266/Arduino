/*
  ASCII table

 Prints out byte values in all possible formats:
 * as raw binary values
 * as ASCII-encoded decimal, hex, octal, and binary values

 For more on ASCII, see http://www.asciitable.com and http://en.wikipedia.org/wiki/ASCII

 The circuit:  No external hardware needed.

 created 2006
 by Nicholas Zambetti
 http://www.zambetti.com
 modified 9 Apr 2012
 by Tom Igoe
 modified 22 May 2013
 by Cristian Maglie

 This example code is in the public domain.

 http://arduino.cc/en/Tutorial/ConsoleAsciiTable

 */

#include <Console.h>

void setup() {
  //Initialize Console and wait for port to open:
  Bridge.begin();
  Console.begin();

  // Uncomment the following line to enable buffering:
  // - better transmission speed and efficiency
  // - needs to call Console.flush() to ensure that all
  //   transmitted data is sent

  //Console.buffer(64);

  while (!Console) {
    ; // wait for Console port to connect.
  }

  // prints title with ending line break
  Console.println("ASCII Table ~ Character Map");
}

// first visible ASCIIcharacter '!' is number 33:
int thisByte = 33;
// you can also write ASCII characters in single quotes.
// for example. '!' is the same as 33, so you could also use this:
//int thisByte = '!';

void loop() {
  // prints value unaltered, i.e. the raw binary version of the
  // byte. The Console monitor interprets all bytes as
  // ASCII, so 33, the first number,  will show up as '!'
  Console.write(thisByte);

  Console.print(", dec: ");
  // prints value as string as an ASCII-encoded decimal (base 10).
  // Decimal is the  default format for Console.print() and Console.println(),
  // so no modifier is needed:
  Console.print(thisByte);
  // But you can declare the modifier for decimal if you want to.
  //this also works if you uncomment it:

  // Console.print(thisByte, DEC);

  Console.print(", hex: ");
  // prints value as string in hexadecimal (base 16):
  Console.print(thisByte, HEX);

  Console.print(", oct: ");
  // prints value as string in octal (base 8);
  Console.print(thisByte, OCT);

  Console.print(", bin: ");
  // prints value as string in binary (base 2)
  // also prints ending line break:
  Console.println(thisByte, BIN);

  // if printed last visible character '~' or 126, stop:
  if (thisByte == 126) {    // you could also use if (thisByte == '~') {
    // ensure the latest bit of data is sent
    Console.flush();

    // This loop loops forever and does nothing
    while (true) {
      continue;
    }
  }
  // go on to the next character
  thisByte++;
}
