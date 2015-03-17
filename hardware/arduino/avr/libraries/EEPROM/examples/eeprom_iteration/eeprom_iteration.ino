/***
    eeprom_iteration example.
    
    A set of example snippets highlighting the
    simplest methods for traversing the EEPROM.
    
    Running this sketch is not necessary, this is 
    simply highlighting certain programming methods.
    
    Written by Christopher Andrews 2015
    Released under MIT licence.
***/

#include <EEPROM.h>

void setup() {

  /***
    Iterate the EEPROM using a for loop.
  ***/
  
  for( int index = 0 ; index < EEPROM.length() ; index++ ){

    //Add one to each cell in the EEPROM
    EEPROM[ index ] += 1;
  }
  
  /***
    Iterate the EEPROM using a while loop.
  ***/
  
  int index = 0;
  
  while( index < EEPROM.length() ){
  
    //Add one to each cell in the EEPROM
    EEPROM[ index ] += 1;  
    index++;
  }
  
  /***
    Iterate the EEPROM using a do-while loop.
  ***/
  
  int idx = 0;
  
  do{
  
    //Add one to each cell in the EEPROM
    EEPROM[ index ] += 1;  
    index++;
  }while( index < EEPROM.length() );
  
  /***
    Iterate the EEPROM using a C++11 ranged for loop.
    
    This version of the loop is best explained in the example 'eeprom_pointer'
    as this kind of iteration uses pointers rather than an index/integer.
    
    !! Note: C++11 is not yet enabled by default in any IDE version.
       Unless you manually enable it, this sketch will not compile.
       You can comment the loop below to verify the non C++11 content.
  ***/
  
  for( auto cell : EEPROM ){

    //Add one to each cell in the EEPROM
    cell += 1;
  }
  
} //End of setup function.

void loop(){}