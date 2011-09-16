/*
 %atmel_license%
*/

#include "variant.h"

void setup( void )
{
  // Initialize the digital pin as an output.
  // Pin PIN_LED has a LED connected on most Arduino boards:
  pinMode( PIN_LED, OUTPUT ) ;
  digitalWrite( PIN_LED, LOW ) ;

  // Initialize the PIN_LED2 digital pin as an output.
  pinMode( PIN_LED2, OUTPUT ) ;
  digitalWrite( PIN_LED2, HIGH ) ;

  Serial.begin( 115200 ) ;
}

void led_step1( void )
{
#if defined sam3s_ek
  digitalWrite( PIN_LED, HIGH ) ;  // set the LED on
  digitalWrite( PIN_LED2, LOW ) ;   // set the red LED off
#endif /* sam3s_ek */

#if defined sam3u_ek
  digitalWrite( PIN_LED, HIGH ) ;  // set the LED on
  digitalWrite( PIN_LED2, LOW ) ;   // set the red LED off
#endif /* sam3u_ek */

#if defined arduino_due
  digitalWrite( PIN_LED, LOW ) ;  // set the LED on
  digitalWrite( PIN_LED2, LOW ) ;   // set the red LED off
#endif /* arduino_due */
}

void led_step2( void )
{
#if defined sam3s_ek
  digitalWrite( PIN_LED, LOW ) ;   // set the LED off
  digitalWrite( PIN_LED2, HIGH ) ;  // set the red LED on
#endif /* sam3s_ek */

#if defined sam3u_ek
  digitalWrite( PIN_LED, LOW ) ;   // set the LED off
  digitalWrite( PIN_LED2, HIGH ) ;  // set the red LED on
#endif /* sam3u_ek */

#if defined arduino_due
  digitalWrite( PIN_LED, HIGH ) ;   // set the LED off
  digitalWrite( PIN_LED2, HIGH ) ;  // set the red LED on
#endif /* arduino_due */
}

void loop( void )
{
  led_step1() ;
  delay( 1000 ) ;              // wait for a second
  led_step2() ;
  delay( 1000 ) ;              // wait for a second

  Serial.write( '-' ) ;   // send an initial char
  Serial.println( "test1\n" ) ;   // send an initial string
//  delay( 1000 ) ;               // wait for a second
//  Serial.println( "test2" ) ;   // send an initial string
}
