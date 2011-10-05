/* ----------------------------------------------------------------------------
 *         SAM Software Package License 
 * ----------------------------------------------------------------------------
 * Copyright (c) 2011, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
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
  Serial.write( "test1\n" ) ;   // send a string
  Serial.write( "test2" ) ;   // send another string
}
