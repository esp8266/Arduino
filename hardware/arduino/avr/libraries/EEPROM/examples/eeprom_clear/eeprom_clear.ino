/*
 * EEPROM Clear
 *
 * Sets all of the bytes of the EEPROM to 0.
 * Please see eeprom_iteration for a more in depth
 * look at how to traverse the EEPROM.
 *
 * This example code is in the public domain.
 */

#include <EEPROM.h>

void setup()
{

  /***
    Iterate through each byte of the EEPROM storage.    
    
    Larger AVR processors have larger EEPROM sizes, E.g:
    - Arduno Duemilanove: 512b EEPROM storage.
    - Arduino Uno:        1kb EEPROM storage.
    - Arduino Mega:       4kb EEPROM storage.
    
    Rather than hard-coding the length, you should use the pre-provided length function.
    This will make your code portable to all AVR processors.    
  ***/
  
  for ( int i = 0 ; i < EEPROM.length() ; i++ )
    EEPROM.write(i, 0);

  // turn the LED on when we're done
  digitalWrite(13, HIGH);
}

void loop(){ /** Empty loop. **/ }
