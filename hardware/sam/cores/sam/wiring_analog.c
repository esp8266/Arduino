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

//#include "wiring_private.h"
#include "Arduino.h"

#ifdef __cplusplus
 extern "C" {
#endif


eAnalogReference analog_reference = AR_DEFAULT ;

void analogReference( eAnalogReference ulMode )
{
	analog_reference = ulMode ;
}

uint32_t analogRead( uint32_t ulPin )
{
  uint32_t ulValue ;
  uint32_t ulChannel ;

  ulChannel=g_APinDescription[ulPin].ulAnalogChannel ;

#if defined sam3u_ek
#elif defined sam3s_ek
#elif defined arduino_due

  switch ( ulChannel )
  {
    // Handling ADC 10 bits channels
    case ADC0 :
    case ADC1 :
    case ADC2 :
    case ADC3 :
    case ADC4 :
    case ADC5 :
    case ADC6 :
    case ADC7 :
      adc_enable_channel( ADC, ulChannel ) ;
      adc_start( ADC ) ;
      adc_get_value( ADC, ulChannel ) ;
      adc_stop( ADC ) ;
    break ;

    // Handling ADC 12 bits channels
    case ADC8 :
    case ADC9 :
    case ADC10 :
    case ADC11 :
    case ADC12 :
    case ADC13 :
    case ADC14 :
    case ADC15 :
      adc12_enable_channel( ADC12B, ulChannel-ADC8 ) ;
      adc12_start( ADC12B ) ;
      adc12_get_value( ADC12B, ulChannel-ADC8 ) ;
      adc12_stop( ADC12B ) ;
    break ;

    // Compiler could yell because we don't handle DAC pins
    default :
    break ;
  }
#endif



	return ulValue ;
}

// Right now, PWM output only works on the pins with
// hardware support.  These are defined in the appropriate
// pins_*.c file.  For the rest of the pins, we default
// to digital output.
void analogWrite( uint32_t ulPin, uint32_t ulValue )
{
	pinMode( ulPin, OUTPUT ) ;

	if ( ulValue == 0 )
	{
		digitalWrite( ulPin, LOW ) ;
	}
	else
  {
    if ( ulValue == 255)
    {
      digitalWrite( ulPin, HIGH ) ;
    }
    else
    {
      if ( (g_APinDescription[ulPin].ulPinAttribute && PIN_ATTR_PWM) == PIN_ATTR_PWM )
      {
        // Setup PWM for this pin
      }
      else
      {
        if ( (g_APinDescription[ulPin].ulPinAttribute && PIN_ATTR_TIMER) == PIN_ATTR_TIMER )
        {
          // Setup Timer for this pin
        }
        else
        {
          if ( ulValue < 128 )
          {
            digitalWrite( ulPin, LOW ) ;
          }
          else
          {
            digitalWrite( ulPin, HIGH ) ;
          }
        }
      }
    }
  }
}

#ifdef __cplusplus
}
#endif
