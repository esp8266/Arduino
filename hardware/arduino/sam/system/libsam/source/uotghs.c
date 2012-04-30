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

#if SAM3XA_SERIES

static void (*gpf_isr)(void)=(0UL);

uint32_t ul_ep = 0;

void UDD_SetStack(void (*pf_isr)(void))
{
	gpf_isr = pf_isr;
}

void UOTGHS_Handler( void )
{
	if (gpf_isr)
		gpf_isr();
}

void UDD_SetEP( uint32_t ep )
{
	ul_ep = ep;
}

void UDD_InitEndpoints(const uint32_t* eps_table)
{
	uint32_t ul_ep_nb ;

	for (ul_ep_nb = 1; ul_ep_nb < sizeof(eps_table); ul_ep_nb++)
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

		// Reset EP
		UOTGHS->UOTGHS_DEVEPT = (UOTGHS_DEVEPT_EPRST0 << ul_ep_nb);
		// Configure EP
		UOTGHS->UOTGHS_DEVEPTCFG[ul_ep_nb] = eps_table[ul_ep_nb];
		// Enable EP
		UOTGHS->UOTGHS_DEVEPT = (UOTGHS_DEVEPT_EPEN0 << ul_ep_nb);
	}

}

void UDD_InitEP( uint32_t ul_ep_nb, uint32_t ul_ep_cfg )
{
	// Reset EP
	UOTGHS->UOTGHS_DEVEPT = (UOTGHS_DEVEPT_EPRST0 << ul_ep_nb);
	// Configure EP
	UOTGHS->UOTGHS_DEVEPTCFG[ul_ep_nb] = ul_ep_cfg;
	// Enable EP
	UOTGHS->UOTGHS_DEVEPT = (UOTGHS_DEVEPT_EPEN0 << ul_ep_nb);
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
	pmc_set_fast_startup_input(PMC_FSMR_USBAL);

	// Enable USB
	UOTGHS->UOTGHS_CTRL |= UOTGHS_CTRL_USBE;

	// Automatic mode speed for device
	UOTGHS->UOTGHS_DEVCTRL &= ~UOTGHS_DEVCTRL_SPDCONF_Msk;  // Normal mode

	UOTGHS->UOTGHS_DEVCTRL &= ~( UOTGHS_DEVCTRL_LS | UOTGHS_DEVCTRL_TSTJ | UOTGHS_DEVCTRL_TSTK |
								UOTGHS_DEVCTRL_TSTPCKT | UOTGHS_DEVCTRL_OPMODE2 ); // Normal mode

	UOTGHS->UOTGHS_DEVCTRL = 0;
	UOTGHS->UOTGHS_HSTCTRL = 0;

	// Enable OTG pad
	UOTGHS->UOTGHS_CTRL |= UOTGHS_CTRL_OTGPADE;

	// Enable clock OTG pad
	UOTGHS->UOTGHS_CTRL &= ~UOTGHS_CTRL_FRZCLK;

	// Usb disable
	UOTGHS->UOTGHS_CTRL &= ~UOTGHS_CTRL_USBE;
	UOTGHS->UOTGHS_CTRL &= ~UOTGHS_CTRL_OTGPADE;
	UOTGHS->UOTGHS_CTRL |= UOTGHS_CTRL_FRZCLK;

	// Usb enable
	UOTGHS->UOTGHS_CTRL |= UOTGHS_CTRL_USBE;
	UOTGHS->UOTGHS_CTRL |= UOTGHS_CTRL_OTGPADE;
	UOTGHS->UOTGHS_CTRL &= ~UOTGHS_CTRL_FRZCLK;

	// Usb select device mode
	UOTGHS->UOTGHS_CTRL &= ~UOTGHS_CTRL_UIDE;
	UOTGHS->UOTGHS_CTRL |= UOTGHS_CTRL_UIMOD_Device;

	// Device is in the Attached state
	//  deviceState = USBD_STATE_SUSPENDED;
	//  previousDeviceState = USBD_STATE_POWERED;

	// Enable USB and clear all other bits
	//UOTGHS->UOTGHS_DEVCTRL |= UOTGHS_CTRL_USBE;
	//UOTGHS->UOTGHS_DEVCTRL = UOTGHS_CTRL_USBE;

	// Configure the pull-up on D+ and disconnect it
	UDD_Detach();

	// Clear General IT
	UOTGHS->UOTGHS_SCR = (UOTGHS_SCR_IDTIC|UOTGHS_SCR_VBUSTIC|UOTGHS_SCR_SRPIC|UOTGHS_SCR_VBERRIC|UOTGHS_SCR_BCERRIC|UOTGHS_SCR_ROLEEXIC|UOTGHS_SCR_HNPERRIC|UOTGHS_SCR_STOIC|UOTGHS_SCR_VBUSRQC);

	// Clear OTG Device IT
	UOTGHS->UOTGHS_DEVICR = (UOTGHS_DEVICR_SUSPC|UOTGHS_DEVICR_MSOFC|UOTGHS_DEVICR_SOFC|UOTGHS_DEVICR_EORSTC|UOTGHS_DEVICR_WAKEUPC|UOTGHS_DEVICR_EORSMC|UOTGHS_DEVICR_UPRSMC);

	// Clear OTG Host IT
	UOTGHS->UOTGHS_HSTICR = (UOTGHS_HSTICR_DCONNIC|UOTGHS_HSTICR_DDISCIC|UOTGHS_HSTICR_RSTIC|UOTGHS_HSTICR_RSMEDIC|UOTGHS_HSTICR_RXRSMIC|UOTGHS_HSTICR_HSOFIC|UOTGHS_HSTICR_HWUPIC);

	// Reset all Endpoints Fifos
	UOTGHS->UOTGHS_DEVEPT |= (UOTGHS_DEVEPT_EPRST0|UOTGHS_DEVEPT_EPRST1|UOTGHS_DEVEPT_EPRST2|UOTGHS_DEVEPT_EPRST3|UOTGHS_DEVEPT_EPRST4|
	UOTGHS_DEVEPT_EPRST5|UOTGHS_DEVEPT_EPRST6|UOTGHS_DEVEPT_EPRST7|UOTGHS_DEVEPT_EPRST8);
	UOTGHS->UOTGHS_DEVEPT &= ~(UOTGHS_DEVEPT_EPRST0|UOTGHS_DEVEPT_EPRST1|UOTGHS_DEVEPT_EPRST2|UOTGHS_DEVEPT_EPRST3|UOTGHS_DEVEPT_EPRST4|
	UOTGHS_DEVEPT_EPRST5|UOTGHS_DEVEPT_EPRST6|UOTGHS_DEVEPT_EPRST7|UOTGHS_DEVEPT_EPRST8);

	// Disable all endpoints
	UOTGHS->UOTGHS_DEVEPT &= ~(UOTGHS_DEVEPT_EPEN0|UOTGHS_DEVEPT_EPEN1|UOTGHS_DEVEPT_EPEN2|UOTGHS_DEVEPT_EPEN3|UOTGHS_DEVEPT_EPEN4|
	UOTGHS_DEVEPT_EPEN5|UOTGHS_DEVEPT_EPEN6|UOTGHS_DEVEPT_EPEN7|UOTGHS_DEVEPT_EPEN8);

	// Device is in the Attached state
	//  deviceState = USBD_STATE_SUSPENDED;
	//  previousDeviceState = USBD_STATE_POWERED;

	// Automatic mode speed for device
	UOTGHS->UOTGHS_DEVCTRL &= ~UOTGHS_DEVCTRL_SPDCONF_Msk;
	// Force Full Speed mode for device
	//UOTGHS->UOTGHS_DEVCTRL = UOTGHS_DEVCTRL_SPDCONF_FORCED_FS;
	// Force High Speed mode for device
	//UOTGHS->UOTGHS_DEVCTRL = UOTGHS_DEVCTRL_SPDCONF_HIGH_SPEED;

	UOTGHS->UOTGHS_DEVCTRL &= ~(UOTGHS_DEVCTRL_LS|UOTGHS_DEVCTRL_TSTJ| UOTGHS_DEVCTRL_TSTK|UOTGHS_DEVCTRL_TSTPCKT|UOTGHS_DEVCTRL_OPMODE2) ;

	// Enable USB macro
	UOTGHS->UOTGHS_CTRL |= UOTGHS_CTRL_USBE;

	// Enable the UID pin select
	UOTGHS->UOTGHS_CTRL |= UOTGHS_CTRL_UIDE;

	// Enable OTG pad
	UOTGHS->UOTGHS_CTRL |= UOTGHS_CTRL_OTGPADE;

	// Enable clock OTG pad
	UOTGHS->UOTGHS_CTRL &= ~UOTGHS_CTRL_FRZCLK;

	// With OR without DMA !!!
	// Initialization of DMA
	for( ul=1; ul<= UOTGHSDEVDMA_NUMBER ; ul++ )
	{
		// RESET endpoint canal DMA:
		// DMA stop channel command
		UOTGHS->UOTGHS_DEVDMA[ul].UOTGHS_DEVDMACONTROL = 0;  // STOP command

		// Disable endpoint
		UOTGHS->UOTGHS_DEVEPTIDR[ul] = (UOTGHS_DEVEPTIDR_TXINEC|UOTGHS_DEVEPTIDR_RXOUTEC|UOTGHS_DEVEPTIDR_RXSTPEC|UOTGHS_DEVEPTIDR_UNDERFEC|UOTGHS_DEVEPTIDR_NAKOUTEC|
		UOTGHS_DEVEPTIDR_HBISOINERREC|UOTGHS_DEVEPTIDR_NAKINEC|UOTGHS_DEVEPTIDR_HBISOFLUSHEC|UOTGHS_DEVEPTIDR_OVERFEC|UOTGHS_DEVEPTIDR_STALLEDEC|
		UOTGHS_DEVEPTIDR_CRCERREC|UOTGHS_DEVEPTIDR_SHORTPACKETEC|UOTGHS_DEVEPTIDR_MDATEC|UOTGHS_DEVEPTIDR_DATAXEC|UOTGHS_DEVEPTIDR_ERRORTRANSEC|
		UOTGHS_DEVEPTIDR_NBUSYBKEC|UOTGHS_DEVEPTIDR_FIFOCONC|UOTGHS_DEVEPTIDR_EPDISHDMAC|UOTGHS_DEVEPTIDR_NYETDISC|UOTGHS_DEVEPTIDR_STALLRQC);

		// Reset endpoint config
		UOTGHS->UOTGHS_DEVEPTCFG[ul] = 0UL;

		// Reset DMA channel (Buff count and Control field)
		UOTGHS->UOTGHS_DEVDMA[ul].UOTGHS_DEVDMACONTROL = 0x02UL;  // NON STOP command

		// Reset DMA channel 0 (STOP)
		UOTGHS->UOTGHS_DEVDMA[ul].UOTGHS_DEVDMACONTROL = 0UL;  // STOP command

		// Clear DMA channel status (read the register to clear it)
		UOTGHS->UOTGHS_DEVDMA[ul].UOTGHS_DEVDMASTATUS = UOTGHS->UOTGHS_DEVDMA[ul].UOTGHS_DEVDMASTATUS;
	}

	UOTGHS->UOTGHS_CTRL |= UOTGHS_CTRL_VBUSTE;
	UOTGHS->UOTGHS_DEVIER = UOTGHS_DEVIER_WAKEUPES;

	return 0UL ;
}

void UDD_Attach(void)
{
  UOTGHS->UOTGHS_DEVCTRL &= ~(unsigned int)UOTGHS_DEVCTRL_DETACH;
}

void UDD_Detach(void)
{
  UOTGHS->UOTGHS_DEVCTRL |= UOTGHS_DEVCTRL_DETACH;
}

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

uint32_t ul_send_index = 0;
uint32_t ul_rcv_index = 0;

void UDD_ClearIN(void)
{
	// UEINTX = ~(1<<TXINI);
	UOTGHS->UOTGHS_DEVEPTICR[ul_ep] = UOTGHS_DEVEPTICR_TXINIC;
	ul_send_index = 0;
}

void UDD_ClearOUT(void)
{
	// UEINTX = ~(1<<RXOUTI);
	UOTGHS->UOTGHS_DEVEPTICR[ul_ep] = UOTGHS_DEVEPTICR_RXOUTIC;
	ul_rcv_index = 0;
}

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
	UOTGHS->UOTGHS_DEVEPTICR[ul_ep] = (UOTGHS_DEVEPTICR_RXSTPIC | UOTGHS_DEVEPTICR_RXOUTIC | UOTGHS_DEVEPTICR_TXINIC);
}

void UDD_Send8( uint8_t data )
{
	uint8_t *ptr_dest = (uint8_t *) &udd_get_endpoint_fifo_access8(ul_ep);

	ptr_dest[ul_send_index++] = data;
}

uint8_t UDD_Recv8(void)
{
	uint8_t *ptr_dest = (uint8_t *) &udd_get_endpoint_fifo_access8(ul_ep);

	return ptr_dest[ul_rcv_index++];
}

void UDD_Recv(volatile uint8_t* data, uint32_t count)
{
	uint8_t *ptr_dest = (uint8_t *) &udd_get_endpoint_fifo_access8(ul_ep);

	while (count--)
		*data++ = ptr_dest[ul_rcv_index++];
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

	UOTGHS->UOTGHS_DEVEPTICR[ul_ep] = (UOTGHS_DEVEPTICR_NAKOUTIC | UOTGHS_DEVEPTICR_RXOUTIC);
	UOTGHS->UOTGHS_DEVEPTIDR[ul_ep] = UOTGHS_DEVEPTIDR_FIFOCONC;
}

void UDD_ReleaseTX(void)
{
/*	UEINTX = 0x3A;	// FIFOCON=0 NAKINI=0 RWAL=1 NAKOUTI=1 RXSTPI=1 RXOUTI=0 STALLEDI=1 TXINI=0
	clear fifocon = send and switch bank
	nakini a clearer
	rxouti/killbank a clearer
	txini a clearer*/

	UOTGHS->UOTGHS_DEVEPTICR[ul_ep] = (UOTGHS_DEVEPTICR_NAKINIC | UOTGHS_DEVEPTICR_RXOUTIC | UOTGHS_DEVEPTICR_TXINIC);
	UOTGHS->UOTGHS_DEVEPTIDR[ul_ep] = UOTGHS_DEVEPTIDR_FIFOCONC;
}

uint32_t UDD_ReadWriteAllowed(void)
{
	return (UOTGHS->UOTGHS_DEVEPTISR[ul_ep] & UOTGHS_DEVEPTISR_RWALL);
}

void UDD_SetAddress(uint32_t addr)
{
	udd_configure_address(addr);
	udd_enable_address();
}

uint32_t UDD_GetFrameNumber(void)
{
	return udd_frame_number();
}

#endif /* SAM3XA_SERIES */
