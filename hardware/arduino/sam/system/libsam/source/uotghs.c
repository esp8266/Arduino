/*
  Copyright (c) 2012 Arduino.  All right reserved.

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

#include "chip.h"
#include <stdio.h>

#if SAM3XA_SERIES

static void (*gpf_isr)(void)	= (0UL);

static volatile uint32_t ul_ep			= (0UL);
static volatile uint32_t ul_send_index	= (0UL);
static volatile uint32_t ul_recv_index	= (0UL);

void UDD_SetStack(void (*pf_isr)(void))
{
	gpf_isr = pf_isr;
}

void UOTGHS_Handler( void )
{
	if (gpf_isr)
		gpf_isr();
}

uint32_t UDD_Init(void)
{
	uint32_t ul ;

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
	while (!Is_otg_clock_usable())
		;

	udd_low_speed_disable();
	udd_high_speed_disable();

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
	//USBCON = ((1<<USBE)|(1<<OTGPADE));	// start USB clock
	//UDIEN = (1<<EORSTE)|(1<<SOFE);		// Enable interrupts for EOR (End of Reset) and SOF (start of frame)
	//UDCON = 0;							// enable attach resistor

	irqflags_t flags = cpu_irq_save();

	//printf("=> UDD_Attach\r\n");


	otg_unfreeze_clock();

	// This section of clock check can be improved with a chek of
	// USB clock source via sysclk()
	// Check USB clock because the source can be a PLL
	while (!Is_otg_clock_usable());

	// Authorize attach if Vbus is present
	udd_attach_device();

	// Enable USB line events
	udd_enable_reset_interrupt();
	//udd_enable_suspend_interrupt();
	//udd_enable_wake_up_interrupt();


	//////////////udd_enable_sof_interrupt();



	// Reset following interupts flag
	//udd_ack_reset();
	//udd_ack_sof();


	// The first suspend interrupt must be forced
	// The first suspend interrupt is not detected else raise it
	//udd_raise_suspend();

	//udd_ack_wake_up();
	//otg_freeze_clock();

	cpu_irq_restore(flags);
}

void UDD_Detach(void)
{
	//printf("=> UDD_Detach\r\n");
	UOTGHS->UOTGHS_DEVCTRL |= UOTGHS_DEVCTRL_DETACH;
}

void UDD_InitEP( uint32_t ul_ep_nb, uint32_t ul_ep_cfg )
{

	ul_ep_nb = ul_ep_nb & 0xF; // EP range is 0..9, hence mask is 0xF.
	//printf("=> UDD_InitEP : init EP %d\r\n", ul_ep_nb);

	// Reset EP
	//UOTGHS->UOTGHS_DEVEPT = (UOTGHS_DEVEPT_EPRST0 << ul_ep_nb);
	// Configure EP
	UOTGHS->UOTGHS_DEVEPTCFG[ul_ep_nb] = ul_ep_cfg;
	// Allocate memory
	//udd_allocate_memory(ul_ep_nb);
	// Enable EP
//	UOTGHS->UOTGHS_DEVEPT |= (UOTGHS_DEVEPT_EPEN0 << ul_ep_nb);
udd_enable_endpoint(ul_ep_nb);
	if (!Is_udd_endpoint_configured(ul_ep_nb)) {
		//printf("=> UDD_InitEP : ############################## ERROR FAILED TO INIT EP %d\r\n", ul_ep_nb);
	}
}


void UDD_InitEndpoints(const uint32_t* eps_table, const uint32_t ul_eps_table_size)
{
	uint32_t ul_ep_nb ;





	for (ul_ep_nb = 1; ul_ep_nb < ul_eps_table_size; ul_ep_nb++)


/*void UDD_InitEndpoints(const uint32_t eps_table[])
{
	uint32_t ul_ep_nb ;


//printf("=> UDD_InitEndpoints : Taille tableau %d %d\r\n", sizeof(eps_table), (sizeof(eps_table) / sizeof(eps_table[0])));

	for (ul_ep_nb = 1; ul_ep_nb < sizeof(eps_table) / sizeof(eps_table[0]); ul_ep_nb++)*/
	{
    // Reset Endpoint Fifos
   /* UOTGHS->UOTGHS_DEVEPTISR[ul_EP].UDPHS_EPTCLRSTA = UDPHS_EPTCLRSTA_TOGGLESQ | UDPHS_EPTCLRSTA_FRCESTALL;
    UOTGHS->UOTGHS_DEVEPT = 1<<ul_EP;

		//UECONX = 1;
		//UECFG0X = pgm_read_byte(_initEndpoints+ul_EP);
        UOTGHS->UDPHS_EPT[ul_EP].UDPHS_EPTCFG = _initEndpoints[ul_EP];

        while( (signed int)UDPHS_EPTCFG_EPT_MAPD != (signed int)((UOTGHS->UDPHS_EPT[ul_EP].UDPHS_EPTCFG) & (unsigned int)UDPHS_EPTCFG_EPT_MAPD) )
        ;
        UOTGHS->UDPHS_EPT[ul_EP].UDPHS_EPTCTLENB = UDPHS_EPTCTLENB_EPT_ENABL;

        //		UECFG1X = EP_DOUBLE_64;
	}*/

		//printf("=> UDD_InitEndpoints : init EP %d\r\n", ul_ep_nb);


		// Reset EP
		//UOTGHS->UOTGHS_DEVEPT = (UOTGHS_DEVEPT_EPRST0 << ul_ep_nb);
		// Configure EP
		UOTGHS->UOTGHS_DEVEPTCFG[ul_ep_nb] = eps_table[ul_ep_nb];
		// Allocate memory
		//udd_allocate_memory(ul_ep_nb);
		// Enable EP
		//UOTGHS->UOTGHS_DEVEPT |= (UOTGHS_DEVEPT_EPEN0 << ul_ep_nb);
udd_enable_endpoint(ul_ep_nb);
		if (!Is_udd_endpoint_configured(ul_ep_nb)) {
			//printf("=> UDD_InitEP : ############################## ERROR FAILED TO INIT EP %d\r\n", ul_ep_nb);
		}
	}

}

void UDD_SetEP( uint32_t ep )
{
	ul_ep = ep & 0xF; // EP range is 0..9, hence mask is 0xF.
}

// Wait until ready to accept IN packet.
void UDD_WaitIN(void)
{
	//while (!(UEINTX & (1<<TXINI)));
	while (!(UOTGHS->UOTGHS_DEVEPTISR[ul_ep] & UOTGHS_DEVEPTISR_TXINI))
		;
}

void UDD_WaitOUT(void)
{
	//while (!(UEINTX & (1<<RXOUTI)));
	while (!(UOTGHS->UOTGHS_DEVEPTISR[ul_ep] & UOTGHS_DEVEPTISR_RXOUTI))
		;
}

// Send packet.
void UDD_ClearIN(void)
{
	//printf("=> UDD_ClearIN: sent %d bytes\r\n", ul_send_index);
	// UEINTX = ~(1<<TXINI);
	UOTGHS->UOTGHS_DEVEPTICR[ul_ep] = UOTGHS_DEVEPTICR_TXINIC;
	ul_send_index = 0;
}

void UDD_ClearOUT(void)
{
	// UEINTX = ~(1<<RXOUTI);
	UOTGHS->UOTGHS_DEVEPTICR[ul_ep] = UOTGHS_DEVEPTICR_RXOUTIC;
	ul_recv_index = 0;
}

// Wait for IN FIFO to be ready to accept data or OUT FIFO to receive data.
// Return true if new IN FIFO buffer available.
uint32_t UDD_WaitForINOrOUT(void)
{
	//while (!(UEINTX & ((1<<TXINI)|(1<<RXOUTI))));
	//return (UEINTX & (1<<RXOUTI)) == 0;
	while (!(UOTGHS->UOTGHS_DEVEPTISR[ul_ep] & (UOTGHS_DEVEPTISR_TXINI | UOTGHS_DEVEPTISR_RXOUTI)))
		;
	return ((UOTGHS->UOTGHS_DEVEPTISR[ul_ep] & UOTGHS_DEVEPTISR_RXOUTI) == 0);
}

uint32_t UDD_ReceivedSetupInt(void)
{
	return UOTGHS->UOTGHS_DEVEPTISR[ul_ep] & UOTGHS_DEVEPTISR_RXSTPI;
}

void UDD_ClearSetupInt(void)
{
	//UEINTX = ~((1<<RXSTPI) | (1<<RXOUTI) | (1<<TXINI));
	//UOTGHS->UOTGHS_DEVEPTICR[ul_ep] = (UOTGHS_DEVEPTICR_RXSTPIC | UOTGHS_DEVEPTICR_RXOUTIC | UOTGHS_DEVEPTICR_TXINIC);
	UOTGHS->UOTGHS_DEVEPTICR[ul_ep] = (UOTGHS_DEVEPTICR_RXSTPIC);
}

void UDD_Send8( uint8_t data )
{
	uint8_t *ptr_dest = (uint8_t *) &udd_get_endpoint_fifo_access8(ul_ep);

	printf("=> UDD_Send8 : ul_send_index=%d data=0x%x\r\n", ul_send_index, data);
	ptr_dest[ul_send_index++] = data;
}

uint8_t UDD_Recv8(void)
{
	uint8_t *ptr_dest = (uint8_t *) &udd_get_endpoint_fifo_access8(ul_ep);

	////printf("=> UDD_Recv8 : ul_recv_index=%d\r\n", ul_recv_index);
	return ptr_dest[ul_recv_index++];
}

void UDD_Recv(volatile uint8_t* data, uint32_t count)
{
	uint8_t *ptr_dest = (uint8_t *) &udd_get_endpoint_fifo_access8(ul_ep);

	while (count--)
		*data++ = ptr_dest[ul_recv_index++];
}

void UDD_Stall(void)
{
	//UECONX = (1<<STALLRQ) | (1<<EPEN);
	UOTGHS->UOTGHS_DEVEPT = (UOTGHS_DEVEPT_EPEN0 << ul_ep);
	UOTGHS->UOTGHS_DEVEPTIER[ul_ep] = UOTGHS_DEVEPTIER_STALLRQS;
}


uint32_t UDD_FifoByteCount(void)
{
	return ((UOTGHS->UOTGHS_DEVEPTISR[ul_ep] & UOTGHS_DEVEPTISR_BYCT_Msk) >> UOTGHS_DEVEPTISR_BYCT_Pos);
}

void UDD_ReleaseRX(void)
{
/*	UEINTX = 0x6B;	// FIFOCON=0 NAKINI=1 RWAL=1 NAKOUTI=0 RXSTPI=1 RXOUTI=0 STALLEDI=1 TXINI=1
	clear fifocon = send and switch bank
	nakouti a clearer
	rxouti/killbank a clearer*/

	//puts("=> UDD_ReleaseRX\r\n");
	UOTGHS->UOTGHS_DEVEPTICR[ul_ep] = (UOTGHS_DEVEPTICR_NAKOUTIC | UOTGHS_DEVEPTICR_RXOUTIC);
	UOTGHS->UOTGHS_DEVEPTIDR[ul_ep] = UOTGHS_DEVEPTIDR_FIFOCONC;
	ul_recv_index = 0;
}

void UDD_ReleaseTX(void)
{
/*	UEINTX = 0x3A;	// FIFOCON=0 NAKINI=0 RWAL=1 NAKOUTI=1 RXSTPI=1 RXOUTI=0 STALLEDI=1 TXINI=0
	clear fifocon = send and switch bank
	nakini a clearer
	rxouti/killbank a clearer
	txini a clearer*/

	//puts("=> UDD_ReleaseTX\r\n");
	UOTGHS->UOTGHS_DEVEPTICR[ul_ep] = (UOTGHS_DEVEPTICR_NAKINIC | UOTGHS_DEVEPTICR_RXOUTIC | UOTGHS_DEVEPTICR_TXINIC);
	UOTGHS->UOTGHS_DEVEPTIDR[ul_ep] = UOTGHS_DEVEPTIDR_FIFOCONC;
	ul_send_index = 0;
}

// Return true if the current bank is not full.
uint32_t UDD_ReadWriteAllowed(void)
{
	return (UOTGHS->UOTGHS_DEVEPTISR[ul_ep] & UOTGHS_DEVEPTISR_RWALL);
}

void UDD_SetAddress(uint32_t addr)
{
	//printf("=> UDD_SetAddress : setting address to %d\r\n", addr);
	udd_configure_address(addr);
	udd_enable_address();
}

uint32_t UDD_GetFrameNumber(void)
{
	return udd_frame_number();
}

#endif /* SAM3XA_SERIES */
