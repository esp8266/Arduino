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

#if 0 //SAM3U_SERIES

#include "USB_device.h"
#include "udphs.h"

/// Max size of the FMA FIFO
#define EPT_VIRTUAL_SIZE  (16384u)
#define SHIFT_INTERUPT    (8u)

int _cmark;
int _cend;

// Global variable for endpoint number
unsigned int NumEndpoint=0;


void USBD_WaitIN(void)
{
//	while (!(UEINTX & (1<<TXINI)));
  while (!(UDPHS->UDPHS_EPT[0].UDPHS_EPTSTA & UDPHS_EPTSTA_TX_PK_RDY));
}

void USBD_WaitOUT(void)
{
//	while (!(UEINTX & (1<<RXOUTI)))
//		;
  // Waiting for Status stage
  while (UDPHS_EPTSTA_RX_BK_RDY != (UDPHS->UDPHS_EPT[NumEndpoint].UDPHS_EPTSTA & UDPHS_EPTSTA_RX_BK_RDY));
}

void USBD_ClearIN(void)
{
//	UEINTX = ~(1<<TXINI);
  UDPHS->UDPHS_EPT[NumEndpoint].UDPHS_EPTCLRSTA = UDPHS_EPTCLRSTA_TX_COMPLT;
}

void USBD_ClearOUT(void)
{
//	UEINTX = ~(1<<RXOUTI);
  UDPHS->UDPHS_EPT[NumEndpoint].UDPHS_EPTCLRSTA = UDPHS_EPTCLRSTA_RX_BK_RDY;
}

uint8_t USBD_WaitForINOrOUT(void)
{
//	while (!(UEINTX & ((1<<TXINI)|(1<<RXOUTI))))
//		;
//	return (UEINTX & (1<<RXOUTI)) == 0;
  while (!(UDPHS->UDPHS_EPT[NumEndpoint].UDPHS_EPTSTA & (UDPHS_EPTSTA_RX_BK_RDY | UDPHS_EPTSTA_TX_PK_RDY)));
  return (UDPHS->UDPHS_EPT[NumEndpoint].UDPHS_EPTSTA & UDPHS_EPTSTA_RX_BK_RDY) == 0;
}

void USBD_ClearRxFlag(unsigned char bEndpoint)
{
  UDPHS->UDPHS_EPT[NumEndpoint].UDPHS_EPTCLRSTA = UDPHS_EPTCLRSTA_RX_BK_RDY;
}

void USBD_Stall(void)
{
//	UECONX = (1<<STALLRQ) | (1<<EPEN);
  UDPHS->UDPHS_EPT[NumEndpoint].UDPHS_EPTSETSTA = UDPHS_EPTSETSTA_FRCESTALL;
}

uint8_t USBD_Stalled(void)
{
//	return UEINTX & (1<<STALLEDI);
  // Check if the data has been STALLed
  return ( UDPHS_EPTSTA_FRCESTALL == (UDPHS->UDPHS_EPT[NumEndpoint].UDPHS_EPTSTA & UDPHS_EPTSTA_FRCESTALL));
}

uint8_t USBD_ReceivedSetupInt(void)
{
//	return UEINTX & (1<<RXSTPI);
  return ( UDPHS_EPTSTA_RX_SETUP == (UDPHS->UDPHS_EPT[NumEndpoint].UDPHS_EPTSTA & UDPHS_EPTSTA_RX_SETUP) );
}

void USBD_ClearSetupInt(void)
{
//	UEINTX = ~((1<<RXSTPI) | (1<<RXOUTI) | (1<<TXINI));
  UDPHS->UDPHS_EPT[NumEndpoint].UDPHS_EPTCLRSTA = UDPHS_EPTSTA_RX_SETUP | UDPHS_EPTCLRSTA_RX_BK_RDY | UDPHS_EPTCLRSTA_TX_COMPLT;
}

uint8_t USBD_ReadWriteAllowed(void)
{
	//return UEINTX & (1<<RWAL);
  return 1;
}

void USBD_SetEP(uint8_t ep)
{
//	UENUM = ep;
	NumEndpoint = ep & 7;
}

uint16_t USBD_FifoByteCount(void)
{
//	return UEBCLX;
  // SAM3X
  //return ((UOTGHS->UOTGHS_DEVEPTISR[ep] & UOTGHS_DEVEPTISR_BYCT_Msk) >> UOTGHS_DEVEPTISR_BYCT_Pos);
  // SAM3U //AT91C_UDPHS_BYTE_COUNT (0x7FF << 20)
  return ((UDPHS->UDPHS_EPT[NumEndpoint].UDPHS_EPTSTA & UDPHS_EPTSTA_BYTE_COUNT_Msk) >> UDPHS_EPTSTA_BYTE_COUNT_Pos);
}

uint8_t USBD_FifoFree(void)
{
//	return UEINTX & (1<<FIFOCON);
  return( 0 != (UDPHS->UDPHS_EPT[NumEndpoint].UDPHS_EPTSTA & UDPHS_EPTSTA_TX_PK_RDY ));
}

void USBD_ReleaseRX(void)
{
	UEINTX = 0x6B;	// FIFOCON=0 NAKINI=1 RWAL=1 NAKOUTI=0 RXSTPI=1 RXOUTI=0 STALLEDI=1 TXINI=1
}

void USBD_ReleaseTX()
{
	UEINTX = 0x3A;	// FIFOCON=0 NAKINI=0 RWAL=1 NAKOUTI=1 RXSTPI=1 RXOUTI=0 STALLEDI=1 TXINI=0
}

uint8_t USBD_FrameNumber(void)
{
	return UDFNUML;
}

uint8_t USBD_GetConfiguration(void)
{
	return _usbConfiguration;
}



void USBD_Recv(volatile uint8_t* data, uint8_t count)
{
  uint8_t     *pFifo;

  pFifo = (uint8_t*)((uint32_t *)UDPHS_RAM_ADDR + (EPT_VIRTUAL_SIZE * NumEndpoint));

  while (count--)
  {
    *data++ = pFifo[0];
  }

//	RXLED1;					// light the RX LED
//	RxLEDPulse = TX_RX_LED_PULSE_MS;
}

uint8_t USBD_Recv8(void)
{
    uint8_t     *pFifo;

//	RXLED1;					// light the RX LED
//	RxLEDPulse = TX_RX_LED_PULSE_MS;

    pFifo = (uint8_t*)((uint32_t *)UDPHS_RAM_ADDR + (EPT_VIRTUAL_SIZE * NumEndpoint));

//	return UEDATX;
    return (pFifo[0]);
}

void USBD_Send8(uint8_t d)
{
    uint8_t     *pFifo;
    pFifo = (uint8_t*)((uint32_t *)UDPHS_RAM_ADDR + (EPT_VIRTUAL_SIZE * NumEndpoint));
//	UEDATX = d;
    pFifo[0] =d;
}

//	Blocking Send of data to an endpoint
int USBD_Send(uint8_t ep, const void* d, int len)
{
	if (!_usbConfiguration)
		return -1;

	int r = len;
	const uint8_t* data = (const uint8_t*)d;
	uint8_t zero = ep & TRANSFER_ZERO;
	uint8_t timeout = 250;		// 250ms timeout on send? TODO
	while (len)
	{
		uint8_t n = USB_SendSpace(ep);
		if (n == 0)
		{
			if (!(--timeout))
				return -1;
			delay(1);
			continue;
		}

		if (n > len)
			n = len;
		len -= n;
		{
			SetEP(ep);
			if (ep & TRANSFER_ZERO)
			{
				while (n--)
					Send8(0);
			}
			else if (ep & TRANSFER_PGM)
			{
				while (n--)
					Send8(*data++);
			}
			else
			{
				while (n--)
					Send8(*data++);
			}
//			if (!ReadWriteAllowed() || ((len == 0) && (ep & TRANSFER_RELEASE)))	// Release full buffer
//				ReleaseTX();
		}
	}
	TXLED1;					// light the TX LED
	TxLEDPulse = TX_RX_LED_PULSE_MS;
	return r;
}


//	Space in send EP
uint8_t USBD_SendSpace(uint8_t ep)
{
	SetEP(ep);
	if (!ReadWriteAllowed())
  {
		return 0;
  }

	return 64 - FifoByteCount();
}

//	Number of bytes, assumes a rx endpoint
uint8_t USBD_Available(uint8_t ep)
{
	SetEP(ep);

	return FifoByteCount();
}

void USBD_InitEP(uint8_t index, uint8_t type, uint8_t size)
{
	UENUM = index;
	UECONX = 1;
	UECFG0X = type;
	UECFG1X = size;
}


void USBD_InitEndpoints(void)
{
	for (uint8_t i = 1; i < sizeof(_initEndpoints); i++)
	{
        // Reset Endpoint Fifos
        UDPHS->UDPHS_EPT[i].UDPHS_EPTCLRSTA = UDPHS_EPTCLRSTA_TOGGLESQ | UDPHS_EPTCLRSTA_FRCESTALL;
        UDPHS->UDPHS_EPTRST = 1<<i;

		//UECONX = 1;
		//UECFG0X = pgm_read_byte(_initEndpoints+i);
        UDPHS->UDPHS_EPT[i].UDPHS_EPTCFG = _initEndpoints[i];

        while( (signed int)UDPHS_EPTCFG_EPT_MAPD != (signed int)((UDPHS->UDPHS_EPT[i].UDPHS_EPTCFG) & (unsigned int)UDPHS_EPTCFG_EPT_MAPD) )
        ;
        UDPHS->UDPHS_EPT[i].UDPHS_EPTCTLENB = UDPHS_EPTCTLENB_EPT_ENABL;

        //		UECFG1X = EP_DOUBLE_64;
	}
///\//	UERST = 0x7E;	// And reset them
///\//	UERST = 0;
}

void USBD_InitControl(int end)
{
	SetEP(0);
  UDPHS->UDPHS_EPT[0].UDPHS_EPTCFG = _initEndpoints[0];

  while( (signed int)UDPHS_EPTCFG_EPT_MAPD != (signed int)((UDPHS->UDPHS_EPT[0].UDPHS_EPTCFG) & (unsigned int)UDPHS_EPTCFG_EPT_MAPD) )
  ;

  UDPHS->UDPHS_EPT[0].UDPHS_EPTCTLENB = UDPHS_EPTCTLENB_RX_BK_RDY
                                       | UDPHS_EPTCTLENB_RX_SETUP
                                       | UDPHS_EPTCTLENB_EPT_ENABL;

	_cmark = 0;
	_cend = end;
}

void UDPHS_Handler( void )
{
    unsigned int  status;
    unsigned char numIT;

    // Get interrupts status
    status = UDPHS->UDPHS_INTSTA & UDPHS->UDPHS_IEN;

    // Handle all UDPHS interrupts
    while (status != 0) {

        //	Start of Frame - happens every millisecond so we use it for TX and RX LED one-shot timing, too
        if ((status & UDPHS_IEN_INT_SOF) != 0) {

#ifdef CDC_ENABLED
            USB_Flush(CDC_TX);				// Send a tx frame if found
#endif

            // check whether the one-shot period has elapsed.  if so, turn off the LED
            if (TxLEDPulse && !(--TxLEDPulse))
                TXLED0;
            if (RxLEDPulse && !(--RxLEDPulse))
                RXLED0;

            // Acknowledge interrupt
            UDPHS->UDPHS_CLRINT = UDPHS_CLRINT_INT_SOF;
            status &= ~UDPHS_IEN_INT_SOF;
        }
        // Suspend
        // This interrupt is always treated last (hence the '==')
        else if (status == UDPHS_IEN_DET_SUSPD) {

            //UDPHS_DisableBIAS();

            // Enable wakeup
            UDPHS->UDPHS_IEN |= UDPHS_IEN_WAKE_UP | UDPHS_IEN_ENDOFRSM;
            UDPHS->UDPHS_IEN &= ~UDPHS_IEN_DET_SUSPD;

            // Acknowledge interrupt
            UDPHS->UDPHS_CLRINT = UDPHS_CLRINT_DET_SUSPD | UDPHS_CLRINT_WAKE_UP;

            //UDPHS_DisableUsbClock();

        }
        // Resume
        else if( ((status & UDPHS_IEN_WAKE_UP) != 0)      // line activity
              || ((status & UDPHS_IEN_ENDOFRSM) != 0))  { // pc wakeup
            {

                //UDPHS_EnableUsbClock();
                //UDPHS_EnableBIAS();

                UDPHS->UDPHS_CLRINT = UDPHS_CLRINT_WAKE_UP | UDPHS_CLRINT_ENDOFRSM
                		| UDPHS_CLRINT_DET_SUSPD;

                UDPHS->UDPHS_IEN |= UDPHS_IEN_ENDOFRSM | UDPHS_IEN_DET_SUSPD;
                UDPHS->UDPHS_CLRINT = UDPHS_CLRINT_WAKE_UP | UDPHS_CLRINT_ENDOFRSM;
                UDPHS->UDPHS_IEN &= ~UDPHS_IEN_WAKE_UP;
            }
        }
        // End of Reset
        else if ((status & UDPHS_IEN_ENDRESET) == UDPHS_IEN_ENDRESET) {

        	InitControl(0);	// init ep0
            _usbConfiguration = 0;			// not configured yet
            //UEIENX = 1 << RXSTPE;			// Enable interrupts for ep0

            //UDPHS_ResetEndpoints();
            //UDPHS_DisableEndpoints();
            //USBD_ConfigureEndpoint(0);
            UDPHS->UDPHS_IEN |= (1<<SHIFT_INTERUPT<<0);

            // Flush and enable the Suspend interrupt
            UDPHS->UDPHS_CLRINT = UDPHS_CLRINT_WAKE_UP | UDPHS_CLRINT_DET_SUSPD;

            //// Enable the Start Of Frame (SOF) interrupt if needed
            UDPHS->UDPHS_IEN |= UDPHS_IEN_INT_SOF;

            // Acknowledge end of bus reset interrupt
            UDPHS->UDPHS_CLRINT = UDPHS_CLRINT_ENDRESET;

            UDPHS->UDPHS_IEN |= UDPHS_IEN_DET_SUSPD;
        }
        // Handle upstream resume interrupt
        else if (status & UDPHS_IEN_UPSTR_RES) {

            // - Acknowledge the IT
            UDPHS->UDPHS_CLRINT = UDPHS_CLRINT_UPSTR_RES;
        }
        // Endpoint interrupts
        else {
            // Handle endpoint interrupts
            for (numIT = 0; numIT < NUM_IT_MAX; numIT++) {

                if ((status & (1 << SHIFT_INTERUPT << numIT)) != 0) {
                	USB_ISR();
                    //EndpointHandler(numIT); // TODO: interrupt for bulk
                }
            }
        }
        // Retrieve new interrupt status
        status = UDPHS->UDPHS_INTSTA & UDPHS->UDPHS_IEN;
    }
}

void USBD_Attach( void )
{/*
	_usbConfiguration = 0;

	//UHWCON = 0x01;						// power internal reg
	//USBCON = (1<<USBE)|(1<<FRZCLK);		// clock frozen, usb enabled
	//PLLCSR = 0x12;						// Need 16 MHz xtal
	//while (!(PLLCSR & (1<<PLOCK)))		// wait for lock pll
	//	;
    PMC->PMC_PCER = (1 << ID_UDPHS);
    // Enable 480MHZ
    //AT91C_BASE_CKGR->CKGR_UCKR |= (AT91C_CKGR_PLLCOUNT & (3 << 20)) | AT91C_CKGR_UPLLEN;
    CKGR->CKGR_UCKR |= ((0xf << 20) & (3 << 20)) | AT91C_CKGR_UPLLEN;
    // Wait until UTMI PLL is locked
    while ((PMC->PMC_SR & PMC_LOCKU) == 0);

    // Reset and enable IP UDPHS
    UDPHS->UDPHS_CTRL &= ~UDPHS_CTRL_EN_UDPHS;
    UDPHS->UDPHS_CTRL |= UDPHS_CTRL_EN_UDPHS;

	//USBCON = ((1<<USBE)|(1<<OTGPADE));	// start USB clock
    UDPHS->UDPHS_IEN = 0;
    UDPHS->UDPHS_CLRINT = UDPHS_CLRINT_UPSTR_RES
                                   | UDPHS_CLRINT_ENDOFRSM
                                   | UDPHS_CLRINT_WAKE_UP
                                   | UDPHS_CLRINT_ENDRESET
                                   | UDPHS_CLRINT_INT_SOF
                                   | UDPHS_CLRINT_MICRO_SOF
                                   | UDPHS_CLRINT_DET_SUSPD;

    // Enable interrupts for EOR (End of Reset), wake up and SOF (start of frame)
    //UDIEN = (1<<EORSTE)|(1<<SOFE);
    UDPHS->UDPHS_IEN = UDPHS_IEN_ENDOFRSM
                                | UDPHS_IEN_WAKE_UP
                                | UDPHS_IEN_DET_SUSPD;

	// enable attach resistor
	//UDCON = 0;
    UDPHS->UDPHS_CTRL &= ~UDPHS_CTRL_DETACH;   // Pull Up on DP
    UDPHS->UDPHS_CTRL |= UDPHS_CTRL_PULLD_DIS; // Disable Pull Down

	TX_RX_LED_INIT;
*/}

void USBD_Detach( void )
{
  UDPHS->UDPHS_CTRL |= UDPHS_CTRL_DETACH; // detach
  UDPHS->UDPHS_CTRL &= ~UDPHS_CTRL_PULLD_DIS; // Enable Pull Down
}

#endif /* SAM3U_SERIES */
