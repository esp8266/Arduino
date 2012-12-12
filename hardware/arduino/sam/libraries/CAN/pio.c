/**
 * \file
 *
 * \brief Parallel Input/Output (PIO) Controller driver for SAM.
 *
 * Copyright (c) 2011 - 2012 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#include "pio.h"

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/// @endcond

/**
 * \defgroup sam_drivers_pio_group Peripheral Parallel Input/Output (PIO) Controller
 *
 * \par Purpose
 *
 * The Parallel Input/Output Controller (PIO) manages up to 32 fully
 * programmable input/output lines. Each I/O line may be dedicated as a
 * general-purpose I/O or be assigned to a function of an embedded peripheral.
 * This assures effective optimization of the pins of a product.
 *
 * @{
 */

#ifndef FREQ_SLOW_CLOCK_EXT
/* External slow clock frequency (hz) */
#define FREQ_SLOW_CLOCK_EXT 32768
#endif

/**
 * \brief Configure PIO internal pull-up.
 *
 * \param p_pio Pointer to a PIO instance.
 * \param ul_mask Bitmask of one or more pin(s) to configure.
 * \param ul_pull_up_enable Indicates if the pin(s) internal pull-up shall be
 * configured.
 */
void pio_pull_up(Pio *p_pio, const uint32_t ul_mask,
		const uint32_t ul_pull_up_enable)
{
	/* Enable the pull-up(s) if necessary */
	if (ul_pull_up_enable) {
		p_pio->PIO_PUER = ul_mask;
	} else {
		p_pio->PIO_PUDR = ul_mask;
	}
}

/**
 * \brief Configure Glitch or Debouncing filter for the specified input(s).
 *
 * \param p_pio Pointer to a PIO instance.
 * \param ul_mask Bitmask of one or more pin(s) to configure.
 * \param ul_cut_off Cuts off frequency for debouncing filter.
 */
void pio_set_debounce_filter(Pio *p_pio, const uint32_t ul_mask,
		const uint32_t ul_cut_off)
{
#if (SAM3S || SAM3N || SAM4S)
	/* Set Debouncing, 0 bit field no effect */
	p_pio->PIO_IFSCER = ul_mask;
#elif (SAM3XA || SAM3U)
	/* Set Debouncing, 0 bit field no effect */
	p_pio->PIO_DIFSR = ul_mask;
#else
#error "Unsupported device"
#endif

	/* The debouncing filter can filter a pulse of less than 1/2 Period of a
	   programmable Divided Slow Clock:
	   Tdiv_slclk = ((DIV+1)*2).Tslow_clock */
	p_pio->PIO_SCDR = PIO_SCDR_DIV((FREQ_SLOW_CLOCK_EXT /
					(2 * (ul_cut_off))) - 1);
}

/**
 * \brief Set a high output level on all the PIOs defined in ul_mask.
 * This has no immediate effects on PIOs that are not output, but the PIO
 * controller will save the value if they are changed to outputs.
 *
 * \param p_pio Pointer to a PIO instance.
 * \param ul_mask Bitmask of one or more pin(s) to configure.
 */
void pio_set(Pio *p_pio, const uint32_t ul_mask)
{
	p_pio->PIO_SODR = ul_mask;
}

/**
 * \brief Set a low output level on all the PIOs defined in ul_mask.
 * This has no immediate effects on PIOs that are not output, but the PIO
 * controller will save the value if they are changed to outputs.
 *
 * \param p_pio Pointer to a PIO instance.
 * \param ul_mask Bitmask of one or more pin(s) to configure.
 */
void pio_clear(Pio *p_pio, const uint32_t ul_mask)
{
	p_pio->PIO_CODR = ul_mask;
}

/**
 * \brief Return 1 if one or more PIOs of the given Pin instance currently have
 * a high level; otherwise returns 0. This method returns the actual value that
 * is being read on the pin. To return the supposed output value of a pin, use
 * pio_get_output_data_status() instead.
 *
 * \param p_pio Pointer to a PIO instance.
 * \param ul_type PIO type.
 * \param ul_mask Bitmask of one or more pin(s) to configure.
 *
 * \retval 1 at least one PIO currently has a high level.
 * \retval 0 all PIOs have a low level.
 */
uint32_t pio_get(Pio *p_pio, const pio_type_t ul_type,
		const uint32_t ul_mask)
{
	uint32_t ul_reg;

	if ((ul_type == PIO_OUTPUT_0) || (ul_type == PIO_OUTPUT_1)) {
		ul_reg = p_pio->PIO_ODSR;
	} else {
		ul_reg = p_pio->PIO_PDSR;
	}

	if ((ul_reg & ul_mask) == 0) {
		return 0;
	} else {
		return 1;
	}
}

/**
 * \brief Configure IO of a PIO controller as being controlled by a specific
 * peripheral.
 *
 * \param p_pio Pointer to a PIO instance.
 * \param ul_type PIO type.
 * \param ul_mask Bitmask of one or more pin(s) to configure.
 */
void pio_set_peripheral(Pio *p_pio, const pio_type_t ul_type,
		const uint32_t ul_mask)
{
	uint32_t ul_sr;

	/* Disable interrupts on the pin(s) */
	p_pio->PIO_IDR = ul_mask;

#if (SAM3S || SAM3N || SAM4S)
	switch (ul_type) {
	case PIO_PERIPH_A:
		ul_sr = p_pio->PIO_ABCDSR[0];
		p_pio->PIO_ABCDSR[0] &= (~ul_mask & ul_sr);

		ul_sr = p_pio->PIO_ABCDSR[1];
		p_pio->PIO_ABCDSR[1] &= (~ul_mask & ul_sr);
		break;

	case PIO_PERIPH_B:
		ul_sr = p_pio->PIO_ABCDSR[0];
		p_pio->PIO_ABCDSR[0] = (ul_mask | ul_sr);

		ul_sr = p_pio->PIO_ABCDSR[1];
		p_pio->PIO_ABCDSR[1] &= (~ul_mask & ul_sr);
		break;

	case PIO_PERIPH_C:
		ul_sr = p_pio->PIO_ABCDSR[0];
		p_pio->PIO_ABCDSR[0] &= (~ul_mask & ul_sr);

		ul_sr = p_pio->PIO_ABCDSR[1];
		p_pio->PIO_ABCDSR[1] = (ul_mask | ul_sr);
		break;

	case PIO_PERIPH_D:
		ul_sr = p_pio->PIO_ABCDSR[0];
		p_pio->PIO_ABCDSR[0] = (ul_mask | ul_sr);

		ul_sr = p_pio->PIO_ABCDSR[1];
		p_pio->PIO_ABCDSR[1] = (ul_mask | ul_sr);
		break;

		// other types are invalid in this function
	case PIO_INPUT:
	case PIO_OUTPUT_0:
	case PIO_OUTPUT_1:
	case PIO_NOT_A_PIN:
		return;
	}
#elif (SAM3XA|| SAM3U)
	switch (ul_type) {
	case PIO_PERIPH_A:
		ul_sr = p_pio->PIO_ABSR;
		p_pio->PIO_ABSR &= (~ul_mask & ul_sr);
		break;

	case PIO_PERIPH_B:
		ul_sr = p_pio->PIO_ABSR;
		p_pio->PIO_ABSR = (ul_mask | ul_sr);
		break;

		// other types are invalid in this function
	case PIO_INPUT:
	case PIO_OUTPUT_0:
	case PIO_OUTPUT_1:
	case PIO_NOT_A_PIN:
		return;
	}
#else
#error "Unsupported device"
#endif

	// Remove the pins from under the control of PIO
	p_pio->PIO_PDR = ul_mask;
}

/**
 * \brief Configure one or more pin(s) or a PIO controller as inputs.
 * Optionally, the corresponding internal pull-up(s) and glitch filter(s) can
 * be enabled.
 *
 * \param p_pio Pointer to a PIO instance.
 * \param ul_mask Bitmask indicating which pin(s) to configure as input(s).
 * \param ul_attribute PIO attribute(s).
 */
void pio_set_input(Pio *p_pio, const uint32_t ul_mask,
		const uint32_t ul_attribute)
{
	pio_disable_interrupt(p_pio, ul_mask);
	pio_pull_up(p_pio, ul_mask, ul_attribute & PIO_PULLUP);

	/* Enable Input Filter if necessary */
	if (ul_attribute & (PIO_DEGLITCH | PIO_DEBOUNCE)) {
		p_pio->PIO_IFER = ul_mask;
	} else {
		p_pio->PIO_IFDR = ul_mask;
	}

#if (SAM3S || SAM3N || SAM4S)
	/* Enable de-glitch or de-bounce if necessary */
	if (ul_attribute & PIO_DEGLITCH) {
		p_pio->PIO_IFSCDR = ul_mask;
	} else {
		if (ul_attribute & PIO_DEBOUNCE) {
			p_pio->PIO_IFSCER = ul_mask;
		}
	}
#elif (SAM3XA|| SAM3U)
	/* Enable de-glitch or de-bounce if necessary */
	if (ul_attribute & PIO_DEGLITCH) {
		p_pio->PIO_SCIFSR = ul_mask;
	} else {
		if (ul_attribute & PIO_DEBOUNCE) {
			p_pio->PIO_SCIFSR = ul_mask;
		}
	}
#else
#error "Unsupported device"
#endif

	/* Configure pin as input */
	p_pio->PIO_ODR = ul_mask;
	p_pio->PIO_PER = ul_mask;
}

/**
 * \brief Configure one or more pin(s) of a PIO controller as outputs, with
 * the given default value. Optionally, the multi-drive feature can be enabled
 * on the pin(s).
 *
 * \param p_pio Pointer to a PIO instance.
 * \param ul_mask Bitmask indicating which pin(s) to configure.
 * \param ul_default_level Default level on the pin(s).
 * \param ul_multidrive_enable Indicates if the pin(s) shall be configured as
 * open-drain.
 * \param ul_pull_up_enable Indicates if the pin shall have its pull-up
 * activated.
 */
void pio_set_output(Pio *p_pio, const uint32_t ul_mask,
		const uint32_t ul_default_level,
		const uint32_t ul_multidrive_enable,
		const uint32_t ul_pull_up_enable)
{
	pio_disable_interrupt(p_pio, ul_mask);
	pio_pull_up(p_pio, ul_mask, ul_pull_up_enable);

	/* Enable multi-drive if necessary */
	if (ul_multidrive_enable) {
		p_pio->PIO_MDER = ul_mask;
	} else {
		p_pio->PIO_MDDR = ul_mask;
	}

	/* Set default value */
	if (ul_default_level) {
		p_pio->PIO_SODR = ul_mask;
	} else {
		p_pio->PIO_CODR = ul_mask;
	}

	/* Configure pin(s) as output(s) */
	p_pio->PIO_OER = ul_mask;
	p_pio->PIO_PER = ul_mask;
}

/**
 * \brief Perform complete pin(s) configuration; general attributes and PIO init
 * if necessary.
 *
 * \param p_pio Pointer to a PIO instance.
 * \param ul_type PIO type.
 * \param ul_mask Bitmask of one or more pin(s) to configure.
 * \param ul_attribute Pins attributes.
 *
 * \return Whether the pin(s) have been configured properly.
 */
uint32_t pio_configure(Pio *p_pio, const pio_type_t ul_type,
		const uint32_t ul_mask, const uint32_t ul_attribute)
{
	/* Configure pins */
	switch (ul_type) {
	case PIO_PERIPH_A:
	case PIO_PERIPH_B:
#     if (SAM3S || SAM3N || SAM4S)
	case PIO_PERIPH_C:
	case PIO_PERIPH_D:
#     endif
		pio_set_peripheral(p_pio, ul_type, ul_mask);
		pio_pull_up(p_pio, ul_mask, (ul_attribute & PIO_PULLUP));
		break;

	case PIO_INPUT:
		pio_set_input(p_pio, ul_mask, ul_attribute);
		break;

	case PIO_OUTPUT_0:
	case PIO_OUTPUT_1:
		pio_set_output(p_pio, ul_mask, (ul_type == PIO_OUTPUT_1),
				(ul_attribute & PIO_OPENDRAIN) ? 1 : 0,
				(ul_attribute & PIO_PULLUP) ? 1 : 0);
		break;

	default:
		return 0;
	}

	return 1;
}

/**
 * \brief Return 1 if one or more PIOs of the given Pin are configured to
 * output a high level (even if they are not output).
 * To get the actual value of the pin, use PIO_Get() instead.
 *
 * \param p_pio Pointer to a PIO instance.
 * \param ul_mask Bitmask of one or more pin(s).
 *
 * \retval 1 At least one PIO is configured to output a high level.
 * \retval 0 All PIOs are configured to output a low level.
 */
uint32_t pio_get_output_data_status(const Pio *p_pio,
		const uint32_t ul_mask)
{
	if ((p_pio->PIO_ODSR & ul_mask) == 0) {
		return 0;
	} else {
		return 1;
	}
}

/**
 * \brief Configure PIO pin multi-driver.
 *
 * \param p_pio Pointer to a PIO instance.
 * \param ul_mask Bitmask of one or more pin(s) to configure.
 * \param ul_multi_driver_enable Indicates if the pin(s) multi-driver shall be
 * configured.
 */
void pio_set_multi_driver(Pio *p_pio, const uint32_t ul_mask,
		const uint32_t ul_multi_driver_enable)
{
	/* Enable the multi-driver if necessary */
	if (ul_multi_driver_enable) {
		p_pio->PIO_MDER = ul_mask;
	} else {
		p_pio->PIO_MDDR = ul_mask;
	}
}

/**
 * \brief Get multi-driver status.
 *
 * \param p_pio Pointer to a PIO instance.
 *
 * \return The multi-driver mask value.
 */
uint32_t pio_get_multi_driver_status(const Pio *p_pio)
{
	return p_pio->PIO_MDSR;
}


#if (SAM3S || SAM3N || SAM4S)
/**
 * \brief Configure PIO pin internal pull-down.
 *
 * \param p_pio Pointer to a PIO instance.
 * \param ul_mask Bitmask of one or more pin(s) to configure.
 * \param ul_pull_down_enable Indicates if the pin(s) internal pull-down shall
 * be configured.
 */
void pio_pull_down(Pio *p_pio, const uint32_t ul_mask,
		const uint32_t ul_pull_down_enable)
{
	/* Enable the pull-down if necessary */
	if (ul_pull_down_enable) {
		p_pio->PIO_PPDER = ul_mask;
	} else {
		p_pio->PIO_PPDDR = ul_mask;
	}
}
#endif

/**
 * \brief Enable PIO output write for synchronous data output.
 *
 * \param p_pio Pointer to a PIO instance.
 * \param ul_mask Bitmask of one or more pin(s) to configure.
 */
void pio_enable_output_write(Pio *p_pio, const uint32_t ul_mask)
{
	p_pio->PIO_OWER = ul_mask;
}

/**
 * \brief Disable PIO output write.
 *
 * \param p_pio Pointer to a PIO instance.
 * \param ul_mask Bitmask of one or more pin(s) to configure.
 */
void pio_disable_output_write(Pio *p_pio, const uint32_t ul_mask)
{
	p_pio->PIO_OWDR = ul_mask;
}

/**
 * \brief Read PIO output write status.
 *
 * \param p_pio Pointer to a PIO instance.
 *
 * \return The output write mask value.
 */
uint32_t pio_get_output_write_status(const Pio *p_pio)
{
	return p_pio->PIO_OWSR;
}

/**
 * \brief Synchronously write on output pins.
 * \note Only bits unmasked by PIO_OWSR (Output Write Status Register) are
 * written.
 *
 * \param p_pio Pointer to a PIO instance.
 * \param ul_mask Bitmask of one or more pin(s) to configure.
 */
void pio_sync_output_write(Pio *p_pio, const uint32_t ul_mask)
{
	p_pio->PIO_ODSR = ul_mask;
}

#if (SAM3S || SAM3N || SAM4S)
/**
 * \brief Configure PIO pin schmitt trigger. By default the Schmitt trigger is
 * active.
 * Disabling the Schmitt Trigger is requested when using the QTouch Library.
 *
 * \param p_pio Pointer to a PIO instance.
 * \param ul_mask Bitmask of one or more pin(s) to configure.
 */
void pio_set_schmitt_trigger(Pio *p_pio, const uint32_t ul_mask)
{
	p_pio->PIO_SCHMITT = ul_mask;
}

/**
 * \brief Get PIO pin schmitt trigger status.
 *
 * \param p_pio Pointer to a PIO instance.
 *
 * \return The schmitt trigger mask value.
 */
uint32_t pio_get_schmitt_trigger(const Pio *p_pio)
{
	return p_pio->PIO_SCHMITT;
}
#endif

/**
 * \brief Configure the given interrupt source.
 * Interrupt can be configured to trigger on rising edge, falling edge,
 * high level, low level or simply on level change.
 *
 * \param p_pio Pointer to a PIO instance.
 * \param ul_mask Interrupt source bit map.
 * \param ul_attr Interrupt source attributes.
 */
void pio_configure_interrupt(Pio *p_pio, const uint32_t ul_mask,
		const uint32_t ul_attr)
{
	/* Configure additional interrupt mode registers. */
	if (ul_attr & PIO_IT_AIME) {
		/* Enable additional interrupt mode. */
		p_pio->PIO_AIMER = ul_mask;

		/* If bit field of the selected pin is 1, set as
		   Rising Edge/High level detection event. */
		if (ul_attr & PIO_IT_RE_OR_HL) {
			/* Rising Edge or High Level */
			p_pio->PIO_REHLSR = ul_mask;
		} else {
			/* Falling Edge or Low Level */
			p_pio->PIO_FELLSR = ul_mask;
		}

		/* If bit field of the selected pin is 1, set as
		   edge detection source. */
		if (ul_attr & PIO_IT_EDGE) {
			/* Edge select */
			p_pio->PIO_ESR = ul_mask;
		} else {
			/* Level select */
			p_pio->PIO_LSR = ul_mask;
		}
	} else {
		/* Disable additional interrupt mode. */
		p_pio->PIO_AIMDR = ul_mask;
	}
}

/**
 * \brief Enable the given interrupt source.
 * The PIO must be configured as an NVIC interrupt source as well.
 * The status register of the corresponding PIO controller is cleared
 * prior to enabling the interrupt.
 *
 * \param p_pio Pointer to a PIO instance.
 * \param ul_mask Interrupt sources bit map.
 */
void pio_enable_interrupt(Pio *p_pio, const uint32_t ul_mask)
{
	p_pio->PIO_ISR;
	p_pio->PIO_IER = ul_mask;
}

/**
 * \brief Disable a given interrupt source, with no added side effects.
 *
 * \param p_pio Pointer to a PIO instance.
 * \param ul_mask Interrupt sources bit map.
 */
void pio_disable_interrupt(Pio *p_pio, const uint32_t ul_mask)
{
	p_pio->PIO_IDR = ul_mask;
}

/**
 * \brief Read PIO interrupt status.
 *
 * \param p_pio Pointer to a PIO instance.
 *
 * \return The interrupt status mask value.
 */
uint32_t pio_get_interrupt_status(const Pio *p_pio)
{
	return p_pio->PIO_ISR;
}

/**
 * \brief Read PIO interrupt mask.
 *
 * \param p_pio Pointer to a PIO instance.
 *
 * \return The interrupt mask value.
 */
uint32_t pio_get_interrupt_mask(const Pio *p_pio)
{
	return p_pio->PIO_IMR;
}

/**
 * \brief Set additional interrupt mode.
 *
 * \param p_pio Pointer to a PIO instance.
 * \param ul_mask Interrupt sources bit map.
 * \param ul_attribute Pin(s) attributes.
 */
void pio_set_additional_interrupt_mode(Pio *p_pio,
		const uint32_t ul_mask, const uint32_t ul_attribute)
{
	/* Enables additional interrupt mode if needed */
	if (ul_attribute & PIO_IT_AIME) {
		/* Enables additional interrupt mode */
		p_pio->PIO_AIMER = ul_mask;

		/* Configures the Polarity of the event detection */
		/* (Rising/Falling Edge or High/Low Level) */
		if (ul_attribute & PIO_IT_RE_OR_HL) {
			/* Rising Edge or High Level */
			p_pio->PIO_REHLSR = ul_mask;
		} else {
			/* Falling Edge or Low Level */
			p_pio->PIO_FELLSR = ul_mask;
		}

		/* Configures the type of event detection (Edge or Level) */
		if (ul_attribute & PIO_IT_EDGE) {
			/* Edge select */
			p_pio->PIO_ESR = ul_mask;
		} else {
			/* Level select */
			p_pio->PIO_LSR = ul_mask;
		}
	} else {
		/* Disable additional interrupt mode */
		p_pio->PIO_AIMDR = ul_mask;
	}
}

#define PIO_WPMR_WPKEY_VALUE PIO_WPMR_WPKEY(0x50494Fu)

/**
 * \brief Enable or disable write protect of PIO registers.
 *
 * \param p_pio Pointer to a PIO instance.
 * \param ul_enable 1 to enable, 0 to disable.
 */
void pio_set_writeprotect(Pio *p_pio, const uint32_t ul_enable)
{
	p_pio->PIO_WPMR = PIO_WPMR_WPKEY_VALUE | ul_enable;
}

/**
 * \brief Read write protect status.
 *
 * \param p_pio Pointer to a PIO instance.
 *
 * \return Return write protect status.
 */
uint32_t pio_get_writeprotect_status(const Pio *p_pio)
{
	return p_pio->PIO_WPSR;
}

#define PIO_DELTA   ((uint32_t) PIOB - (uint32_t) PIOA)

/**
 * \brief Return the value of a pin.
 *
 * \param ul_pin The pin number.
 *
 * \return The pin value.
 *
 * \note If pin is output: a pull-up or pull-down could hide the actual value.
 *       The function \ref pio_get can be called to get the actual pin output
 *       level.
 * \note If pin is input: PIOx must be clocked to sample the signal.
 *       See PMC driver.
 */
uint32_t pio_get_pin_value(uint32_t ul_pin)
{
	Pio *p_pio = (Pio *)((uint32_t)PIOA + (PIO_DELTA * (ul_pin >> 5)));
	return (p_pio->PIO_PDSR >> (ul_pin & 0x1F)) & 1;
}

/**
 * \brief Drive a GPIO pin to 1.
 *
 * \param ul_pin The pin index.
 *
 * \note The function \ref pio_configure_pin must be called beforehand.
 */
void pio_set_pin_high(uint32_t ul_pin)
{
	Pio *p_pio = (Pio *)((uint32_t)PIOA + (PIO_DELTA * (ul_pin >> 5)));
	// Value to be driven on the I/O line: 1.
	p_pio->PIO_SODR = 1 << (ul_pin & 0x1F);
}

/**
 * \brief Drive a GPIO pin to 0.
 *
 * \param ul_pin The pin index.
 *
 * \note The function \ref pio_configure_pin must be called before.
 */
void pio_set_pin_low(uint32_t ul_pin)
{
	Pio *p_pio = (Pio *)((uint32_t)PIOA + (PIO_DELTA * (ul_pin >> 5)));
	// Value to be driven on the I/O line: 0.
	p_pio->PIO_CODR = 1 << (ul_pin & 0x1F);
}

/**
 * \brief Toggle a GPIO pin.
 *
 * \param ul_pin The pin index.
 *
 * \note The function \ref pio_configure_pin must be called before.
 */
void pio_toggle_pin(uint32_t ul_pin)
{
	Pio *p_pio = (Pio *)((uint32_t)PIOA + (PIO_DELTA * (ul_pin >> 5)));
	if (p_pio->PIO_ODSR & (1 << (ul_pin & 0x1F))) {
		// Value to be driven on the I/O line: 0.
		p_pio->PIO_CODR = 1 << (ul_pin & 0x1F);
	} else {
		// Value to be driven on the I/O line: 1.
		p_pio->PIO_SODR = 1 << (ul_pin & 0x1F);
	}
}

/**
 * \brief Perform complete pin(s) configuration; general attributes and PIO init
 * if necessary.
 *
 * \param ul_pin Bitmask of one or more pin(s) to configure.
 * \param ul_flags Pins attributes.
 *
 * \return Whether the pin(s) have been configured properly.
 */
uint32_t pio_configure_pin(uint32_t ul_pin, const uint32_t ul_flags)
{
	Pio *p_pio = (Pio *)((uint32_t)PIOA + (PIO_DELTA * (ul_pin >> 5)));

	/* Configure pins */
	switch (ul_flags & PIO_TYPE_Msk) {
	case PIO_TYPE_PIO_PERIPH_A:
		pio_set_peripheral(p_pio, PIO_PERIPH_A, (1 << (ul_pin & 0x1F)));
		pio_pull_up(p_pio, (1 << (ul_pin & 0x1F)),
				(ul_flags & PIO_PULLUP));
		break;
	case PIO_TYPE_PIO_PERIPH_B:
		pio_set_peripheral(p_pio, PIO_PERIPH_B, (1 << (ul_pin & 0x1F)));
		pio_pull_up(p_pio, (1 << (ul_pin & 0x1F)),
				(ul_flags & PIO_PULLUP));
		break;
#     if (SAM3S || SAM3N || SAM4S)
	case PIO_TYPE_PIO_PERIPH_C:
		pio_set_peripheral(p_pio, PIO_PERIPH_C, (1 << (ul_pin & 0x1F)));
		pio_pull_up(p_pio, (1 << (ul_pin & 0x1F)),
				(ul_flags & PIO_PULLUP));
		break;
	case PIO_TYPE_PIO_PERIPH_D:
		pio_set_peripheral(p_pio, PIO_PERIPH_D, (1 << (ul_pin & 0x1F)));
		pio_pull_up(p_pio, (1 << (ul_pin & 0x1F)),
				(ul_flags & PIO_PULLUP));
		break;
#     endif

	case PIO_TYPE_PIO_INPUT:
		pio_set_input(p_pio, (1 << (ul_pin & 0x1F)), ul_flags);
		break;

	case PIO_TYPE_PIO_OUTPUT_0:
	case PIO_TYPE_PIO_OUTPUT_1:
		pio_set_output(p_pio, (1 << (ul_pin & 0x1F)),
				((ul_flags & PIO_TYPE_PIO_OUTPUT_1)
				== PIO_TYPE_PIO_OUTPUT_1) ? 1 : 0,
				(ul_flags & PIO_OPENDRAIN) ? 1 : 0,
				(ul_flags & PIO_PULLUP) ? 1 : 0);
		break;

	default:
		return 0;
	}

	return 1;
}

/**
 * \brief Drive a GPIO port to 1.
 *
 * \param p_pio Base address of the PIO port.
 * \param ul_mask Bitmask of one or more pin(s) to toggle.
 */
void pio_set_pin_group_high(Pio *p_pio, uint32_t ul_mask)
{
	// Value to be driven on the I/O line: 1.
	p_pio->PIO_SODR = ul_mask;
}

/**
 * \brief Drive a GPIO port to 0.
 *
 * \param p_pio Base address of the PIO port.
 * \param ul_mask Bitmask of one or more pin(s) to toggle.
 */
void pio_set_pin_group_low(Pio *p_pio, uint32_t ul_mask)
{
	// Value to be driven on the I/O line: 0.
	p_pio->PIO_CODR = ul_mask;
}

/**
 * \brief Toggle a GPIO group.
 *
 * \param p_pio Pointer to a PIO instance.
 * \param ul_mask Bitmask of one or more pin(s) to configure.
 */
void pio_toggle_pin_group(Pio *p_pio, uint32_t ul_mask)
{
	if (p_pio->PIO_ODSR & ul_mask) {
		// Value to be driven on the I/O line: 0.
		p_pio->PIO_CODR = ul_mask;
	} else {
		// Value to be driven on the I/O line: 1.
		p_pio->PIO_SODR = ul_mask;
	}
}

/**
 * \brief Perform complete pin(s) configuration; general attributes and PIO init
 * if necessary.
 *
 * \param p_pio Pointer to a PIO instance.
 * \param ul_mask Bitmask of one or more pin(s) to configure.
 * \param ul_flags Pin(s) attributes.
 *
 * \return Whether the pin(s) have been configured properly.
 */
uint32_t pio_configure_pin_group(Pio *p_pio,
		uint32_t ul_mask, const uint32_t ul_flags)
{
	/* Configure pins */
	switch (ul_flags & PIO_TYPE_Msk) {
	case PIO_TYPE_PIO_PERIPH_A:
		pio_set_peripheral(p_pio, PIO_PERIPH_A, ul_mask);
		pio_pull_up(p_pio, ul_mask, (ul_flags & PIO_PULLUP));
		break;
	case PIO_TYPE_PIO_PERIPH_B:
		pio_set_peripheral(p_pio, PIO_PERIPH_B, ul_mask);
		pio_pull_up(p_pio, ul_mask, (ul_flags & PIO_PULLUP));
		break;
#     if (SAM3S || SAM3N || SAM4S)
	case PIO_TYPE_PIO_PERIPH_C:
		pio_set_peripheral(p_pio, PIO_PERIPH_C, ul_mask);
		pio_pull_up(p_pio, ul_mask, (ul_flags & PIO_PULLUP));
		break;
	case PIO_TYPE_PIO_PERIPH_D:
		pio_set_peripheral(p_pio, PIO_PERIPH_D, ul_mask);
		pio_pull_up(p_pio, ul_mask, (ul_flags & PIO_PULLUP));
		break;
#     endif

	case PIO_TYPE_PIO_INPUT:
		pio_set_input(p_pio, ul_mask, ul_flags);
		break;

	case PIO_TYPE_PIO_OUTPUT_0:
	case PIO_TYPE_PIO_OUTPUT_1:
		pio_set_output(p_pio, ul_mask,
				((ul_flags & PIO_TYPE_PIO_OUTPUT_1)
				== PIO_TYPE_PIO_OUTPUT_1) ? 1 : 0,
				(ul_flags & PIO_OPENDRAIN) ? 1 : 0,
				(ul_flags & PIO_PULLUP) ? 1 : 0);
		break;

	default:
		return 0;
	}

	return 1;
}

/**
 * \brief Enable interrupt for a GPIO pin.
 *
 * \param ul_pin The pin index.
 *
 * \note The function \ref gpio_configure_pin must be called before.
 */
void pio_enable_pin_interrupt(uint32_t ul_pin)
{
	Pio *p_pio = (Pio *)((uint32_t)PIOA + (PIO_DELTA * (ul_pin >> 5)));
	p_pio->PIO_IER = 1 << (ul_pin & 0x1F);
}


/**
 * \brief Disable interrupt for a GPIO pin.
 *
 * \param ul_pin The pin index.
 *
 * \note The function \ref gpio_configure_pin must be called before.
 */
void pio_disable_pin_interrupt(uint32_t ul_pin)
{
	Pio *p_pio = (Pio *)((uint32_t)PIOA + (PIO_DELTA * (ul_pin >> 5)));
	p_pio->PIO_IDR = 1 << (ul_pin & 0x1F);
}


/**
 * \brief Return GPIO port for a GPIO pin.
 *
 * \param ul_pin The pin index.
 *
 * \return Pointer to \ref Pio struct for GPIO port.
 */
Pio *pio_get_pin_group(uint32_t ul_pin)
{
	Pio *p_pio = (Pio *)((uint32_t)PIOA + (PIO_DELTA * (ul_pin >> 5)));
	return p_pio;
}

/**
 * \brief Return GPIO port peripheral ID for a GPIO pin.
 *
 * \param ul_pin The pin index.
 *
 * \return GPIO port peripheral ID.
 */
uint32_t pio_get_pin_group_id(uint32_t ul_pin)
{
	uint32_t ul_id = ID_PIOA + (ul_pin >> 5);
	return ul_id;
}


/**
 * \brief Return GPIO port pin mask for a GPIO pin.
 *
 * \param ul_pin The pin index.
 *
 * \return GPIO port pin mask.
 */
uint32_t pio_get_pin_group_mask(uint32_t ul_pin)
{
	uint32_t ul_mask = 1 << (ul_pin & 0x1F);
	return ul_mask;
}



#if (SAM3S || SAM4S)
/**
 * \brief Configure PIO capture mode.
 * \note PIO capture mode will be disabled automatically.
 *
 * \param p_pio Pointer to a PIO instance.
 * \param ul_mode Bitmask of one or more modes.
 */
void pio_capture_set_mode(Pio *p_pio, uint32_t ul_mode)
{
	ul_mode &= (~PIO_PCMR_PCEN); /* Disable PIO capture mode */
	p_pio->PIO_PCMR = ul_mode;
}

/**
 * \brief Enable PIO capture mode.
 *
 * \param p_pio Pointer to a PIO instance.
 */
void pio_capture_enable(Pio *p_pio)
{
	p_pio->PIO_PCMR |= PIO_PCMR_PCEN;
}

/**
 * \brief Disable PIO capture mode.
 *
 * \param p_pio Pointer to a PIO instance.
 */
void pio_capture_disable(Pio *p_pio)
{
	p_pio->PIO_PCMR &= (~PIO_PCMR_PCEN);
}

/**
 * \brief Read from Capture Reception Holding Register.
 * Data presence should be tested before any read attempt.
 *
 * \param p_pio Pointer to a PIO instance.
 * \param pul_data Pointer to store the data.
 *
 * \retval 0 Success.
 * \retval 1 I/O Failure, Capture data is not ready.
 */
uint32_t pio_capture_read(const Pio *p_pio, uint32_t *pul_data)
{
	/* Check if the data is ready */
	if ((p_pio->PIO_PCISR & PIO_PCISR_DRDY) == 0) {
		return 1;
	}

	/* Read data */
	*pul_data = p_pio->PIO_PCRHR;
	return 0;
}

/**
 * \brief Enable the given interrupt source of PIO capture. The status
 * register of the corresponding PIO capture controller is cleared prior
 * to enabling the interrupt.
 *
 * \param p_pio Pointer to a PIO instance.
 * \param ul_mask Interrupt sources bit map.
 */
void pio_capture_enable_interrupt(Pio *p_pio, const uint32_t ul_mask)
{
	p_pio->PIO_PCISR;
	p_pio->PIO_PCIER = ul_mask;
}

/**
 * \brief Disable a given interrupt source of PIO capture.
 *
 * \param p_pio Pointer to a PIO instance.
 * \param ul_mask Interrupt sources bit map.
 */
void pio_capture_disable_interrupt(Pio *p_pio, const uint32_t ul_mask)
{
	p_pio->PIO_PCIDR = ul_mask;
}

/**
 * \brief Read PIO interrupt status of PIO capture.
 *
 * \param p_pio Pointer to a PIO instance.
 *
 * \return The interrupt status mask value.
 */
uint32_t pio_capture_get_interrupt_status(const Pio *p_pio)
{
	return p_pio->PIO_PCISR;
}

/**
 * \brief Read PIO interrupt mask of PIO capture.
 *
 * \param p_pio Pointer to a PIO instance.
 *
 * \return The interrupt mask value.
 */
uint32_t pio_capture_get_interrupt_mask(const Pio *p_pio)
{
	return p_pio->PIO_PCIMR;
}

/**
 * \brief Get PDC registers base address.
 *
 * \param p_pio Pointer to an PIO peripheral.
 *
 * \return PIOA PDC register base address.
 */
Pdc *pio_capture_get_pdc_base(const Pio *p_pio)
{
	p_pio = p_pio; /* Stop warning */
	return PDC_PIOA;
}
#endif

//@}

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond
