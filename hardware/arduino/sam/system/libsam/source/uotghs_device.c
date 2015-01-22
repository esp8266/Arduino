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

//#define TRACE_UOTGHS_DEVICE(x)	x
#define TRACE_UOTGHS_DEVICE(x)

extern void (*gpf_isr)(void);

static volatile uint32_t ul_send_fifo_ptr[MAX_ENDPOINTS];
static volatile uint32_t ul_recv_fifo_ptr[MAX_ENDPOINTS];

void UDD_SetStack(void (*pf_isr)(void))
{
	gpf_isr = pf_isr;
}

uint32_t UDD_Init(void)
{
	uint32_t i;

	for (i = 0; i < MAX_ENDPOINTS; ++i)
	{
		ul_send_fifo_ptr[i] = 0;
		ul_recv_fifo_ptr[i] = 0;
	}

	// Enables the USB Clock
	pmc_enable_periph_clk(ID_UOTGHS);
	pmc_enable_upll_clock();
	pmc_switch_udpck_to_upllck(0); // div=0+1
	pmc_enable_udpck();

	// Configure interrupts
	NVIC_SetPriority((IRQn_Type) ID_UOTGHS, 0UL);
	NVIC_EnableIRQ((IRQn_Type) ID_UOTGHS);

	// Always authorize asynchrone USB interrupts to exit from sleep mode
	//   for SAM3 USB wake up device except BACKUP mode
	//pmc_set_fast_startup_input(PMC_FSMR_USBAL);

	// ID pin not used then force device mode
	otg_disable_id_pin();
	otg_force_device_mode();

	// Enable USB hardware
	otg_disable_pad();
	otg_enable_pad();
	otg_enable();
	otg_unfreeze_clock();

	// Check USB clock
	//while (!Is_otg_clock_usable())
	//	;

	// Enable High Speed
	udd_low_speed_disable();
	udd_high_speed_enable();

	//otg_ack_vbus_transition();
	// Force Vbus interrupt in case of Vbus always with a high level
	// This is possible with a short timing between a Host mode stop/start.
	/*if (Is_otg_vbus_high()) {
		otg_raise_vbus_transition();
	}
	otg_enable_vbus_interrupt();*/
	otg_freeze_clock();

	return 0UL ;
}

void UDD_Attach(void)
{
	irqflags_t flags = cpu_irq_save();

	TRACE_UOTGHS_DEVICE(printf("=> UDD_Attach\r\n");)

	otg_unfreeze_clock();

	// Check USB clock because the source can be a PLL
	while (!Is_otg_clock_usable());

	// Authorize attach if Vbus is present
	udd_attach_device();

	// Enable USB line events
	udd_enable_reset_interrupt();
//	udd_enable_sof_interrupt();

	cpu_irq_restore(flags);
}

void UDD_Detach(void)
{
	TRACE_UOTGHS_DEVICE(printf("=> UDD_Detach\r\n");)
	UOTGHS->UOTGHS_DEVCTRL |= UOTGHS_DEVCTRL_DETACH;
}

void UDD_InitEP( uint32_t ul_ep_nb, uint32_t ul_ep_cfg )
{
	ul_ep_nb = ul_ep_nb & 0xF; // EP range is 0..9, hence mask is 0xF.

	TRACE_UOTGHS_DEVICE(printf("=> UDD_InitEP : init EP %lu\r\n", ul_ep_nb);)

	// Configure EP
	UOTGHS->UOTGHS_DEVEPTCFG[ul_ep_nb] = ul_ep_cfg;
	// Enable EP
	udd_enable_endpoint(ul_ep_nb);

	if (!Is_udd_endpoint_configured(ul_ep_nb)) {
		TRACE_UOTGHS_DEVICE(printf("=> UDD_InitEP : ERROR FAILED TO INIT EP %lu\r\n", ul_ep_nb);)
		while(1);
	}
}


void UDD_InitEndpoints(const uint32_t* eps_table, const uint32_t ul_eps_table_size)
{
	uint32_t ul_ep_nb ;

	for (ul_ep_nb = 1; ul_ep_nb < ul_eps_table_size; ul_ep_nb++)
	{
		// Configure EP
		UOTGHS->UOTGHS_DEVEPTCFG[ul_ep_nb] = eps_table[ul_ep_nb];
		// Enable EP
		udd_enable_endpoint(ul_ep_nb);

		if (!Is_udd_endpoint_configured(ul_ep_nb)) {
			TRACE_UOTGHS_DEVICE(printf("=> UDD_InitEP : ERROR FAILED TO INIT EP %lu\r\n", ul_ep_nb);)
			while(1);
		}
	}
}

// Wait until ready to accept IN packet.
void UDD_WaitIN(void)
{
	while (!(UOTGHS->UOTGHS_DEVEPTISR[EP0] & UOTGHS_DEVEPTISR_TXINI))
		;
}

void UDD_WaitOUT(void)
{
	while (!(UOTGHS->UOTGHS_DEVEPTISR[EP0] & UOTGHS_DEVEPTISR_RXOUTI))
		;
}

// Send packet.
void UDD_ClearIN(void)
{
	TRACE_UOTGHS_DEVICE(printf("=> UDD_ClearIN: sent %lu bytes\r\n", ul_send_fifo_ptr[EP0]);)

	UOTGHS->UOTGHS_DEVEPTICR[EP0] = UOTGHS_DEVEPTICR_TXINIC;
	ul_send_fifo_ptr[EP0] = 0;
}

void UDD_ClearOUT(void)
{
	UOTGHS->UOTGHS_DEVEPTICR[EP0] = UOTGHS_DEVEPTICR_RXOUTIC;
	ul_recv_fifo_ptr[EP0] = 0;
}

// Wait for IN FIFO to be ready to accept data or OUT FIFO to receive data.
// Return true if new IN FIFO buffer available.
uint32_t UDD_WaitForINOrOUT(void)
{
	while (!(UOTGHS->UOTGHS_DEVEPTISR[EP0] & (UOTGHS_DEVEPTISR_TXINI | UOTGHS_DEVEPTISR_RXOUTI)))
		;
	return ((UOTGHS->UOTGHS_DEVEPTISR[EP0] & UOTGHS_DEVEPTISR_RXOUTI) == 0);
}

uint32_t UDD_ReceivedSetupInt(void)
{
	return UOTGHS->UOTGHS_DEVEPTISR[EP0] & UOTGHS_DEVEPTISR_RXSTPI;
}

void UDD_ClearSetupInt(void)
{
	UOTGHS->UOTGHS_DEVEPTICR[EP0] = (UOTGHS_DEVEPTICR_RXSTPIC);
}

uint32_t UDD_Send(uint32_t ep, const void* data, uint32_t len)
{
	const uint8_t *ptr_src = data;
	uint8_t *ptr_dest = (uint8_t *) &udd_get_endpoint_fifo_access8(ep);
	uint32_t i;

	TRACE_UOTGHS_DEVICE(printf("=> UDD_Send (1): ep=%lu ul_send_fifo_ptr=%lu len=%lu\r\n", ep, ul_send_fifo_ptr[ep], len);)

	while( UOTGHS_DEVEPTISR_TXINI != (UOTGHS->UOTGHS_DEVEPTISR[ep] & UOTGHS_DEVEPTISR_TXINI )) {}

	if (ep == EP0)
	{
		if (ul_send_fifo_ptr[ep] + len > EP0_SIZE)
			len = EP0_SIZE - ul_send_fifo_ptr[ep];
	}
	else
	{
		ul_send_fifo_ptr[ep] = 0;
	}
	for (i = 0, ptr_dest += ul_send_fifo_ptr[ep]; i < len; ++i)
		*ptr_dest++ = *ptr_src++;

	ul_send_fifo_ptr[ep] += i;

	if (ep == EP0)
	{
		TRACE_UOTGHS_DEVICE(printf("=> UDD_Send (2): ep=%lu ptr_dest=%lu maxlen=%d\r\n", ep, ul_send_fifo_ptr[ep], EP0_SIZE);)
		if (ul_send_fifo_ptr[ep] == EP0_SIZE)
		{
			UDD_ClearIN();	// Fifo is full, release this packet  // UOTGHS->UOTGHS_DEVEPTICR[EP0] = UOTGHS_DEVEPTICR_TXINIC;
        }
	}
	else
	{
		UOTGHS->UOTGHS_DEVEPTICR[ep] = UOTGHS_DEVEPTICR_TXINIC;
		UOTGHS->UOTGHS_DEVEPTIDR[ep] = UOTGHS_DEVEPTIDR_FIFOCONC;
	}
	return len;
}

void UDD_Send8(uint32_t ep,  uint8_t data )
{
	uint8_t *ptr_dest = (uint8_t *) &udd_get_endpoint_fifo_access8(ep);

	TRACE_UOTGHS_DEVICE(printf("=> UDD_Send8 : ul_send_fifo_ptr=%lu data=0x%x\r\n", ul_send_fifo_ptr[ep], data);)

	ptr_dest[ul_send_fifo_ptr[ep]] = data;
	ul_send_fifo_ptr[ep] += 1;
}

uint8_t UDD_Recv8(uint32_t ep)
{
	uint8_t *ptr_dest = (uint8_t *) &udd_get_endpoint_fifo_access8(ep);
	uint8_t data = ptr_dest[ul_recv_fifo_ptr[ep]];

	TRACE_UOTGHS_DEVICE(printf("=> UDD_Recv8 : ul_recv_fifo_ptr=%lu\r\n", ul_recv_fifo_ptr[ep]);)

	ul_recv_fifo_ptr[ep] += 1;
	return data;
}

void UDD_Recv(uint32_t ep, uint8_t* data, uint32_t len)
{
	uint8_t *ptr_src = (uint8_t *) &udd_get_endpoint_fifo_access8(ep);
	uint8_t *ptr_dest = data;
	uint32_t i;

	for (i = 0, ptr_src += ul_recv_fifo_ptr[ep]; i < len; ++i)
		*ptr_dest++ = *ptr_src++;

	ul_recv_fifo_ptr[ep] += i;
}

void UDD_Stall(void)
{
	UOTGHS->UOTGHS_DEVEPT = (UOTGHS_DEVEPT_EPEN0 << EP0);
	UOTGHS->UOTGHS_DEVEPTIER[EP0] = UOTGHS_DEVEPTIER_STALLRQS;
}


uint32_t UDD_FifoByteCount(uint32_t ep)
{
	return ((UOTGHS->UOTGHS_DEVEPTISR[ep] & UOTGHS_DEVEPTISR_BYCT_Msk) >> UOTGHS_DEVEPTISR_BYCT_Pos);
}

void UDD_ReleaseRX(uint32_t ep)
{
	TRACE_UOTGHS_DEVICE(puts("=> UDD_ReleaseRX\r\n");)
//	UOTGHS->UOTGHS_DEVEPTICR[ep] = (UOTGHS_DEVEPTICR_NAKOUTIC | UOTGHS_DEVEPTICR_RXOUTIC);
	UOTGHS->UOTGHS_DEVEPTICR[ep] = UOTGHS_DEVEPTICR_RXOUTIC;
	UOTGHS->UOTGHS_DEVEPTIDR[ep] = UOTGHS_DEVEPTIDR_FIFOCONC;
	ul_recv_fifo_ptr[ep] = 0;
}

void UDD_ReleaseTX(uint32_t ep)
{
	TRACE_UOTGHS_DEVICE(printf("=> UDD_ReleaseTX ep=%lu\r\n", ep);)
//	UOTGHS->UOTGHS_DEVEPTICR[ep] = (UOTGHS_DEVEPTICR_NAKINIC | UOTGHS_DEVEPTICR_RXOUTIC | UOTGHS_DEVEPTICR_TXINIC);
	UOTGHS->UOTGHS_DEVEPTICR[ep] = UOTGHS_DEVEPTICR_TXINIC;
	UOTGHS->UOTGHS_DEVEPTIDR[ep] = UOTGHS_DEVEPTIDR_FIFOCONC;
	ul_send_fifo_ptr[ep] = 0;
}

// Return true if the current bank is not full.
uint32_t UDD_ReadWriteAllowed(uint32_t ep)
{
	return (UOTGHS->UOTGHS_DEVEPTISR[ep] & UOTGHS_DEVEPTISR_RWALL);
}

void UDD_SetAddress(uint32_t addr)
{
	TRACE_UOTGHS_DEVICE(printf("=> UDD_SetAddress : setting address to %lu\r\n", addr);)

	udd_configure_address(addr);
	udd_enable_address();
}

uint32_t UDD_GetFrameNumber(void)
{
	return udd_frame_number();
}

#endif /* SAM3XA_SERIES */
