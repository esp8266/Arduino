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

/* Google ADK interface */

#include "adk.h"

const uint32_t ADK::epDataInIndex  = 1;
const uint32_t ADK::epDataOutIndex = 2;

/**
 * \brief ADK class constructor.
 */
ADK::ADK(USBHost *p, const char* pmanufacturer,
				const char* pmodel,
				const char* pdescription,
				const char* pversion,
				const char* puri,
				const char* pserial) :
		manufacturer(pmanufacturer),
		model(pmodel),
		description(pdescription),
		version(pversion),
		uri(puri),
		serial(pserial),
		pUsb(p),
		bAddress(0),
		bNumEP(1),
		ready(false)
{
	// Initialize endpoint data structures
	for (uint32_t i = 0; i < ADK_MAX_ENDPOINTS; ++i)
	{
		epInfo[i].deviceEpNum	= 0;
		epInfo[i].hostPipeNum	= 0;
		epInfo[i].maxPktSize	= (i) ? 0 : 8;
		epInfo[i].epAttribs		= 0;
		epInfo[i].bmNakPower  	= (i) ? USB_NAK_NOWAIT : USB_NAK_MAX_POWER;
	}

	// Register in USB subsystem
	if (pUsb)
	{
		pUsb->RegisterDeviceClass(this);
	}
}

/**
 * \brief Initialize connection to an Android Phone.
 *
 * \param parent USB device address of the Parent device.
 * \param port USB device base address.
 * \param lowspeed USB device speed.
 *
 * \return 0 on success, error code otherwise.
 */
uint32_t ADK::Init(uint32_t parent, uint32_t port, uint32_t lowspeed)
{

	uint8_t		buf[sizeof(USB_DEVICE_DESCRIPTOR)];
	uint32_t	rcode = 0;
	UsbDevice	*p = NULL;
	EpInfo		*oldep_ptr = NULL;
	uint32_t 	adkproto = -1;
	uint32_t	num_of_conf = 0;

	// Get memory address of USB device address pool
	AddressPool	&addrPool = pUsb->GetAddressPool();

	TRACE_USBHOST(printf("ADK::Init\r\n");)

    // Check if address has already been assigned to an instance
    if (bAddress)
	{
		return USB_ERROR_CLASS_INSTANCE_ALREADY_IN_USE;
	}

    // Get pointer to pseudo device with address 0 assigned
	p = addrPool.GetUsbDevicePtr(0);

	if (!p)
	{
		return USB_ERROR_ADDRESS_NOT_FOUND_IN_POOL;
    }

	if (!p->epinfo)
	{
		return USB_ERROR_EPINFO_IS_NULL;
	}

	// Save old pointer to EP_RECORD of address 0
	oldep_ptr = p->epinfo;

	// Temporary assign new pointer to epInfo to p->epinfo in order to avoid toggle inconsistence
	p->epinfo = epInfo;

	p->lowspeed = lowspeed;

	// Get device descriptor
	rcode = pUsb->getDevDescr(0, 0, sizeof(USB_DEVICE_DESCRIPTOR), (uint8_t*)buf);

	// Restore p->epinfo
	p->epinfo = oldep_ptr;

	if (rcode)
	{
		goto FailGetDevDescr;
	}

	// Allocate new address according to device class
	bAddress = addrPool.AllocAddress(parent, false, port);

	// Extract Max Packet Size from device descriptor
	epInfo[0].maxPktSize = (uint8_t)((USB_DEVICE_DESCRIPTOR*)buf)->bMaxPacketSize0;

	// Assign new address to the device
	rcode = pUsb->setAddr(0, 0, bAddress);
	if (rcode)
	{
		p->lowspeed = false;
		addrPool.FreeAddress(bAddress);
		bAddress = 0;
		TRACE_USBHOST(printf("ADK::Init : setAddr failed with rcode %lu\r\n", rcode);)
		return rcode;
	}

	TRACE_USBHOST(printf("ADK::Init : device address is now %lu\r\n", bAddress);)

	p->lowspeed = false;

	//get pointer to assigned address record
	p = addrPool.GetUsbDevicePtr(bAddress);
	if (!p)
	{
		return USB_ERROR_ADDRESS_NOT_FOUND_IN_POOL;
	}

	p->lowspeed = lowspeed;

	// Assign epInfo to epinfo pointer - only EP0 is known
	rcode = pUsb->setEpInfoEntry(bAddress, 1, epInfo);
	if (rcode)
	{
		goto FailSetDevTblEntry;
	}

	// Check if ADK device is already in accessory mode; if yes, configure and exit
	if (((USB_DEVICE_DESCRIPTOR*)buf)->idVendor == ADK_VID &&
		(((USB_DEVICE_DESCRIPTOR*)buf)->idProduct == ADK_PID || ((USB_DEVICE_DESCRIPTOR*)buf)->idProduct == ADB_PID))
	{
		TRACE_USBHOST(printf("ADK::Init : Accessory mode device detected\r\n");)

		/* Go through configurations, find first bulk-IN, bulk-OUT EP, fill epInfo and quit */
		num_of_conf = ((USB_DEVICE_DESCRIPTOR*)buf)->bNumConfigurations;

		TRACE_USBHOST(printf("ADK::Init : number of configuration is %lu\r\n", num_of_conf);)

		for (uint32_t i = 0; i < num_of_conf; ++i)
		{
			ConfigDescParser<0, 0, 0, 0> confDescrParser(this);

			delay(1);
			rcode = pUsb->getConfDescr(bAddress, 0, i, &confDescrParser);

#if defined(XOOM)
			//Added by Jaylen Scott Vanorden
			if (rcode)
			{
				TRACE_USBHOST(printf("ADK::Init : Got 1st bad code for config: %lu\r\n", rcode);)

				// Try once more
				rcode = pUsb->getConfDescr(bAddress, 0, i, &confDescrParser);
			}
#endif

			if (rcode)
			{
				goto FailGetConfDescr;
			}

			if (bNumEP > 2)
			{
				break;
			}
		}

		if (bNumEP == 3)
		{
			// Assign epInfo to epinfo pointer - this time all 3 endpoins
			rcode = pUsb->setEpInfoEntry(bAddress, 3, epInfo);
			if (rcode)
			{
				goto FailSetDevTblEntry;
			}
		}

		// Set Configuration Value
		rcode = pUsb->setConf(bAddress, 0, bConfNum);
		if (rcode)
		{
			goto FailSetConf;
		}

		TRACE_USBHOST(printf("ADK::Init : ADK device configured successfully\r\n");)
		ready = true;

		return 0;
	}

	// Probe device - get accessory protocol revision
	delay(1);
	rcode = getProto((uint8_t*)&adkproto);

#if defined(XOOM)
	// Added by Jaylen Scott Vanorden
	if (rcode)
	{
		TRACE_USBHOST(printf("ADK::Init : Got 1st bad code for config: %lu\r\n", rcode);)

		// Try once more
		rcode = getProto((uint8_t*)&adkproto );
	}
#endif

	if (rcode)
	{
		goto FailGetProto;
	}
	TRACE_USBHOST(printf("ADK::Init : DK protocol rev. %lu", adkproto);)

	// Send ID strings
	sendStr(ACCESSORY_STRING_MANUFACTURER, manufacturer);
	sendStr(ACCESSORY_STRING_MODEL, model);
	sendStr(ACCESSORY_STRING_DESCRIPTION, description);
	sendStr(ACCESSORY_STRING_VERSION, version);
	sendStr(ACCESSORY_STRING_URI, uri);
	sendStr(ACCESSORY_STRING_SERIAL, serial);

	// Switch to accessory mode
	// The Android phone will reset
	rcode = switchAcc();
	if (rcode)
	{
		goto FailSwAcc;
	}
	rcode = -1;
	goto SwAttempt;   // Switch to accessory mode

	// Diagnostic messages
FailGetDevDescr:
	TRACE_USBHOST(printf("ADK::Init getDevDescr : ");)
	goto Fail;

FailSetDevTblEntry:
	TRACE_USBHOST(printf("ADK::Init setDevTblEn : ");)
	goto Fail;

FailGetProto:
	TRACE_USBHOST(printf("ADK::Init getProto : ");)
	goto Fail;

FailSwAcc:
	TRACE_USBHOST(printf("ADK::Init swAcc : ");)
	goto Fail;

SwAttempt:
	TRACE_USBHOST(printf("ADK::Init Accessory mode switch attempt : ");)
	goto Fail;

FailGetConfDescr:
	TRACE_USBHOST(printf("ADK::Init getConf : ");)
	goto Fail;

FailSetConf:
	TRACE_USBHOST(printf("ADK::Init setConf : ");)
	goto Fail;

Fail:
	TRACE_USBHOST(printf("error code: %lu\r\n", rcode);)
	Release();
	return rcode;
}

/**
 * \brief Extract bulk-IN and bulk-OUT endpoint information from configuration
 * descriptor.
 *
 * \param conf Configuration number.
 * \param iface Interface number.
 * \param alt Alternate setting.
 * \param proto Protocol version used.
 * \param pep Pointer to endpoint descriptor.
 */
void ADK::EndpointXtract(uint32_t conf, uint32_t iface, uint32_t alt, uint32_t proto, const USB_ENDPOINT_DESCRIPTOR *pep)
{
	if (bNumEP == 3)
	{
		return;
	}

	bConfNum = conf;

	uint32_t index = 0;
	uint32_t pipe = 0;

	if ((pep->bmAttributes & 0x02) == 2)
	{
		index = ((pep->bEndpointAddress & 0x80) == 0x80) ? epDataInIndex : epDataOutIndex;
	}

	// Fill in the endpoint info structure
	epInfo[index].deviceEpNum = pep->bEndpointAddress & 0x0F;
	epInfo[index].maxPktSize = pep->wMaxPacketSize;

	TRACE_USBHOST(printf("ADK::EndpointXtract : Found new endpoint\r\n");)
	TRACE_USBHOST(printf("ADK::EndpointXtract : deviceEpNum: %lu\r\n", epInfo[index].deviceEpNum);)
	TRACE_USBHOST(printf("ADK::EndpointXtract : maxPktSize: %lu\r\n", epInfo[index].maxPktSize);)
	TRACE_USBHOST(printf("ADK::EndpointXtract : index: %lu\r\n", index);)

	if (index == epDataInIndex)
		pipe = UHD_Pipe_Alloc(bAddress, epInfo[index].deviceEpNum, UOTGHS_HSTPIPCFG_PTYPE_BLK, UOTGHS_HSTPIPCFG_PTOKEN_IN, epInfo[index].maxPktSize, 0, UOTGHS_HSTPIPCFG_PBK_1_BANK);
	else if (index == epDataOutIndex)
		pipe = UHD_Pipe_Alloc(bAddress, epInfo[index].deviceEpNum, UOTGHS_HSTPIPCFG_PTYPE_BLK, UOTGHS_HSTPIPCFG_PTOKEN_OUT, epInfo[index].maxPktSize, 0, UOTGHS_HSTPIPCFG_PBK_1_BANK);

	// Ensure pipe allocation is okay
	if (pipe == 0)
	{
		TRACE_USBHOST(printf("ADK::EndpointXtract : Pipe allocation failure\r\n");)
		// Enumeration failed, so user should not perform write/read since isConnected will return false
		return;
	}

	epInfo[index].hostPipeNum = pipe;

	bNumEP++;
}

/**
 * \brief Release USB allocated resources (pipes and address).
 *
 * \note Release call is made from USBHost.task() on disconnection events.
 * \note Release call is made from Init() on enumeration failure.
 *
 * \return Always 0.
 */
uint32_t ADK::Release()
{
	// Free allocated host pipes
	UHD_Pipe_Free(epInfo[epDataInIndex].hostPipeNum);
	UHD_Pipe_Free(epInfo[epDataOutIndex].hostPipeNum);

	// Free allocated USB address
	pUsb->GetAddressPool().FreeAddress(bAddress);

	// Must have to be reset to 1
	bNumEP = 1;

	bAddress = 0;
	ready = false;

	return 0;
}

/**
 * \brief Read from ADK.
 *
 * \param nreadbytes Return value containing the number of read bytes.
 * \param datalen Buffer length.
 * \param dataptr Buffer to store the incoming data.
 *
 * \return 0 on success, error code otherwise.
 */
uint32_t ADK::read(uint32_t *nreadbytes, uint32_t datalen, uint8_t *dataptr)
{
	*nreadbytes = datalen;
	return pUsb->inTransfer(bAddress, epInfo[epDataInIndex].deviceEpNum, nreadbytes, dataptr);
}

/**
 * \brief Write to ADK.
 *
 * \param datalen Amount of data to send. This parameter shall not exceed
 * dataptr length.
 * \param dataptr Buffer containing the data to be sent.
 *
 * \return 0 on success, error code otherwise.
 */
uint32_t ADK::write(uint32_t datalen, uint8_t *dataptr)
{
	return pUsb->outTransfer(bAddress, epInfo[epDataOutIndex].deviceEpNum, datalen, dataptr);
}
