#include "../Arduino.h"

void setup( void )
{
  // Initialize the digital pin as an output.
  // Pin PIN_LED has a LED connected on most Arduino boards:
  pinMode( PIN_LED, OUTPUT ) ;
  digitalWrite( PIN_LED, LOW ) ;

  // Initialize the PIN_LED_GREEN digital pin as an output.
  pinMode( PIN_LED_GREEN, OUTPUT ) ;
  digitalWrite( PIN_LED_GREEN, HIGH ) ;

//  Serial1.begin( 19200 ) ;
}

void loop( void )
{
  digitalWrite( PIN_LED, HIGH ) ;  // set the LED on
  digitalWrite( PIN_LED_GREEN, LOW ) ;   // set the red LED off
  delay( 1000 ) ;              // wait for a second
  digitalWrite( PIN_LED, LOW ) ;   // set the LED off
  digitalWrite( PIN_LED_GREEN, HIGH ) ;  // set the red LED on
  delay( 1000 ) ;              // wait for a second

  //Serial1.println( "test1" ) ;   // send an initial string
  //delay( 1000 ) ;               // wait for a second
  //Serial1.println( "test2" ) ;   // send an initial string
}
