/* ----------------------------------------------------------------------------
 *         SAM Software Package License
 * ----------------------------------------------------------------------------
 * Copyright (c) 2011-2012, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following condition is met:
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

#include "chip.h"
#include <stdio.h>

#if SAM3XA_SERIES

//#define TRACE_UOTGHS_HOST(x)	x
#define TRACE_UOTGHS_HOST(x)

extern void (*gpf_isr)(void);

// Handle UOTGHS Host driver state
static uhd_vbus_state_t uhd_state = UHD_STATE_NO_VBUS;

/**
 * \brief Interrupt sub routine for USB Host state machine management.
 */
static void UHD_ISR(void)
{
	// Manage dis/connection event
	if (Is_uhd_disconnection() && Is_uhd_disconnection_int_enabled()) {
		TRACE_UOTGHS_HOST(printf(">>> UHD_ISR : Disconnection INT\r\n");)
		uhd_ack_disconnection();
		uhd_disable_disconnection_int();
		// Stop reset signal, in case of disconnection during reset
		uhd_stop_reset();
		// Disable wakeup/resumes interrupts,
		// in case of disconnection during suspend mode
		//UOTGHS->UOTGHS_HSTIDR = UOTGHS_HSTIDR_HWUPIEC
		//		| UOTGHS_HSTIDR_RSMEDIEC
		//		| UOTGHS_HSTIDR_RXRSMIEC;
		uhd_ack_connection();
		uhd_enable_connection_int();
		uhd_state = UHD_STATE_DISCONNECTED;
		return;
	}
	if (Is_uhd_connection() && Is_uhd_connection_int_enabled()) {
		TRACE_UOTGHS_HOST(printf(">>> UHD_ISR : Connection INT\r\n");)
		uhd_ack_connection();
		uhd_disable_connection_int();
		uhd_ack_disconnection();
		uhd_enable_disconnection_int();
		//uhd_enable_sof();
		uhd_state = UHD_STATE_CONNECTED;
		return;
	}

	// Manage Vbus error
	if (Is_uhd_vbus_error_interrupt())
	{
		TRACE_UOTGHS_HOST(printf(">>> UHD_ISR : VBUS error INT\r\n");)
		uhd_ack_vbus_error_interrupt();
		uhd_state = UHD_STATE_DISCONNECTED; //UHD_STATE_ERROR;
		return;
	}

	// Check USB clock ready after asynchronous interrupt
	while (!Is_otg_clock_usable())
		;
	otg_unfreeze_clock();

	// Manage Vbus state change
	if (Is_otg_vbus_transition())
	{
		otg_ack_vbus_transition();
		if (Is_otg_vbus_high())
		{
			TRACE_UOTGHS_HOST(printf(">>> UHD_ISR : VBUS transition INT : UHD_STATE_DISCONNECT\r\n");)
			uhd_state = UHD_STATE_DISCONNECTED;
		}
		else
		{
			TRACE_UOTGHS_HOST(printf(">>> UHD_ISR : VBUS transition INT : UHD_STATE_NO_VBUS\r\n");)
			otg_freeze_clock();
			uhd_state = UHD_STATE_NO_VBUS;
		}
		TRACE_UOTGHS_HOST(printf(">>> UHD_ISR : VBUS transition INT : done.\r\n");)
		return;
	}

	// Other errors
	if (Is_uhd_errors_interrupt())
	{
		TRACE_UOTGHS_HOST(printf(">>> UHD_ISR : Other error INT\r\n");)
		uhd_ack_errors_interrupt();
		return;
	}
}

/**
 * \brief Set the interrupt sub routines callback for USB interrupts.
 *
 * \param pf_isr the ISR address.
 */
void UHD_SetStack(void (*pf_isr)(void))
{
	gpf_isr = pf_isr;
}

/**
 * \brief Initialize the UOTGHS host driver.
 */
void UHD_Init(void)
{
	irqflags_t flags;

	// To avoid USB interrupt before end of initialization
	flags = cpu_irq_save();

	// Setup USB Host interrupt callback
	UHD_SetStack(&UHD_ISR);

	// Enables the USB Clock
	pmc_enable_upll_clock();
	pmc_switch_udpck_to_upllck(0); // div=0+1
	pmc_enable_udpck();
	pmc_enable_periph_clk(ID_UOTGHS);

	// Always authorize asynchronous USB interrupts to exit of sleep mode
	// For SAM3 USB wake up device except BACKUP mode
	NVIC_SetPriority((IRQn_Type) ID_UOTGHS, 0);
	NVIC_EnableIRQ((IRQn_Type) ID_UOTGHS);

	// ID pin not used then force host mode
	otg_disable_id_pin();
	otg_force_host_mode();

	// Signal is active low (because all SAM3X Pins are high after startup)
	// Hence VBOF must be low after connection request to power up the remote device
	// uhd_set_vbof_active_low();

	// According to the Arduino Due circuit the VBOF must be active high to power up the remote device
	uhd_set_vbof_active_high();

	otg_enable_pad();
	otg_enable();

	otg_unfreeze_clock();

	// Check USB clock
	while (!Is_otg_clock_usable())
		;

	// Clear all interrupts that may have been set by a previous host mode
	UOTGHS->UOTGHS_HSTICR = UOTGHS_HSTICR_DCONNIC | UOTGHS_HSTICR_DDISCIC
			| UOTGHS_HSTICR_HSOFIC  | UOTGHS_HSTICR_HWUPIC
			| UOTGHS_HSTICR_RSMEDIC | UOTGHS_HSTICR_RSTIC
			| UOTGHS_HSTICR_RXRSMIC;

	otg_ack_vbus_transition();

	// Enable Vbus change and error interrupts
	// Disable automatic Vbus control after Vbus error
	Set_bits(UOTGHS->UOTGHS_CTRL,
		UOTGHS_CTRL_VBUSHWC | UOTGHS_CTRL_VBUSTE | UOTGHS_CTRL_VBERRE);

	uhd_enable_vbus();

	// Force Vbus interrupt when Vbus is always high
	// This is possible due to a short timing between a Host mode stop/start.
	if (Is_otg_vbus_high())
	{
		otg_raise_vbus_transition();
	}

	// Enable main control interrupt
	// Connection, SOF and reset
	UOTGHS->UOTGHS_HSTIER = UOTGHS_HSTICR_DCONNIC;

	otg_freeze_clock();

	uhd_state = UHD_STATE_NO_VBUS;

	cpu_irq_restore(flags);
}

/**
 * \brief Trigger a USB bus reset.
 */
void UHD_BusReset(void)
{
	uhd_start_reset();
}

/**
 * \brief Get VBUS state.
 *
 * \return VBUS status.
 */
uhd_vbus_state_t UHD_GetVBUSState(void)
{
	return uhd_state;
}

/*uhd_speed_t uhd_get_speed(void)
{
	switch (uhd_get_speed_mode())
	{
		case UOTGHS_SR_SPEED_HIGH_SPEED:
			return UHD_SPEED_HIGH;

		case UOTGHS_SR_SPEED_FULL_SPEED:
			return UHD_SPEED_FULL;

		case UOTGHS_SR_SPEED_LOW_SPEED:
			return UHD_SPEED_LOW;

		default:
			return UHD_SPEED_LOW;
	}
}*/

/**
 * \brief Allocate FIFO for pipe 0.
 *
 * \param ul_add Address of remote device for pipe 0.
 * \param ul_ep_size Actual size of the FIFO in bytes.
 *
 * \retval 0 success.
 * \retval 1 error.
 */
uint32_t UHD_Pipe0_Alloc(uint32_t ul_add, uint32_t ul_ep_size)
{
	if (ul_ep_size < 8)
	{
		TRACE_UOTGHS_HOST(printf("/!\\ UHD_EP0_Alloc : incorrect pipe size!\r\n");)
		return 1;
	}

	if (Is_uhd_pipe_enabled(0))
	{
		// Pipe is already allocated
		return 0;
	}

	uhd_enable_pipe(0);
	uhd_configure_pipe(0, 	// Pipe 0
			0, 				// No frequency
			0, 				// Enpoint 0
			UOTGHS_HSTPIPCFG_PTYPE_CTRL,
			UOTGHS_HSTPIPCFG_PTOKEN_SETUP,
			ul_ep_size,
			UOTGHS_HSTPIPCFG_PBK_1_BANK, 0);

	uhd_allocate_memory(0);

	if (!Is_uhd_pipe_configured(0))
	{
		TRACE_UOTGHS_HOST(printf("/!\\ UHD_EP0_Alloc : incorrect pipe settings!\r\n");)
		uhd_disable_pipe(0);
		return 1;
	}

	uhd_configure_address(0, ul_add);

	return 0;
}

/**
 * \brief Allocate a new pipe.
 *
 * \note UOTGHS maximum pipe number is limited to 10, meaning that only a limited
 * amount of devices can be connected. Unfortunately, using only one pipe shared accross
 * various endpoints and devices is not possible because the UOTGHS IP does not allow to
 * change the data toggle value through register interface.
 *
 * \param ul_dev_addr Address of remote device.
 * \param ul_dev_ep Targeted endpoint of remote device.
 * \param ul_type Pipe type.
 * \param ul_dir Pipe direction.
 * \param ul_maxsize Pipe size.
 * \param ul_interval Polling interval (if applicable to pipe type).
 * \param ul_nb_bank Number of banks associated with this pipe.
 *
 * \return the newly allocated pipe number on success, 0 otherwise.
 */
uint32_t UHD_Pipe_Alloc(uint32_t ul_dev_addr, uint32_t ul_dev_ep, uint32_t ul_type, uint32_t ul_dir, uint32_t ul_maxsize, uint32_t ul_interval, uint32_t ul_nb_bank)
{
	uint32_t ul_pipe = 1;

	for (ul_pipe = 1; ul_pipe < UOTGHS_EPT_NUM; ++ul_pipe)
	{
		if (Is_uhd_pipe_enabled(ul_pipe))
		{
			continue;
		}

		uhd_enable_pipe(ul_pipe);

		uhd_configure_pipe(ul_pipe, ul_interval, ul_dev_ep, ul_type, ul_dir,
				ul_maxsize, ul_nb_bank, UOTGHS_HSTPIPCFG_AUTOSW);

		uhd_allocate_memory(ul_pipe);

		if (!Is_uhd_pipe_configured(ul_pipe))
		{
			uhd_disable_pipe(ul_pipe);
			return 0;
		}

		uhd_configure_address(ul_pipe, ul_dev_addr);

		// Pipe is configured and allocated successfully
		return ul_pipe;
	}

	return 0;
}

/**
 * \brief Free a pipe.
 *
 * \param ul_pipe Pipe number to free.
 */
void UHD_Pipe_Free(uint32_t ul_pipe)
{
	// Unalloc pipe
	uhd_disable_pipe(ul_pipe);
	uhd_unallocate_memory(ul_pipe);
	uhd_reset_pipe(ul_pipe);
}

/**
 * \brief Read from a pipe.
 *
 * \param ul_pipe Pipe number.
 * \param ul_size Maximum number of data to read.
 * \param data Buffer to store the data.
 *
 * \return number of data read.
 */
uint32_t UHD_Pipe_Read(uint32_t ul_pipe, uint32_t ul_size, uint8_t* data)
{
	uint8_t *ptr_ep_data = 0;
	uint8_t nb_byte_received = 0;
	uint32_t ul_nb_trans = 0;

	// Get information to read data
	nb_byte_received = uhd_byte_count(ul_pipe);

	ptr_ep_data = (uint8_t *) & uhd_get_pipe_fifo_access(ul_pipe, 8);

	// Copy data from pipe to payload buffer
	while (ul_size && nb_byte_received) {
		*data++ = *ptr_ep_data++;
		ul_nb_trans++;
		ul_size--;
		nb_byte_received--;
	}

	return ul_nb_trans;
}

/**
 * \brief Write into a pipe.
 *
 * \param ul_pipe Pipe number.
 * \param ul_size Maximum number of data to read.
 * \param data Buffer containing data to write.
 */
void UHD_Pipe_Write(uint32_t ul_pipe, uint32_t ul_size, uint8_t* data)
{
	volatile uint8_t *ptr_ep_data = 0;
	uint32_t i = 0;

	// Check pipe
	if (!Is_uhd_pipe_enabled(ul_pipe))
	{
		// Endpoint not valid
		TRACE_UOTGHS_HOST(printf("/!\\ UHD_EP_Send : pipe is not enabled!\r\n");)
		return;
	}

	ptr_ep_data = (volatile uint8_t *)&uhd_get_pipe_fifo_access(ul_pipe, 8);
	for (i = 0; i < ul_size; ++i)
		*ptr_ep_data++ = *data++;
}

/**
 * \brief Send a pipe content.
 *
 * \param ul_pipe Pipe number.
 * \param ul_token_type Token type.
 */
void UHD_Pipe_Send(uint32_t ul_pipe, uint32_t ul_token_type)
{
	// Check pipe
	if (!Is_uhd_pipe_enabled(ul_pipe))
	{
		// Endpoint not valid
		TRACE_UOTGHS_HOST(printf("/!\\ UHD_EP_Send : pipe %lu is not enabled!\r\n", ul_pipe);)
		return;
	}

	// Set token type for zero length packet
	// When actually using the FIFO, pipe token MUST be configured first
	uhd_configure_pipe_token(ul_pipe, ul_token_type);

	// Clear interrupt flags
	uhd_ack_setup_ready(ul_pipe);
	uhd_ack_in_received(ul_pipe);
	uhd_ack_out_ready(ul_pipe);
	uhd_ack_short_packet(ul_pipe);
	uhd_ack_nak_received(ul_pipe);

	// Send actual packet
	uhd_ack_fifocon(ul_pipe);
	uhd_unfreeze_pipe(ul_pipe);
}

/**
 * \brief Check for pipe transfer completion.
 *
 * \param ul_pipe Pipe number.
 * \param ul_token_type Token type.
 *
 * \retval 0 transfer is not complete.
 * \retval 1 transfer is complete.
 */
uint32_t UHD_Pipe_Is_Transfer_Complete(uint32_t ul_pipe, uint32_t ul_token_type)
{
	// Check for transfer completion depending on token type
	switch (ul_token_type)
	{
		case UOTGHS_HSTPIPCFG_PTOKEN_SETUP:
			if (Is_uhd_setup_ready(ul_pipe))
			{
				uhd_freeze_pipe(ul_pipe);
				uhd_ack_setup_ready(ul_pipe);
				return 1;
			}

		case UOTGHS_HSTPIPCFG_PTOKEN_IN:
			if (Is_uhd_in_received(ul_pipe))
			{
				// In case of low USB speed and with a high CPU frequency,
				// a ACK from host can be always running on USB line
				// then wait end of ACK on IN pipe.
				while(!Is_uhd_pipe_frozen(ul_pipe))
					;

				// IN packet received
				uhd_ack_in_received(ul_pipe);

				return 1;
			}

		case UOTGHS_HSTPIPCFG_PTOKEN_OUT:
			if (Is_uhd_out_ready(ul_pipe))
			{
				// OUT packet sent
				uhd_freeze_pipe(ul_pipe);
				uhd_ack_out_ready(ul_pipe);

				return 1;
			}
	}

	return 0;
}

#endif /* SAM3XA_SERIES */
