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

#include "Arduino.h"

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
			while ((adc_get_status(ADC) & ADC_SR_DRDY) != ADC_SR_DRDY)
				;

			// Read the value
			ulValue = adc_get_latest_value(ADC);

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
      adc12b_enable_channel( ADC12B, ulChannel );

      // Start the ADC12B
      adc12b_start( ADC12B );

			// Wait for end of conversion
			while ((adc12b_get_status(ADC12B) & ADC12B_SR_DRDY) != ADC12B_SR_DRDY)
				;

			// Read the value
			ulValue = adc12b_get_latest_value(ADC12B);

      // Stop the ADC12B
      //      adc12_stop( ADC12B ) ; // never do adc12_stop() else we have to reconfigure the ADC12B each time

      // Disable the corresponding channel
      adc12b_disable_channel( ADC12B, ulChannel );
		break;

		// Compiler could yell because we don't handle DAC pins
		default :
      ulValue=0;
		break;
	}
#endif

#if defined __SAM3X8E__
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
		case ADC8 :
		case ADC9 :
		case ADC10 :
		case ADC11 :

			// Enable the corresponding channel
			adc_enable_channel( ADC, ulChannel );

			// Start the ADC
			adc_start( ADC );

			// Wait for end of conversion
			while ((adc_get_status(ADC) & ADC_ISR_DRDY) != ADC_ISR_DRDY)
				;

			// Read the value
			ulValue = adc_get_latest_value(ADC);

			// Disable the corresponding channel
			adc_disable_channel(ADC, ulChannel);
			break;

		// Compiler could yell because we don't handle DAC pins
		default :
			ulValue=0;
			break;
	}
#endif

	return ulValue;
}

static void TC_SetRA(Tc *tc, uint32_t chan, uint32_t v )
{
    tc->TC_CHANNEL[chan].TC_RA = v;
}

static void TC_SetRB(Tc *tc, uint32_t chan, uint32_t v )
{
    tc->TC_CHANNEL[chan].TC_RB = v;
}

static void TC_SetRC(Tc *tc, uint32_t chan, uint32_t v )
{
    tc->TC_CHANNEL[chan].TC_RC = v;
}

static uint8_t PWMEnabled = 0;
static uint8_t pinEnabled[PINS_COUNT];
static uint8_t TCChanEnabled[] = {0, 0, 0};

void analogOutputInit(void) {
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
		    PMC_EnablePeripheral(PWM_INTERFACE_ID);
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
		}

		PWMC_SetDutyCycle(PWM_INTERFACE, chan, ulValue);
		return;
	}

	if ((attr & PIN_ATTR_TIMER) == PIN_ATTR_TIMER) {
		// We use MCLK/2 => 96Mhz/2 => 48Mhz as clock.
		// To get 1KHz we should use 48000 as TC
		// 48Mhz/48000 = 1KHz
		const uint32_t TC = VARIANT_MCK / 2 / TC_FREQUENCY;

		// Map value to Timer ranges 0..255=>0..48000
		ulValue = ulValue * TC;
		ulValue = ulValue / TC_MAX_DUTY_CYCLE;

		// Setup Timer for this pin
		ETCChannel channel = g_APinDescription[ulPin].ulTCChannel;
		static const uint32_t channelToChNo[] = { 0, 0, 1, 1, 2, 2 };
		static const uint32_t channelToAB[] = { 1, 0, 1, 0, 1, 0 };
		uint32_t chNo = channelToChNo[channel];
		uint32_t chA = channelToAB[channel];

		if (!TCChanEnabled[chNo]) {
			pmc_enable_periph_clk(TC_INTERFACE_ID + chNo);
			TC_Configure(TC_INTERFACE, chNo,
					TC_CMR_TCCLKS_TIMER_CLOCK1 |
					TC_CMR_WAVE |
					TC_CMR_WAVSEL_UP_RC |
					TC_CMR_ACPA_CLEAR |   // RA Compare Effect on OA: clear
					TC_CMR_ACPC_SET |     // RC Compare Effect on OA: set
					TC_CMR_BCPB_CLEAR |   // RB Compare Effect on OB: clear
					TC_CMR_BCPC_SET);     // RC Compare Effect on OB: set
			TC_SetRC(TC_INTERFACE, chNo, TC);
		}
		if (chA)
			TC_SetRA(TC_INTERFACE, chNo, ulValue);
		else
			TC_SetRB(TC_INTERFACE, chNo, ulValue);
		if (!pinEnabled[ulPin]) {
			PIO_Configure(g_APinDescription[ulPin].pPort,
					g_APinDescription[ulPin].ulPinType,
					g_APinDescription[ulPin].ulPin,
					g_APinDescription[ulPin].ulPinConfiguration);
			pinEnabled[ulPin] = 1;
		}
		if (!TCChanEnabled[chNo]) {
			TC_Start(TC_INTERFACE, chNo);
			TCChanEnabled[chNo] = 1;
		}
		return;
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
