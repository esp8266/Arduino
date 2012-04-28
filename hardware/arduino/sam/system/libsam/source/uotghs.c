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

void USBD_InitEndpoints( uint32_t* puc_EndPoints, uint32_t ul_EndPoints )
{
  uint32_t ul_EP ;

	for (ul_EP = 1; ul_EP < sizeof(_initEndpoints); ul_EP++)
	{
    // Reset Endpoint Fifos
    UOTGHS->UOTGHS_DEVEPTISR[ul_EP].UDPHS_EPTCLRSTA = UDPHS_EPTCLRSTA_TOGGLESQ | UDPHS_EPTCLRSTA_FRCESTALL;
    UOTGHS->UOTGHS_DEVEPT = 1<<ul_EP;

		//UECONX = 1;
		//UECFG0X = pgm_read_byte(_initEndpoints+ul_EP);
        UOTGHS->UDPHS_EPT[ul_EP].UDPHS_EPTCFG = _initEndpoints[ul_EP];

        while( (signed int)UDPHS_EPTCFG_EPT_MAPD != (signed int)((UOTGHS->UDPHS_EPT[ul_EP].UDPHS_EPTCFG) & (unsigned int)UDPHS_EPTCFG_EPT_MAPD) )
        ;
        UOTGHS->UDPHS_EPT[ul_EP].UDPHS_EPTCTLENB = UDPHS_EPTCTLENB_EPT_ENABL;

        //		UECFG1X = EP_DOUBLE_64;
	}
}

uint32_t USBD_Init(void)
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

  // Enable USB macro
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

  // Usb select_device
  UOTGHS->UOTGHS_CTRL &= ~UOTGHS_CTRL_UIDE;
  UOTGHS->UOTGHS_CTRL |= UOTGHS_CTRL_UIMOD_Device;

  // Device is in the Attached state
//  deviceState = USBD_STATE_SUSPENDED;
//  previousDeviceState = USBD_STATE_POWERED;

  // Enable USB macro and clear all other bits
  UOTGHS->UOTGHS_DEVCTRL |= UOTGHS_CTRL_USBE;
  UOTGHS->UOTGHS_DEVCTRL = UOTGHS_CTRL_USBE;

  // Configure the pull-up on D+ and disconnect it
  USBD_Detach();

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
  UOTGHS->UOTGHS_DEVCTRL |= UOTGHS_CTRL_USBE;

  // Enable the UID pin select
  UOTGHS->UOTGHS_DEVCTRL |= UOTGHS_CTRL_UIDE;

  // Enable OTG pad
  UOTGHS->UOTGHS_DEVCTRL |= UOTGHS_CTRL_OTGPADE;

  // Enable clock OTG pad
  UOTGHS->UOTGHS_DEVCTRL &= ~UOTGHS_CTRL_FRZCLK;

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

  UOTGHS->UOTGHS_DEVCTRL |= UOTGHS_CTRL_VBUSTE;
  UOTGHS->UOTGHS_DEVIER = UOTGHS_DEVIER_WAKEUPES;

  return 0UL ;
}

void USBD_Attach(void)
{
  UOTGHS->UOTGHS_DEVCTRL &= ~(unsigned int)UOTGHS_DEVCTRL_DETACH;
}

void USBD_Detach(void)
{
  UOTGHS->UOTGHS_DEVCTRL |= UOTGHS_DEVCTRL_DETACH;
}


#endif /* SAM3XA_SERIES */
