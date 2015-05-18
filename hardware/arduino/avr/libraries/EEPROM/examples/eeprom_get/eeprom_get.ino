/***
    eeprom_get example.
 
    This shows how to use the EEPROM.get() method.
    
    To pre-set the EEPROM data, run the example sketch eeprom_put.
    This sketch will run without it, however, the values shown 
    will be shown from what ever is already on the EEPROM.
    
    This may cause the serial object to print out a large string
    of garbage if there is no null character inside one of the strings
    loaded.
    
    Written by Christopher Andrews 2015
    Released under MIT licence.      
***/

#include <EEPROM.h>

void setup(){
  
  float f = 0.00f;   //Variable to store data read from EEPROM.
  int eeAddress = 0; //EEPROM address to start reading from
  
  Serial.begin( 9600 );
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  Serial.print( "Read float from EEPROM: " );

  //Get the float data from the EEPROM at position 'eeAddress'
  EEPROM.get( eeAddress, f );
  Serial.println( f, 3 );  //This may print 'ovf, nan' if the data inside the EEPROM is not a valid float.
  
  /***
    As get also returns a reference to 'f', you can use it inline.
    E.g: Serial.print( EEPROM.get( eeAddress, f ) );
  ***/
  
  /*** 
    Get can be used with custom structures too. 
    I have separated this into an extra function.
  ***/
  
  secondTest(); //Run the next test.
}

struct MyObject{
  float field1;
  byte field2;
  char name[10];
};

void secondTest(){
  int eeAddress = sizeof(float); //Move address to the next byte after float 'f'.

  MyObject customVar; //Variable to store custom object read from EEPROM.
  EEPROM.get( eeAddress, customVar );
  
  Serial.println( "Read custom object from EEPROM: " );
  Serial.println( customVar.field1 );
  Serial.println( customVar.field2 );
  Serial.println( customVar.name );
}

void loop(){ /* Empty loop */ }