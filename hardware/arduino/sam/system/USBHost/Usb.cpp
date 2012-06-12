/* Copyright (C) 2011 Circuits At Home, LTD. All rights reserved.

This software may be distributed and modified under the terms of the GNU
General Public License version 2 (GPL2) as published by the Free Software
Foundation and appearing in the file GPL2.TXT included in the packaging of
this file. Please note that GPL2 Section 2[b] requires that all works based
on this software must also be made publicly available under the terms of
the GPL2 ("Copyleft").

Contact information
-------------------

Circuits At Home, LTD
Web      :  http://www.circuitsathome.com
e-mail   :  support@circuitsathome.com
*/
/* USB functions */

#include "Arduino.h"
#include "Usb.h"
#include <stdio.h>

static uint32_t usb_error = 0;
static uint32_t usb_task_state = USB_DETACHED_SUBSTATE_INITIALIZE;

/**
 * Class Constructor.
 */
USBHost::USBHost () : bmHubPre(0)
{
	// Set up state machine
	usb_task_state = USB_DETACHED_SUBSTATE_INITIALIZE;

	// Init host stack
	init();
}

/**
 * Initialize data structures.
 */
void USBHost::init()
{
	devConfigIndex	= 0;
	bmHubPre		= 0;
}

/**
 * Get USB state.
 */
uint32_t USBHost::getUsbTaskState(void)
{
    return (usb_task_state);
}

/**
 * Set USB state.
 */
void USBHost::setUsbTaskState(uint32_t state)
{
    usb_task_state = state;
}

/**
 * Get end point info from address.
 */
EpInfo* USBHost::getEpInfoEntry(uint32_t addr, uint32_t ep)
{
	UsbDevice *p = addrPool.GetUsbDevicePtr(addr);

	if (!p || !p->epinfo)
		return NULL;

	EpInfo *pep = p->epinfo;

	for (uint32_t i = 0; i < p->epcount; i++)
	{
		if (pep->epAddr == ep)
			return pep;

		pep++;
	}
	return NULL;
}

/**
 * Set device end point entry.
 * Each device is different and has different number of endpoints.
 * This function plugs endpoint record structure, defined in application to devtable.
 */
uint32_t USBHost::setEpInfoEntry(uint32_t addr, uint32_t epcount, EpInfo* eprecord_ptr)
{
	if (!eprecord_ptr)
		return USB_ERROR_INVALID_ARGUMENT;

	UsbDevice *p = addrPool.GetUsbDevicePtr(addr);

	if (!p)
		return USB_ERROR_ADDRESS_NOT_FOUND_IN_POOL;

	p->address	= addr;
	p->epinfo	= eprecord_ptr;
	p->epcount	= epcount;

	return 0;
}

uint32_t USBHost::SetAddress(uint32_t addr, uint32_t ep, EpInfo **ppep, uint32_t &nak_limit)
{
	UsbDevice *p = addrPool.GetUsbDevicePtr(addr);

	if (!p)
		return USB_ERROR_ADDRESS_NOT_FOUND_IN_POOL;

 	if (!p->epinfo)
		return USB_ERROR_EPINFO_IS_NULL;

	*ppep = getEpInfoEntry(addr, ep);

	if (!*ppep)
		return USB_ERROR_EP_NOT_FOUND_IN_TBL;

	nak_limit = (0x0001UL << ( ( (*ppep)->bmNakPower > USB_NAK_MAX_POWER ) ? USB_NAK_MAX_POWER : (*ppep)->bmNakPower) );
	nak_limit--;

	// Set peripheral address
	//regWr( rPERADDR, addr );
	uhd_configure_address(ep, addr);

	///////////////////////////////////////////////////////////////////////////////////////////// Y A TIL QQCHOSE A FAIRE???
	//uint8_t mode = regRd( rMODE );
	// Set bmLOWSPEED and bmHUBPRE in case of low-speed device, reset them otherwise
	//regWr( rMODE, (p->lowspeed) ? mode | bmLOWSPEED | bmHubPre : mode & ~(bmHUBPRE | bmLOWSPEED));

	return 0;
}

/**
 * Send a control request.
 * Sets address, endpoint, fills control packet with necessary data, dispatches control packet, and initiates bulk IN transfer depending on request.
 * Actual requests are defined as inlines.
 *
 * return codes:
 * 00       =   success
 * 01-0f    =   non-zero HRSLT
 */
uint32_t USBHost::ctrlReq(uint32_t addr, uint32_t ep, uint8_t bmReqType, uint8_t bRequest, uint8_t wValLo, uint8_t wValHi, uint16_t wInd, uint16_t total, uint32_t nbytes, uint8_t* dataptr, USBReadParser *p)
{
	// Request direction, IN or OUT
	uint32_t direction = 0;
	uint32_t rcode = 0;
	SETUP_PKT setup_pkt;

	EpInfo *pep = 0;
	uint32_t nak_limit;

	TRACE_USBHOST(printf("    => ctrlReq\r\n");)

	// Set peripheral address
	rcode = SetAddress(addr, ep, &pep, nak_limit);
	if (rcode)
		return rcode;

	// Allocate EP0 with default 8 bytes size if not already initialized
	rcode = UHD_EP0_Alloc(0, 8);
	if (rcode)
	{
		TRACE_USBHOST(printf("/!\\ USBHost::ctrlReq : EP0 allocation error: %lu\r\n", rcode);)
		return (rcode);
	}

	// Determine request direction
	direction = (( bmReqType & 0x80 ) > 0);

	// Fill in setup packet
    setup_pkt.ReqType_u.bmRequestType	= bmReqType;
    setup_pkt.bRequest					= bRequest;
    setup_pkt.wVal_u.wValueLo			= wValLo;
    setup_pkt.wVal_u.wValueHi			= wValHi;
    setup_pkt.wIndex					= wInd;
    setup_pkt.wLength					= total;

	// Configure and write the setup packet into the FIFO
	//bytesWr(rSUDFIFO, 8, (uint8_t *)&setup_pkt);
	uhd_configure_pipe_token(0, tokSETUP);
	UHD_EP_Write(ep, 8, (uint8_t *)&setup_pkt);

	// Dispatch packet
	rcode = dispatchPkt(tokSETUP, ep, nak_limit);
	if (rcode)
	{
		// Return HRSLT if not zero
		TRACE_USBHOST(printf("/!\\ USBHost::ctrlReq : Setup packet error: %lu\r\n", rcode);)
		return (rcode);
	}

	// Data stage (if present)
	if (dataptr != 0)
	{
		if (direction)
		{
			// IN transfer
			TRACE_USBHOST(printf("    => ctrlData IN\r\n");)
			uint32_t left = total;

			while (left)
			{
				// Bytes read into buffer
				uint32_t read = nbytes;

				rcode = InTransfer(pep, nak_limit, &read, dataptr);
				if (rcode)
					return rcode;

				// Invoke callback function if inTransfer completed successfuly and callback function pointer is specified
				if (!rcode && p)
					((USBReadParser*)p)->Parse(read, dataptr, total - left);

				left -= read;

				if (read < nbytes)
					break;
			}
		}
		else
		{
			// OUT transfer
			//devtable[addr].epinfo[ep].sndToggle = bmSNDTOG1;
			TRACE_USBHOST(printf("    => ctrlData OUT\r\n");)
			rcode = OutTransfer(pep, nak_limit, nbytes, dataptr);
		}

		if (rcode)
		{
			TRACE_USBHOST(printf("/!\\ USBHost::ctrlData : Data packet error: %lu\r\n", rcode);)
			return (rcode);
		}
	}

	// Status stage
	return dispatchPkt((direction) ? tokOUTHS : tokINHS, ep, nak_limit );
}

/**
 * IN transfer to arbitrary endpoint.
 * Assumes PERADDR is set. Handles multiple packets if necessary. Transfers 'nbytes' bytes.
 * Keep sending INs and writes data to memory area pointed by 'data'.
 * rcode 0 if no errors
 *       01-0f is relayed from dispatchPkt()
 *		 f0 means RCVDAVIRQ error
 *       fe USB xfer timeout
 */
 uint32_t USBHost::inTransfer(uint32_t addr, uint32_t ep, uint32_t *nbytesptr, uint8_t* data)
{
	EpInfo *pep = NULL;
	uint32_t nak_limit = 0;

	uint32_t rcode = SetAddress(addr, ep, &pep, nak_limit);

	if (rcode)
		return rcode;

	return InTransfer(pep, nak_limit, nbytesptr, data);
}

uint32_t USBHost::InTransfer(EpInfo *pep, uint32_t nak_limit, uint32_t *nbytesptr, uint8_t* data)
{
	uint32_t rcode = 0;
	uint32_t pktsize = 0;
	uint16_t nbytes = *nbytesptr;
	uint8_t maxpktsize = pep->maxPktSize;

	*nbytesptr = 0;

	// Set toggle value
	//regWr(rHCTL, devtable[addr].epinfo[ep].rcvToggle);

    while (1)
	{
		// Use a 'return' to exit this loop
		// IN packet to EP-'endpoint'. Function takes care of NAKS.
        rcode = dispatchPkt(tokIN, pep->epAddr, nak_limit);
        if (rcode)
		{
			// Should be 0, indicating ACK. Else return error code.
            return (rcode);
        }

        // check for RCVDAVIRQ and generate error if not present
        // the only case when absense of RCVDAVIRQ makes sense is when toggle error occured. Need to add handling for that
        /*if ((regRd(rHIRQ) & bmRCVDAVIRQ) == 0)
		{
			// Receive error
            return (0xf0);
        }*/

		// Number of received bytes
        //pktsize = regRd(rRCVBC);
        //data = bytesRd(rRCVFIFO, pktsize, data);
		pktsize = uhd_byte_count(pep->epAddr);
		if (nbytes < pktsize)
			printf("ce test n'a pas ete fait...\r\n");
        data += UHD_EP_Read(pep->epAddr, pktsize, data);

		// Clear the IRQ & free the buffer
        //regWr(rHIRQ, bmRCVDAVIRQ);

		// Add this packet's byte count to total transfer length
        *nbytesptr += pktsize;

        // The transfer is complete under two conditions:
        // 1. The device sent a short packet (L.T. maxPacketSize)
        // 2. 'nbytes' have been transferred.
        if ((pktsize < maxpktsize) || (*nbytesptr >= nbytes))
		{
			/*// Have we transferred 'nbytes' bytes?
            if (regRd(rHRSL) & bmRCVTOGRD)
			{
				// Save toggle value
                devtable[addr].epinfo[ep].rcvToggle = bmRCVTOG1;
            }
            else
			{
                devtable[addr].epinfo[ep].rcvToggle = bmRCVTOG0;
            }*/

            return (0);
        }
	}
}

/**
 * OUT transfer to arbitrary endpoint.
 * Assumes PERADDR is set. Handles multiple packets if necessary. Transfers 'nbytes' bytes.
 * Handles NAK bug per Maxim Application Note 4000 for single buffer transfer
 * rcode 0 if no errors
 *       01-0f is relayed from HRSL
 *
 * Major part of this function borrowed from code shared by Richard Ibbotson
 */
uint32_t USBHost::outTransfer(uint32_t addr, uint32_t ep, uint32_t nbytes, uint8_t* data)
{
/*
	EpInfo		*pep = NULL;
	uint16_t	nak_limit;

	uint8_t rcode = SetAddress(addr, ep, &pep, nak_limit);

	if (rcode)
		return rcode;

	return OutTransfer(pep, nak_limit, nbytes, data);
*/
printf("j'ai beau etre matinal.. j'ai mal!!!!\r\n");
return 1;
}

uint32_t USBHost::OutTransfer(EpInfo *pep, uint32_t nak_limit, uint32_t nbytes, uint8_t *data)
{
/*
	uint8_t		rcode, retry_count;
	uint8_t		*data_p = data;   //local copy of the data pointer
	uint16_t	bytes_tosend, nak_count;
	uint16_t	bytes_left = nbytes;

	uint8_t maxpktsize = pep->maxPktSize;

	if (maxpktsize < 1 || maxpktsize > 64)
		return USB_ERROR_INVALID_MAX_PKT_SIZE;

	unsigned long timeout = millis() + USB_XFER_TIMEOUT;

	regWr( rHCTL, (pep->bmSndToggle) ? bmSNDTOG1 : bmSNDTOG0 );    //set toggle value

	while( bytes_left )
	{
		retry_count = 0;
		nak_count = 0;
		bytes_tosend = ( bytes_left >= maxpktsize ) ? maxpktsize : bytes_left;
		bytesWr( rSNDFIFO, bytes_tosend, data_p );      //filling output FIFO
		regWr( rSNDBC, bytes_tosend );                  //set number of bytes
		regWr( rHXFR, ( tokOUT | pep->epAddr ));        //dispatch packet
		while(!(regRd( rHIRQ ) & bmHXFRDNIRQ ));        //wait for the completion IRQ
		regWr( rHIRQ, bmHXFRDNIRQ );                    //clear IRQ
		rcode = ( regRd( rHRSL ) & 0x0f );

		while( rcode && ( timeout > millis()))
		{
			switch( rcode )
			{
			case hrNAK:
				nak_count ++;
				if( nak_limit && ( nak_count == nak_limit ))
					return( rcode );
				break;
			case hrTIMEOUT:
				retry_count ++;
				if( retry_count == USB_RETRY_LIMIT )
					return( rcode );
				break;
			default:
				return( rcode );
			}//switch( rcode

			// process NAK according to Host out NAK bug
			regWr( rSNDBC, 0 );
			regWr( rSNDFIFO, *data_p );
			regWr( rSNDBC, bytes_tosend );
			regWr( rHXFR, ( tokOUT | pep->epAddr ));        //dispatch packet
			while(!(regRd( rHIRQ ) & bmHXFRDNIRQ ));        //wait for the completion IRQ
			regWr( rHIRQ, bmHXFRDNIRQ );                    //clear IRQ
			rcode = ( regRd( rHRSL ) & 0x0f );
		}//while( rcode && ....
		bytes_left -= bytes_tosend;
		data_p += bytes_tosend;
	}//while( bytes_left...
	pep->bmSndToggle = ( regRd( rHRSL ) & bmSNDTOGRD ) ? 1 : 0; //bmSNDTOG1 : bmSNDTOG0;  //update toggle
	return( rcode );    //should be 0 in all cases
	*/
	printf("j'ai beau etre matinal.. j'ai mal!!!! arggghh\r\n");
return 1;
}

/**
 * Dispatch USB packet.
 * Assumes peripheral address is set and relevant buffer is loaded/empty.
 * If NAK, tries to re-send up to nak_limit times.
 * If nak_limit == 0, do not count NAKs, exit after timeout.
 * If bus timeout, re-sends up to USB_RETRY_LIMIT times.
 * rcode 0 for success
 *       1 for naked
 *       2 for timeout
 *
 * Note: pipe token MUST be configured first when the corresponding FIFO is used,
 * else packet may be corrupted.
 */
uint32_t USBHost::dispatchPkt(uint32_t token, uint32_t ep, uint32_t nak_limit)
{
	uint32_t timeout = millis() + USB_XFER_TIMEOUT;
	uint32_t nak_count = 0;
	uint32_t rcode = USB_ERROR_TRANSFER_TIMEOUT;

	TRACE_USBHOST(printf("     => dispatchPkt token=%lu\r\n", token);)

	// Launch the transfer
	//regWr(rHXFR, (token | ep));
	UHD_EP_Send(ep, token);

	while (timeout > millis())
	{
		// Wait for transfer completion
		if (UHD_EP_Is_Transfer_Complete(ep, token))
		{
			return 0;
		}

		//
		if (Is_uhd_nak_received(ep))
		{
			uhd_ack_nak_received(ep);
			nak_count++;
			if (nak_limit && (nak_count == nak_limit))
			{
				return 1; //////////////////////////// cree un code pour ca
			}
		}
	}

	return rcode;
}

/**
 * Configure a device using known device classes.
 * The device get a new address even if its class remain unknown.
 */
uint32_t USBHost::Configuring(uint32_t parent, uint32_t port, uint32_t lowspeed)
{
	uint32_t rcode = 0;

	for (; devConfigIndex < USB_NUMDEVICES; ++devConfigIndex)
	{
		if (!devConfig[devConfigIndex])
			continue;

		rcode = devConfig[devConfigIndex]->Init(parent, port, lowspeed);

		if (!rcode)
		{
			TRACE_USBHOST(printf("USBHost::Configuring : found device class!\r\n");)
			devConfigIndex = 0;
			return 0;
		}

		if (!(rcode == USB_DEV_CONFIG_ERROR_DEVICE_NOT_SUPPORTED || rcode == USB_ERROR_CLASS_INSTANCE_ALREADY_IN_USE))
		{
			// in case of an error devConfigIndex should be reset to 0
			//		in order to start from the very beginning the
			//		next time the program gets here
			if (rcode != USB_DEV_CONFIG_ERROR_DEVICE_INIT_INCOMPLETE)
				devConfigIndex = 0;

			return rcode;
		}
	}

	// if we get here that means that the device class is not supported by any of registered classes
	devConfigIndex = 0;

	rcode = DefaultAddressing(parent, port, lowspeed);

	return rcode;
}

uint32_t USBHost::DefaultAddressing(uint32_t parent, uint32_t port, uint32_t lowspeed)
{
	uint32_t rcode = 0;
	UsbDevice *p0 = 0, *p = 0;

	// Get pointer to pseudo device with address 0 assigned
	p0 = addrPool.GetUsbDevicePtr(0);

	if (!p0)
		return USB_ERROR_ADDRESS_NOT_FOUND_IN_POOL;

	if (!p0->epinfo)
		return USB_ERROR_EPINFO_IS_NULL;

	p0->lowspeed = (lowspeed) ? 1 : 0;

	// Allocate new address according to device class
	uint32_t bAddress = addrPool.AllocAddress(parent, 0, port);

	if (!bAddress)
		return USB_ERROR_OUT_OF_ADDRESS_SPACE_IN_POOL;

	p = addrPool.GetUsbDevicePtr(bAddress);

	if (!p)
		return USB_ERROR_ADDRESS_NOT_FOUND_IN_POOL;

	p->lowspeed = lowspeed;

	// Assign new address to the device
	rcode = setAddr(0, 0, bAddress);

	if (rcode)
	{
		TRACE_USBHOST(printf("/!\\ USBHost::DefaultAddressing : Set address failed with code: %lu\r\n", rcode);)
		addrPool.FreeAddress(bAddress);
		bAddress = 0;
		return rcode;
	}

	return 0;
}

uint32_t USBHost::ReleaseDevice(uint32_t addr)
{
	if (!addr)
		return 0;

	for (uint32_t i = 0; i < USB_NUMDEVICES; ++i)
		if (devConfig[i]->GetAddress() == addr)
			return devConfig[i]->Release();

	return 0;
}

// Get device descriptor
uint32_t USBHost::getDevDescr(uint32_t addr, uint32_t ep, uint32_t nbytes, uint8_t* dataptr)
{
    return (ctrlReq(addr, ep, bmREQ_GET_DESCR, USB_REQUEST_GET_DESCRIPTOR, 0x00, USB_DESCRIPTOR_DEVICE, 0x0000, nbytes, nbytes, dataptr, 0));
}

// Get configuration descriptor
uint32_t USBHost::getConfDescr(uint32_t addr, uint32_t ep, uint32_t nbytes, uint32_t conf, uint8_t* dataptr)
{
	return (ctrlReq(addr, ep, bmREQ_GET_DESCR, USB_REQUEST_GET_DESCRIPTOR, conf, USB_DESCRIPTOR_CONFIGURATION, 0x0000, nbytes, nbytes, dataptr, 0));
}

uint32_t USBHost::getConfDescr(uint32_t addr, uint32_t ep, uint32_t conf, USBReadParser *p)
{
	const uint32_t bufSize = 64;
	uint8_t buf[bufSize];

	uint32_t ret = getConfDescr(addr, ep, 8, conf, buf);

	if (ret)
		return ret;

	uint32_t total = ((USB_CONFIGURATION_DESCRIPTOR*)buf)->wTotalLength;
	delay(100);

    return (ctrlReq(addr, ep, bmREQ_GET_DESCR, USB_REQUEST_GET_DESCRIPTOR, conf, USB_DESCRIPTOR_CONFIGURATION, 0x0000, total, bufSize, buf, p));
}

// Get string descriptor
uint32_t USBHost::getStrDescr(uint32_t addr, uint32_t ep, uint16_t ns, uint8_t index, uint16_t langid, uint8_t* dataptr)
{
    return (ctrlReq(addr, ep, bmREQ_GET_DESCR, USB_REQUEST_GET_DESCRIPTOR, index, USB_DESCRIPTOR_STRING, langid, ns, ns, dataptr, 0));
}

// Set address
uint32_t USBHost::setAddr(uint32_t oldaddr, uint32_t ep, uint32_t newaddr)
{
	TRACE_USBHOST(printf("   => setAddr\r\n");)
    return ctrlReq(oldaddr, ep, bmREQ_SET, USB_REQUEST_SET_ADDRESS, newaddr, 0x00, 0x0000, 0x0000, 0x0000, 0, 0);
}

// Set configuration
uint32_t USBHost::setConf(uint32_t addr, uint32_t ep, uint32_t conf_value)
{
    return (ctrlReq(addr, ep, bmREQ_SET, USB_REQUEST_SET_CONFIGURATION, conf_value, 0x00, 0x0000, 0x0000, 0x0000, 0, 0));
}

/**
 * USB main task.
 * Performs enumeration/cleanup.
 */
void USBHost::Task(void)
{
	uint32_t rcode = 0;
	volatile uint32_t tmpdata = 0;
	static uint32_t delay = 0;
	uint32_t lowspeed = 0;

    // Update USB task state on Vbus change
	tmpdata = UHD_GetVBUSState();
    switch (tmpdata)
	{
        case UHD_STATE_ERROR:
			// Illegal state
            usb_task_state = USB_DETACHED_SUBSTATE_ILLEGAL;
			lowspeed = 0;
            break;

        case UHD_STATE_DISCONNECTED:
			// Disconnected state
            if ((usb_task_state & USB_STATE_MASK) != USB_STATE_DETACHED)
			{
                usb_task_state = USB_DETACHED_SUBSTATE_INITIALIZE;
				lowspeed = 0;
            }
            break;

        case UHD_STATE_CONNECTED:
			// Attached state
            if ((usb_task_state & USB_STATE_MASK) == USB_STATE_DETACHED)
			{
                delay = millis() + USB_SETTLE_DELAY;
                usb_task_state = USB_ATTACHED_SUBSTATE_SETTLE;
				///////////////////////////////////////////////////////////lowspeed = 0 ou 1;  faire un get speed
            }
            break;
	}

	for (uint32_t i = 0; i < USB_NUMDEVICES; ++i)
		if (devConfig[i])
			rcode = devConfig[i]->Poll();

	// USB state machine
    switch (usb_task_state)
	{
        case USB_DETACHED_SUBSTATE_INITIALIZE:
			TRACE_USBHOST(printf(" + USB_DETACHED_SUBSTATE_INITIALIZE\r\n");)

			// Init USB stack and driver
			UHD_Init();
            init();

			for (uint32_t i = 0; i < USB_NUMDEVICES; ++i)
				if (devConfig[i])
					rcode = devConfig[i]->Release();

            usb_task_state = USB_DETACHED_SUBSTATE_WAIT_FOR_DEVICE;
            break;

        case USB_DETACHED_SUBSTATE_WAIT_FOR_DEVICE:
			// Nothing to do
            break;

        case USB_DETACHED_SUBSTATE_ILLEGAL:
			// Nothing to do
            break;

        case USB_ATTACHED_SUBSTATE_SETTLE:
			// Settle time for just attached device
            if (delay < millis())
			{
				TRACE_USBHOST(printf(" + USB_ATTACHED_SUBSTATE_SETTLE\r\n");)
                usb_task_state = USB_ATTACHED_SUBSTATE_RESET_DEVICE;
            }
            break;

        case USB_ATTACHED_SUBSTATE_RESET_DEVICE:
			TRACE_USBHOST(printf(" + USB_ATTACHED_SUBSTATE_RESET_DEVICE\r\n");)

			// Trigger Bus Reset
            UHD_BusReset();
            usb_task_state = USB_ATTACHED_SUBSTATE_WAIT_RESET_COMPLETE;
            break;

        case USB_ATTACHED_SUBSTATE_WAIT_RESET_COMPLETE:
            if (Is_uhd_reset_sent())
			{
				TRACE_USBHOST(printf(" + USB_ATTACHED_SUBSTATE_WAIT_RESET_COMPLETE\r\n");)

				// Clear Bus Reset flag
				uhd_ack_reset_sent();

				// Enable Start Of Frame generation
                uhd_enable_sof();

                usb_task_state = USB_ATTACHED_SUBSTATE_WAIT_SOF;

				// Wait 20ms after Bus Reset (USB spec)
                delay = millis() + 20;
            }
            break;

        case USB_ATTACHED_SUBSTATE_WAIT_SOF:
			// Wait for SOF received first
            if (Is_uhd_sof())
			{
				if (delay < millis())
				{
					TRACE_USBHOST(printf(" + USB_ATTACHED_SUBSTATE_WAIT_SOF\r\n");)

					// 20ms waiting elapsed
					usb_task_state = USB_STATE_CONFIGURING;
				}
            }
            break;

        case USB_STATE_CONFIGURING:
			TRACE_USBHOST(printf(" + USB_STATE_CONFIGURING\r\n");)
			rcode = Configuring(0, 0, lowspeed);

			if (rcode)
			{
				TRACE_USBHOST(printf("/!\\ USBHost::Task : USB_STATE_CONFIGURING failed with code: %lu\r\n", rcode);)
				if (rcode != USB_DEV_CONFIG_ERROR_DEVICE_INIT_INCOMPLETE)
				{
					usb_error = rcode;
					usb_task_state = USB_STATE_ERROR;
				}
			}
			else
			{
				usb_task_state = USB_STATE_RUNNING;
				TRACE_USBHOST(printf(" + USB_STATE_RUNNING\r\n");)
			}
            break;

        case USB_STATE_RUNNING:
            break;

        case USB_STATE_ERROR:
            break;
    }
}
