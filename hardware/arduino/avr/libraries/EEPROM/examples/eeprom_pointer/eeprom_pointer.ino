/***
    eeprom_pointer example.
    
    This example shows how the built-in EEPtr
    object can be used to manipulate the EEPROM
    using standard pointer arithmetic.
    
    Running this sketch is not necessary, this is 
    simply highlighting certain programming methods.
    
    Written by Christopher Andrews 2015
    Released under MIT licence.
***/

#include <EEPROM.h>

void setup() {

  Serial.begin(9600);

  /***
    In this example, we will iterate forward over the EEPROM,
    starting at the 10th cell (remember indices are zero based).
  ***/
  
  EEPtr ptr = 9;
  
  //Rather than hard coding a length, we can use the provided .length() function.

  while( ptr < EEPROM.length() ){
  
    Serial.print( *ptr, HEX );  //Print out hex value of the EEPROM cell pointed to by 'ptr'
    Serial.print( ", " );       //Separate values with a comma.
    ptr++;                      //Move to next cell
  }

  /***
    In this example, we will iterate backwards over the EEPROM,
    starting at the last cell.
  ***/
  
  ptr = EEPROM.length() - 1;
  
  do{
    
    Serial.print( *ptr, HEX );
    Serial.print( ", " );
    
  }while( ptr-- ); //When the pointer reaches zero the loop will end as zero is considered 'false'.
  
  
  /***
    And just for clarity, the loop below is an equivalent implementation
    of the C++11 ranged for loop.
  ***/
  
  for( EEPtr ptr = EEPROM.begin() ; ptr != EEPROM.end() ; ++ptr ){
    Serial.print( *ptr, HEX );
    Serial.print( ", " );
  }
  
  /*** 
    The actual C++11 version:
  
    for( auto ptr : EEPROM ){
      Serial.print( *ptr, HEX );
      Serial.print( ", " );
    }
  ***/
  

}

void loop(){}