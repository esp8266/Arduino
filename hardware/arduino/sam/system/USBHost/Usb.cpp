/* Copyright 2009-2011 Oleg Mazurov, Circuits At Home, http://www.circuitsathome.com */
/* USB functions */


#include "Arduino.h"
#include "Usb.h"
#include <stdio.h>

//#define TRACE_USBHOST(x)	x
#define TRACE_USBHOST(x)

static uint32_t usb_error = 0;
static uint32_t usb_task_state = USB_DETACHED_SUBSTATE_INITIALIZE;

DEV_RECORD devtable[USB_NUMDEVICES + 1];

// Endpoint data structure used during enumeration for uninitialized device.
EP_RECORD dev0ep;

/**
 * Class Constructor.
 */
USBHost::USBHost () {
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

	uint8_t i = 0;

	for (i = 0; i < (USB_NUMDEVICES + 1); i++)
	{
		// Clear device table
		devtable[i].epinfo = 0;
		devtable[i].devclass = 0;
	}

	// Set single EP for uninitialized device
    devtable[0].epinfo = &dev0ep;
  /*  dev0ep.sndToggle = bmSNDTOG0;   //set DATA0/1 toggles to 0
    dev0ep.rcvToggle = bmRCVTOG0;
*/
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
 * Get device table entry.
 */
EP_RECORD* USBHost::getDevTableEntry(uint32_t addr, uint32_t ep)
{
	EP_RECORD* ptr = 0;

	ptr = devtable[addr].epinfo;
	ptr += ep;
	return ptr;
}

/**
 * Set device table entry.
 */
void USBHost::setDevTableEntry(uint32_t addr, EP_RECORD* eprecord_ptr)
{
	devtable[addr].epinfo = eprecord_ptr;
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
uint32_t USBHost::ctrlReq(uint32_t addr, uint32_t ep, uint8_t bmReqType, uint8_t bRequest, uint8_t wValLo, uint8_t wValHi, uint16_t wInd, uint32_t nbytes, uint8_t* dataptr, uint32_t nak_limit)
{
	// Request direction, IN or OUT
	uint32_t direction = 0;
	uint32_t rcode = 0;
	SETUP_PKT setup_pkt;

	TRACE_USBHOST(printf("    => ctrlReq\r\n");)

	// Set peripheral address
	//regWr(rPERADDR, addr);

	// Allocate EP0 with default 8 bytes size if not already initialized
	rcode = UHD_EP0_Alloc(0, 8);
	if (rcode)
	{
		TRACE_USBHOST(printf("/!\\ USBHost::ctrlReq : EP0 allocation error: %lu\r\n", rcode);)
		return (rcode);
	}

	// Determine request direction
	if (bmReqType & 0x80)
	{
		direction = 1;
	}

	// Fill in setup packet
	setup_pkt.ReqType_u.bmRequestType = bmReqType;
	setup_pkt.bRequest = bRequest;
	setup_pkt.wVal_u.wValueLo = wValLo;
	setup_pkt.wVal_u.wValueHi = wValHi;
	setup_pkt.wIndex = wInd;
	setup_pkt.wLength = nbytes;

	// Transfer to setup packet FIFO
	//bytesWr(rSUDFIFO, 8, (uint8_t *)&setup_pkt);
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
		rcode = ctrlData(addr, ep, nbytes, dataptr, direction);
		if (rcode)
		{
			TRACE_USBHOST(printf("/!\\ USBHost::ctrlData : Data packet error: %lu\r\n", rcode);)
			return (rcode);
		}
	}

	// Status stage
	rcode = ctrlStatus(ep, direction);
	return (rcode);

}

/**
 * Control transfer with status stage and no data stage.
 * Assumes peripheral address is already set.
 */
uint32_t USBHost::ctrlStatus(uint32_t ep, uint32_t direction, uint32_t nak_limit)
{
	uint32_t rcode = 0;

	if (direction)
	{
		// GET
		TRACE_USBHOST(printf("    => ctrlStatus OUTHS\r\n");)
		rcode = dispatchPkt(tokOUTHS, ep, nak_limit);
	}
	else
	{
		// SET
		TRACE_USBHOST(printf("    => ctrlStatus INHS\r\n");)
		rcode = dispatchPkt(tokINHS, ep, nak_limit);
	}

	return (rcode);
}

/**
 * Control transfer with data stage. Stages 2 and 3 of control transfer.
 * Assumes peripheral address is set and setup packet has been sent.
 */
uint32_t USBHost::ctrlData(uint32_t addr, uint32_t ep, uint32_t nbytes, uint8_t* dataptr, uint32_t direction, uint32_t nak_limit)
{
	uint32_t rcode = 0;

	if (direction)
	{
		// IN transfer
		//devtable[addr].epinfo[ep].rcvToggle = bmRCVTOG1;
		TRACE_USBHOST(printf("    => ctrlData IN\r\n");)
		rcode = inTransfer(addr, ep, nbytes, dataptr, nak_limit);
		return (rcode);
	}
	else
	{
		// OUT transfer
		//devtable[addr].epinfo[ep].sndToggle = bmSNDTOG1;
		TRACE_USBHOST(printf("    => ctrlData OUT\r\n");)
		rcode = outTransfer(addr, ep, nbytes, dataptr, nak_limit);
		return (rcode);
	}
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
uint32_t USBHost::inTransfer(uint32_t addr, uint32_t ep, uint32_t nbytes, uint8_t* data, uint32_t nak_limit)
{
	uint32_t rcode = 0;
	uint32_t pktsize = 0;
	uint32_t maxpktsize = devtable[addr].epinfo[ep].MaxPktSize;
	uint32_t xfrlen = 0;

	// Set toggle value
	//regWr(rHCTL, devtable[addr].epinfo[ep].rcvToggle);

    while (1)
	{
		// Use a 'return' to exit this loop
		// IN packet to EP-'endpoint'. Function takes care of NAKS.
        rcode = dispatchPkt(tokIN, ep, nak_limit);
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
		pktsize = uhd_byte_count(ep);
		if (nbytes < pktsize)
			printf("ce test n'a pas ete fait...\r\n");
        data += UHD_EP_Read(ep, pktsize, data);

		// Clear the IRQ & free the buffer
        //regWr(rHIRQ, bmRCVDAVIRQ);

		// Add this packet's byte count to total transfer length
        xfrlen += pktsize;

        // The transfer is complete under two conditions:
        // 1. The device sent a short packet (L.T. maxPacketSize)
        // 2. 'nbytes' have been transferred.
        if ((pktsize < maxpktsize) || (xfrlen >= nbytes))
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
 * Google variant of inTransfer.
 * Pasted verbatim from ADK. Returns length instead of error code.
 * Provided for compatibility with Google Open Accessory code.
 */
int32_t USBHost::newInTransfer(uint32_t addr, uint32_t ep, uint32_t nbytes, uint8_t* data, uint32_t nak_limit)
{
/*
	uint32_t rcode = 0;
	uint32_t pktsize = 0;
	uint32_t maxpktsize = devtable[addr].epinfo[ep].MaxPktSize;
	uint32_t xfrlen = 0;

	// Set toggle value
	regWr(rHCTL, devtable[addr].epinfo[ep].rcvToggle);

    while (1)
	{
		// Use a 'return' to exit this loop
		// IN packet to EP-'endpoint'. Function takes care of NAKS.
        rcode = dispatchPkt(tokIN, ep, nak_limit);
        if (rcode)
		{
			// Should be 0, indicating ACK. Else return error code.
			return -1;
        }

        // check for RCVDAVIRQ and generate error if not present
        // the only case when absense of RCVDAVIRQ makes sense is when toggle error occured. Need to add handling for that
        if ((regRd(rHIRQ) & bmRCVDAVIRQ) == 0)
		{
			// Receive error
            return -1;
        }

		// Number of received bytes
		pktsize = regRd(rRCVBC);

		if (xfrlen + pktsize <= nbytes)
		{
			// Only copy the data to the buffer if the buffer's large enough.
			data = bytesRd(rRCVFIFO, pktsize, data);
		}

		// Clear the IRQ & free the buffer
        regWr(rHIRQ, bmRCVDAVIRQ);

		// Add this packet's byte count to total transfer length
        xfrlen += pktsize;

        // The transfer is complete under two conditions:
        // 1. The device sent a short packet (L.T. maxPacketSize)
        // 2. 'nbytes' have been transferred.
        if ((pktsize < maxpktsize) || (xfrlen >= nbytes))
		{
			// Have we transferred 'nbytes' bytes?
			if (regRd(rHRSL) & bmRCVTOGRD)
			{
				// Save toggle value
				devtable[addr].epinfo[ep].rcvToggle = bmRCVTOG1;
            }
            else
			{
				devtable[addr].epinfo[ep].rcvToggle = bmRCVTOG0;
            }

			if (xfrlen <= nbytes)
			{
				return xfrlen;
			}
			else
			{
				// Buffer overflow avoided so treat it as an error rather
				// than return partial data.
				return -1;
			}
        }
	}
*/
printf("error2\r\n");
return 1;
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
uint32_t USBHost::outTransfer(uint32_t addr, uint32_t ep, uint32_t nbytes, uint8_t* data, uint32_t nak_limit)
{
/*
	uint32_t rcode, retry_count;
	uint8_t* data_p = data;
	uint32_t bytes_tosend, nak_count;
	uint32_t bytes_left = nbytes;
	uint32_t maxpktsize = devtable[addr].epinfo[ep].MaxPktSize;
	uint32_t timeout = millis() + USB_XFER_TIMEOUT;

	if (!maxpktsize)
	{
		// Todo: move this check close to epinfo init. Make it 1< pktsize <64
		return 0xFE;
	}

	// Set toggle value
	regWr(rHCTL, devtable[addr].epinfo[ep].sndToggle);

	while (bytes_left)
	{
		retry_count = 0;
		nak_count = 0;
		bytes_tosend = (bytes_left >= maxpktsize) ? maxpktsize : bytes_left;

		// Fill output FIFO
		bytesWr(rSNDFIFO, bytes_tosend, data_p);
		// Set number of bytes
		regWr(rSNDBC, bytes_tosend);

		// Dispatch packet
		regWr(rHXFR, (tokOUT | ep));

		// Wait for the completion IRQ
		while (!(regRd(rHIRQ) & bmHXFRDNIRQ))
			;

		// Clear IRQ
		regWr(rHIRQ, bmHXFRDNIRQ);
		rcode = (regRd(rHRSL) & 0x0f);

		while (rcode && (timeout > millis()))
		{
			switch (rcode)
			{
				case hrNAK:
					nak_count++;
					if (nak_limit && (nak_count == USB_NAK_LIMIT)) {
					return (rcode);
					}
					break;
				case hrTIMEOUT:
					retry_count++;
					if (retry_count == USB_RETRY_LIMIT) {
					return (rcode);
					}
					break;
				default:
					return (rcode);
			}

			// Process NAK according to Host out NAK bug
			regWr(rSNDBC, 0);
			regWr(rSNDFIFO, *data_p);
			regWr(rSNDBC, bytes_tosend);

			// Dispatch packet
			regWr(rHXFR, (tokOUT | ep));

			// Wait for the completion IRQ
			while (!(regRd(rHIRQ) & bmHXFRDNIRQ))
				;

			// Clear IRQ
			regWr(rHIRQ, bmHXFRDNIRQ);
			rcode = (regRd(rHRSL) & 0x0f);
		}

		bytes_left -= bytes_tosend;
		data_p += bytes_tosend;
	}

	// Update toggle
	devtable[ addr ].epinfo[ ep ].sndToggle = (regRd(rHRSL) & bmSNDTOGRD) ? bmSNDTOG1 : bmSNDTOG0;
	return (rcode);
*/
printf("error3\r\n");
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
 */
uint32_t USBHost::dispatchPkt(uint32_t token, uint32_t ep, uint32_t nak_limit)
{
	uint32_t timeout = millis() + USB_XFER_TIMEOUT;
	uint32_t nak_count = 0;

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
				return 1;
			}
		}
	}

	return 2;
}

/**
 * USB main task.
 * Performs enumeration/cleanup.
 */
void USBHost::Task(void)
{
	uint32_t i = 0;
	uint32_t rcode = 0;
	//static uint8_t tmpaddr = 0;
	volatile uint32_t tmpdata = 0;
	static uint32_t delay = 0;
	USB_DEVICE_DESCRIPTOR buf;

    // Update USB task state on Vbus change
	tmpdata = UHD_GetVBUSState();
    switch (tmpdata)
	{
        case UHD_STATE_ERROR:
			// Illegal state
            usb_task_state = USB_DETACHED_SUBSTATE_ILLEGAL;
            break;

        case UHD_STATE_DISCONNECTED:
			// Disconnected state
            if ((usb_task_state & USB_STATE_MASK) != USB_STATE_DETACHED)
			{
                usb_task_state = USB_DETACHED_SUBSTATE_INITIALIZE;
            }
            break;

        case UHD_STATE_CONNECTED:
			// Attached state
            if ((usb_task_state & USB_STATE_MASK) == USB_STATE_DETACHED)
			{
                delay = millis() + USB_SETTLE_DELAY;
                usb_task_state = USB_ATTACHED_SUBSTATE_SETTLE;
            }
            break;
	}

	// USB state machine
    switch (usb_task_state)
	{
        case USB_DETACHED_SUBSTATE_INITIALIZE:
			TRACE_USBHOST(printf(" + USB_DETACHED_SUBSTATE_INITIALIZE\r\n");)

			// Init USB stack and driver
			UHD_Init();
            init();

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
					usb_task_state = USB_ATTACHED_SUBSTATE_GET_DEVICE_DESCRIPTOR_SIZE;
				}
            }
            break;

        case USB_ATTACHED_SUBSTATE_GET_DEVICE_DESCRIPTOR_SIZE:
			TRACE_USBHOST(printf(" + USB_ATTACHED_SUBSTATE_GET_DEVICE_DESCRIPTOR_SIZE\r\n");)

			// Get Device descriptor size
			// Set max.packet size to the minimum allowed
            devtable[0].epinfo->MaxPktSize = 8;
            rcode = getDevDescr(0, 0, 8, (uint8_t*)&buf);
            if (rcode == 0)
			{
				TRACE_USBHOST(printf(" + USB_ATTACHED_SUBSTATE_GET_DEVICE_DESCRIPTOR_SIZE : max pkt size = %d\r\n", buf.bMaxPacketSize0);)
                devtable[0].epinfo->MaxPktSize = buf.bMaxPacketSize0;

				// Reconfigure EP0 with max pkt size. (should be done after a USB reset)
				UHD_EP_Free(0, 0);
				if (UHD_EP0_Alloc(0, devtable[0].epinfo->MaxPktSize) == 0)
				{
					usb_task_state = USB_STATE_ADDRESSING;
				}
				else
				{
					TRACE_USBHOST(printf(" + USB_ATTACHED_SUBSTATE_GET_DEVICE_DESCRIPTOR_SIZE : realloc failure!\r\n");)
					usb_task_state = USB_STATE_ERROR;
				}
            }
            else
			{
                usb_error = USB_ATTACHED_SUBSTATE_GET_DEVICE_DESCRIPTOR_SIZE;
                usb_task_state = USB_STATE_ERROR;
            }
            break;

        case USB_STATE_ADDRESSING:
            for (i = 1; i < USB_NUMDEVICES; i++)
			{
				TRACE_USBHOST(printf(" + USB_STATE_ADDRESSING (i=%lu)\r\n", i);)

                if (devtable[i].epinfo == 0)
				{
					// Set correct MaxPktSize
                    // Temporary record until plugged with real device endpoint structure
                    devtable[i].epinfo = devtable[0].epinfo;

                    rcode = setAddr(0, 0, i);
                    if (rcode == 0)
					{
                        // Free address 0 used to start enumeration
						UHD_EP_Free(0, 0);

						// Alloc control endpoint with the new USB address
						if (UHD_EP0_Alloc(i, devtable[i].epinfo->MaxPktSize) == 0)
						{
							usb_task_state = USB_STATE_CONFIGURING;
							TRACE_USBHOST(printf(" + USB_STATE_CONFIGURING (i=%lu)\r\n", i);)
						}
						else
						{
							// Set address error
							TRACE_USBHOST(printf(" + USB_STATE_CONFIGURING (i=%lu) : realloc failure!\r\n", i);)
							usb_error = USB_STATE_ADDRESSING;
							usb_task_state = USB_STATE_ERROR;
						}
                    }
                    else
					{
						// Set address error
                        usb_error = USB_STATE_ADDRESSING;
                        usb_task_state = USB_STATE_ERROR;
                    }
					// Break if address assigned or error occured during address assignment attempt
                    break;
                }
            }

            if (usb_task_state == USB_STATE_ADDRESSING)
			{
				// No vacant place in devtable
                usb_error = 0xfe;
                usb_task_state = USB_STATE_ERROR;
            }
            break;

        case USB_STATE_CONFIGURING:
            break;

        case USB_STATE_RUNNING:
            break;

        case USB_STATE_ERROR:
            break;
    }
}
