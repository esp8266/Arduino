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
#include "variant.h"

#ifdef __cplusplus
extern "C" {
#endif

eAnalogReference analog_reference = AR_DEFAULT;

void analogReference(eAnalogReference ulMode)
{
	analog_reference = ulMode;
}

uint32_t analogRead(uint32_t ulPin)
{
  uint32_t ulValue = 0;
  uint32_t ulChannel;

  if (ulPin < A0)
    ulPin += A0;

  ulChannel = g_APinDescription[ulPin].ulADCChannelNumber ;

#if defined __SAM3U4E__
	switch ( g_APinDescription[ulPin].ulAnalogChannel )
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
      // Enable the corresponding channel
      adc_enable_channel( ADC, ulChannel );

      // Start the ADC
      adc_start( ADC );

      // Wait for end of conversion
      while ((adc_get_status(ADC) & (1<<ulChannel)) == 0);

      // Read the value
      ulValue=adc_get_value( ADC, ulChannel );

      // Disable the corresponding channel
      adc_disable_channel( ADC, ulChannel );

      // Stop the ADC
      //      adc_stop( ADC ) ; // never do adc_stop() else we have to reconfigure the ADC each time
		break;

		// Handling ADC 12 bits channels
		case ADC8 :
		case ADC9 :
		case ADC10 :
		case ADC11 :
		case ADC12 :
		case ADC13 :
		case ADC14 :
		case ADC15 :
      // Enable the corresponding channel
      adc12_enable_channel( ADC12B, ulChannel );

      // Start the ADC12B
      adc12_start( ADC12B );

      // Wait for end of conversion
      while ((adc12_get_status(ADC12B) & (1<<(ulChannel))) == 0);

      // Read the value
      ulValue=adc12_get_value( ADC12B, ulChannel );

      // Stop the ADC12B
      //      adc12_stop( ADC12B ) ; // never do adc12_stop() else we have to reconfigure the ADC12B each time

      // Disable the corresponding channel
      adc12_disable_channel( ADC12B, ulChannel );
		break;

		// Compiler could yell because we don't handle DAC pins
		default :
      ulValue=0;
		break;
	}
#endif

	return ulValue;
}

static uint8_t PWMEnabled = 0;
static uint8_t TCEnabled = 0;
static uint8_t pinEnabled[PINS_COUNT];

void analogOutputInit() {
	uint8_t i;
	for (i=0; i<PINS_COUNT; i++)
		pinEnabled[i] = 0;
}

// Right now, PWM output only works on the pins with
// hardware support.  These are defined in the appropriate
// pins_*.c file.  For the rest of the pins, we default
// to digital output.
void analogWrite(uint32_t ulPin, uint32_t ulValue) {
	uint32_t attr = g_APinDescription[ulPin].ulPinAttribute;

	if ((attr & PIN_ATTR_PWM) == PIN_ATTR_PWM) {
		if (!PWMEnabled) {
			// PWM Startup code
		    PMC_EnablePeripheral(ID_PWM);
		    PWMC_ConfigureClocks(PWM_FREQUENCY * PWM_MAX_DUTY_CYCLE, 0, VARIANT_MCK);
			PWMEnabled = 1;
		}

		uint32_t chan = g_APinDescription[ulPin].ulPWMChannel;
		if (!pinEnabled[ulPin]) {
			// Setup PWM for this pin
			PIO_Configure(g_APinDescription[ulPin].pPort,
					g_APinDescription[ulPin].ulPinType,
					g_APinDescription[ulPin].ulPin,
					g_APinDescription[ulPin].ulPinConfiguration);
			PWMC_ConfigureChannel(PWM_INTERFACE, chan, PWM_CMR_CPRE_CLKA, 0, 0);
			PWMC_SetPeriod(PWM_INTERFACE, chan, PWM_MAX_DUTY_CYCLE);
			PWMC_SetDutyCycle(PWM_INTERFACE, chan, ulValue);
			PWMC_EnableChannel(PWM_INTERFACE, chan);
			pinEnabled[ulPin] = 1;
		} else {
			PWMC_SetDutyCycle(PWM_INTERFACE, chan, ulValue);
		}
		return;
	}

	if ((attr & PIN_ATTR_TIMER) == PIN_ATTR_TIMER) {
		// TODO
		/*
		if (!TCEnabled) {
			TCEnabled = 1;
		}

		// Setup Timer for this pin
		if (!pinEnabled[ulPin]) {
		} else {
		}
		return;
		*/
	}

	// Default to digital write
	pinMode(ulPin, OUTPUT);
	if (ulValue < 128)
		digitalWrite(ulPin, LOW);
	else
		digitalWrite(ulPin, HIGH);
}

#ifdef __cplusplus
}
#endif
