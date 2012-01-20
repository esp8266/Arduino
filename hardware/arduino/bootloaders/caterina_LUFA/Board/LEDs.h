/*
             LUFA Library
     Copyright (C) Dean Camera, 2011.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2011  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaim all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

#ifndef __LEDS_LEONARDO_H__
#define __LEDS_LEONARDO_H__

	/* Enable C linkage for C++ Compilers: */
		#if defined(__cplusplus)
			extern "C" {
		#endif

	/* Preprocessor Checks: */
		#if !defined(__INCLUDE_FROM_LEDS_H)
			#error Do not include this file directly. Include LUFA/Drivers/Board/LEDS.h instead.
		#endif

	/* Public Interface - May be used in end-application: */
		/* Macros: */
			/** LED mask for the TX LED on the board. */
			#define LEDS_TX_LED        	(1 << 5)
			#define LEDS_TX_LED_DDR		DDRD
			#define LEDS_TX_LED_PORT 	PORTD

			/** LED mask for the RX LED on the board. */
			#define LEDS_RX_LED        	(1 << 0)
			#define LEDS_RX_LED_DDR		DDRB
			#define LEDS_RX_LED_PORT	PORTB

			/** LED mask for the "L" LED on the board. */
			#define LEDS_L_LED        	(1 << 7)
			#define LEDS_L_LED_DDR		DDRC
			#define LEDS_L_LED_PORT		PORTC
			
			/** LED mask for all the LEDs on the board. */
			#define LEDS_ALL_LEDS 		(LEDS_TX_LED | LEDS_RX_LED | LEDS_L_LED)

			/** LED mask for none of the board LEDs. */
			#define LEDS_NO_LEDS     0

		/* Inline Functions: */
		#if !defined(__DOXYGEN__)
			static inline void LEDs_Init(void)
			{
				LEDS_TX_LED_DDR |= LEDS_TX_LED;
				LEDS_RX_LED_DDR |= LEDS_RX_LED;
				LEDS_L_LED_DDR |= LEDS_L_LED;
				LEDS_TX_LED_PORT |= LEDS_TX_LED;
				LEDS_RX_LED_PORT |= LEDS_RX_LED;
				LEDS_L_LED_PORT &= ~LEDS_L_LED;		// unlike TX and RX LEDs, L LED is active-high
			}

			static inline void LEDs_TurnOnLEDs(const uint8_t LEDMask)
			{
				LEDS_TX_LED_PORT &= ~(LEDMask & LEDS_TX_LED);
				LEDS_RX_LED_PORT &= ~(LEDMask & LEDS_RX_LED);
				LEDS_L_LED_PORT |= (LEDMask & LEDS_L_LED);
			}

			static inline void LEDs_TurnOffLEDs(const uint8_t LEDMask)
			{
				LEDS_TX_LED_PORT |= (LEDMask & LEDS_TX_LED);
				LEDS_RX_LED_PORT |= (LEDMask & LEDS_RX_LED);
				LEDS_L_LED_PORT &= ~(LEDMask & LEDS_L_LED);
			}

			static inline void LEDs_SetAllLEDs(const uint8_t LEDMask)
			{
//				PORTD = ((PORTD & ~LEDS_ALL_LEDS) | LEDMask);
				LEDs_TurnOffLEDs(LEDS_ALL_LEDS);
				LEDs_TurnOnLEDs(LEDMask);
			}

			static inline void LEDs_ChangeLEDs(const uint8_t LEDMask,
			                                   const uint8_t ActiveMask)
			{
				LEDs_TurnOffLEDs(LEDMask);
				LEDs_TurnOnLEDs(ActiveMask);
			}

			static inline void LEDs_ToggleLEDs(const uint8_t LEDMask)
			{
				LEDS_TX_LED_PORT ^= (LEDMask & LEDS_TX_LED);
				LEDS_RX_LED_PORT ^= (LEDMask & LEDS_RX_LED);
				LEDS_L_LED_PORT ^= (LEDMask & LEDS_L_LED);
			}

			static inline uint8_t LEDs_GetLEDs(void) ATTR_WARN_UNUSED_RESULT;
			static inline uint8_t LEDs_GetLEDs(void)
			{
				return ((LEDS_TX_LED_PORT & LEDS_TX_LED) | (LEDS_RX_LED_PORT & LEDS_RX_LED) | (LEDS_L_LED_PORT & LEDS_L_LED));
			}
		#endif

	/* Disable C linkage for C++ Compilers: */
		#if defined(__cplusplus)
			}
		#endif

#endif


