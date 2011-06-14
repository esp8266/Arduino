
void setup( void )
{                
  // initialize the digital pin as an output.
  // Pin PIN_LED has an LED connected on most Arduino boards:
  pinMode( PIN_LED, OUTPUT ) ;     

  Serial.begin( 19200 ) ;
}

void loop( void )
{
  digitalWrite( PIN_LED, HIGH ) ;  // set the LED on
  delay( 1000 ) ;              // wait for a second
  digitalWrite( PIN_LED, LOW ) ;   // set the LED off
  delay( 1000 ) ;              // wait for a second
  
  Serial.println( "test1" ) ;   // send an initial string
  delay( 1000 ) ;               // wait for a second
  Serial.println( "test2" ) ;   // send an initial string
}
