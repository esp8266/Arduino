/***
    eeprom_reference example.
    
    This example shows how to use the EEPROM
    reference object EERef, which allows usage
    similar to using a simple char (uint8_t in this case).
    
    Running this sketch is not necessary, this is 
    simply highlighting certain programming methods.
    
    Written by Christopher Andrews 2015
    Released under MIT licence.
***/

#include <EEPROM.h>

void setup() {


  /***
    To create a reference to an EEPROM cell, simply create an EERef variable.
    To let it know which cell you want to reference, you can simply assign the
    address when you create it.
  ***/
  
  EERef ref = 0;
  
  /***
    An equivalent way is by calling the constructor directly:
    EERef ref( 0 );
  ***/
  
  /** Using the reference **/
  
  /*** 
    Updating cell data.
    To prevent unnecessary wear on the EEPROM cells
    this function will only write the data when it
    is different to what is already stored. 
  ***/
  
  ref.update( 44 ); //May write 44 if not present.
  ref.update( 44 ); //This second call will not write anything.
  
  /*** 
    Assign values directly to the EEPROM cell. 
    
    You can use any form of assignment that would otherwise be available
    to a standard uint8_t:
    
    *=
    /=
    +=
    -=
    ^=
    %=
    &=
    |=
    <<=
    >>=
    
  ***/
  
  ref = 4;  /*** 
                Take care to notice, this changes the EEPROM cell data, it does not 
                change the index of the cell referenced by 'ref'.
                
                Only the initial declaration like 'EERef ref = 0;' will set the address.
                Using an assignment anywhere else modifies the referenced cell.
                To modify the referenced address after declaring your variable see below.
            ***/

  /***
    Changing the referenced object. 
    The class has a member named 'index' which is an integer you can modify.
  ***/
  
  ref.index++; //Move reference to the next cell.
  
  
  /***
    Grouping of references.
    
    Using EERef objects you can create a contiguous array referencing
    non-contiguous EEPROM cells.
  ***/
  
  EERef array[] = { 0, 20, 40, 60, 80 };


} //End of setup function.

void loop(){}