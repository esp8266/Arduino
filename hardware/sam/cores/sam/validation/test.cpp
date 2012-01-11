/*
  Copyright (c) 2011 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
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

static void led_step1( void )
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

static void led_step2( void )
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

  Serial.write( '-' ) ;   // send a char
//  Serial.write( "test1\n" ) ;   // send a string
//  Serial.write( "test2" ) ;   // send another string
}
