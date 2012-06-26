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

#ifndef HIDBOOT_H_INCLUDED
#define HIDBOOT_H_INCLUDED

#include <stdint.h>
#include "usb_ch9.h"
#include "Usb.h"
#include "hid.h"
#include "Arduino.h"
#include "confdescparser.h"

#define KEY_SPACE					0x2c
#define KEY_ZERO					0x27
#define KEY_ZERO2					0x62
#define KEY_ENTER					0x28
#define KEY_PERIOD					0x63

/**
 * \brief MOUSEINFO definition.
 */
struct MOUSEINFO
{
	struct
	{
		uint8_t		bmLeftButton	: 1;
		uint8_t		bmRightButton	: 1;
		uint8_t		bmMiddleButton	: 1;
		uint8_t		bmDummy			: 1;
	};
	int8_t			dX;
	int8_t			dY;
};

/**
 * \class MouseReportParser definition.
 */
class MouseReportParser : public HIDReportParser
{
	union
	{
		MOUSEINFO	mouseInfo;
		uint8_t		bInfo[3];
	}	prevState;

public:
	virtual void Parse(HID *hid, bool is_rpt_id, uint32_t len, uint8_t *buf);

protected:
	virtual void OnMouseMove		(MOUSEINFO *mi)	{};
	virtual void OnLeftButtonUp		(MOUSEINFO *mi)	{};
	virtual void OnLeftButtonDown	(MOUSEINFO *mi)	{};
	virtual void OnRightButtonUp	(MOUSEINFO *mi)	{};
	virtual void OnRightButtonDown	(MOUSEINFO *mi)	{};
	virtual void OnMiddleButtonUp	(MOUSEINFO *mi)	{};
	virtual void OnMiddleButtonDown	(MOUSEINFO *mi)	{};
};

/**
 * \brief MODIFIERKEYS definition.
 */
struct MODIFIERKEYS
{
	uint8_t		bmLeftCtrl		: 1;
	uint8_t		bmLeftShift		: 1;
	uint8_t		bmLeftAlt		: 1;
	uint8_t		bmLeftGUI		: 1;
	uint8_t		bmRightCtrl		: 1;
	uint8_t		bmRightShift	: 1;
	uint8_t		bmRightAlt		: 1;
	uint8_t		bmRightGUI		: 1;
};

/**
 * \brief KBDINFO definition.
 */
struct KBDINFO
{
	struct
	{
		uint8_t		bmLeftCtrl		: 1;
		uint8_t		bmLeftShift		: 1;
		uint8_t		bmLeftAlt		: 1;
		uint8_t		bmLeftGUI		: 1;
		uint8_t		bmRightCtrl		: 1;
		uint8_t		bmRightShift	: 1;
		uint8_t		bmRightAlt		: 1;
		uint8_t		bmRightGUI		: 1;
	};
	uint8_t			bReserved;
	uint8_t			Keys[6];
};

/**
 * \brief KBDLEDS definition.
 */
struct KBDLEDS
{
	uint8_t		bmNumLock		: 1;
	uint8_t		bmCapsLock		: 1;
	uint8_t		bmScrollLock	: 1;
	uint8_t		bmCompose		: 1;
	uint8_t		bmKana			: 1;
	uint8_t		bmReserved		: 3;
};

#define KEY_NUM_LOCK				0x53

// Clear compiler warning
#ifdef KEY_CAPS_LOCK
#undef KEY_CAPS_LOCK
#endif

#define KEY_CAPS_LOCK				0x39
#define KEY_SCROLL_LOCK				0x47

/**
 * \class KeyboardReportParser definition.
 */
class KeyboardReportParser : public HIDReportParser
{
	static const uint8_t numKeys[];
	static const uint8_t symKeysUp[];
	static const uint8_t symKeysLo[];
	static const uint8_t padKeys[];

protected:
	union
	{
		KBDINFO		kbdInfo;
		uint8_t		bInfo[sizeof(KBDINFO)];
	}	prevState;

	union
	{
		KBDLEDS		kbdLeds;
		uint8_t		bLeds;
	}	kbdLockingKeys;

	uint8_t OemToAscii(uint8_t mod, uint8_t key);

public:
	KeyboardReportParser() { kbdLockingKeys.bLeds = 0; };

	virtual void Parse(HID *hid, bool is_rpt_id, uint32_t len, uint8_t *buf);

protected:
	uint8_t HandleLockingKeys(HID* hid, uint8_t key);

	virtual void OnKeyDown	(uint8_t mod, uint8_t key)	{};
	virtual void OnKeyUp	(uint8_t mod, uint8_t key)	{};
};

#define totalEndpoints						2

#define HID_MAX_HID_CLASS_DESCRIPTORS		5

/**
 * \class HIDBoot definition.
 */
template <const uint8_t BOOT_PROTOCOL>
class HIDBoot : public HID
{
	EpInfo		epInfo[totalEndpoints];

	HIDReportParser	*pRptParser;

	uint32_t	bConfNum;				// configuration number
	uint32_t	bIfaceNum;				// Interface Number
	uint32_t	bNumIface;				// number of interfaces in the configuration
	uint32_t	bNumEP;					// total number of EP in the configuration
	uint32_t	qNextPollTime;			// next poll time
	bool		bPollEnable;			// poll enable flag

	void Initialize();

	virtual HIDReportParser* GetReportParser(uint32_t id) { return pRptParser; };

public:
	HIDBoot(USBHost *p);

	virtual bool SetReportParser(uint32_t id, HIDReportParser *prs) { pRptParser = prs; return true; };

	// USBDeviceConfig implementation
	virtual uint32_t Init(uint32_t parent, uint32_t port, uint32_t lowspeed);
	virtual uint32_t Release();
	virtual uint32_t Poll();
	virtual uint32_t GetAddress() { return bAddress; };

	// UsbConfigXtracter implementation
	virtual void EndpointXtract(uint32_t conf, uint32_t iface, uint32_t alt, uint32_t proto, const USB_ENDPOINT_DESCRIPTOR *ep);
};

/**
 * \brief HIDBoot class constructor.
 */
template <const uint8_t BOOT_PROTOCOL>
HIDBoot<BOOT_PROTOCOL>::HIDBoot(USBHost *p) :
		HID(p),
		pRptParser(NULL),
		qNextPollTime(0),
		bPollEnable(false)
{
	Initialize();

	if (pUsb)
		pUsb->RegisterDeviceClass(this);
}

/**
 * \brief Initialize HIDBoot class.
 */
template <const uint8_t BOOT_PROTOCOL>
void HIDBoot<BOOT_PROTOCOL>::Initialize()
{
	for (uint32_t i = 0; i < totalEndpoints; ++i)
	{
		epInfo[i].deviceEpNum	= 0;
		epInfo[i].hostPipeNum	= 0;
		epInfo[i].maxPktSize	= (i) ? 0 : 8;
		epInfo[i].epAttribs		= 0;
		epInfo[i].bmNakPower	= (i) ? USB_NAK_NOWAIT : USB_NAK_MAX_POWER;
	}

	bNumEP		= 1;
	bNumIface	= 0;
	bConfNum	= 0;
}

/**
 * \brief Initialize connection to an HID device.
 *
 * \param parent USB device address of the Parent device.
 * \param port USB device base address.
 * \param lowspeed USB device speed.
 *
 * \return 0 on success, error code otherwise.
 */
template <const uint8_t BOOT_PROTOCOL>
uint32_t HIDBoot<BOOT_PROTOCOL>::Init(uint32_t parent, uint32_t port, uint32_t lowspeed)
{
	const uint32_t constBufSize = sizeof(USB_DEVICE_DESCRIPTOR);

	uint8_t		buf[constBufSize];
	uint32_t	rcode = 0;
	UsbDevice	*p = 0;
	EpInfo		*oldep_ptr = 0;
	uint32_t	len = 0;

	uint32_t	num_of_conf = 0;	// number of configurations

	AddressPool	&addrPool = pUsb->GetAddressPool();

	TRACE_USBHOST(printf("HIDBoot::Init\r\n");)

	if (bAddress)
		return USB_ERROR_CLASS_INSTANCE_ALREADY_IN_USE;

	// Get pointer to pseudo device with address 0 assigned
	p = addrPool.GetUsbDevicePtr(0);

	if (!p)
		return USB_ERROR_ADDRESS_NOT_FOUND_IN_POOL;

	if (!p->epinfo)
	{
		TRACE_USBHOST(printf("HIDBoot::Init : epinfo is null!\r\n");)
		return USB_ERROR_EPINFO_IS_NULL;
	}

	// Save old pointer to EP_RECORD of address 0
	oldep_ptr = p->epinfo;

	// Temporary assign new pointer to epInfo to p->epinfo in order to avoid toggle inconsistence
	p->epinfo = epInfo;

	p->lowspeed = lowspeed;

	// Get device descriptor
	rcode = pUsb->getDevDescr(0, 0, 8, (uint8_t*)buf);

	if (!rcode)
		len = (buf[0] > constBufSize) ? constBufSize : buf[0];

	if (rcode)
	{
		// Restore p->epinfo
		p->epinfo = oldep_ptr;

		goto FailGetDevDescr;
	}

	// Restore p->epinfo
	p->epinfo = oldep_ptr;

	// Allocate new address according to device class
	bAddress = addrPool.AllocAddress(parent, false, port);

	if (!bAddress)
		return USB_ERROR_OUT_OF_ADDRESS_SPACE_IN_POOL;

	// Extract Max Packet Size from the device descriptor
	epInfo[0].maxPktSize = (uint8_t)((USB_DEVICE_DESCRIPTOR*)buf)->bMaxPacketSize0;

	// Assign new address to the device
	rcode = pUsb->setAddr(0, 0, bAddress);

	if (rcode)
	{
		p->lowspeed = false;
		addrPool.FreeAddress(bAddress);
		bAddress = 0;
		TRACE_USBHOST(printf("HIDBoot::Init : setAddr failed with rcode %lu\r\n", rcode);)
		return rcode;
	}

	TRACE_USBHOST(printf("HIDBoot::Init : device address is now %lu\r\n", bAddress);)

	p->lowspeed = false;

	p = addrPool.GetUsbDevicePtr(bAddress);

	if (!p)
		return USB_ERROR_ADDRESS_NOT_FOUND_IN_POOL;

	p->lowspeed = lowspeed;

	if (len)
		rcode = pUsb->getDevDescr(bAddress, 0, len, (uint8_t*)buf);

	if(rcode)
		goto FailGetDevDescr;

	num_of_conf = ((USB_DEVICE_DESCRIPTOR*)buf)->bNumConfigurations;

	// Assign epInfo to epinfo pointer
	rcode = pUsb->setEpInfoEntry(bAddress, 1, epInfo);

	if (rcode)
		goto FailSetDevTblEntry;

	TRACE_USBHOST(printf("HIDBoot::Init : number of configuration is %lu\r\n", num_of_conf);)

	for (uint32_t i = 0; i < num_of_conf; ++i)
	{
		ConfigDescParser<
			USB_CLASS_HID,
			HID_BOOT_INTF_SUBCLASS,
			BOOT_PROTOCOL,
			CP_MASK_COMPARE_ALL>	confDescrParser(this);

		rcode = pUsb->getConfDescr(bAddress, 0, i, &confDescrParser);

		if (bNumEP > 1)
			break;
	}

	if (bNumEP < 2)
		return USB_DEV_CONFIG_ERROR_DEVICE_NOT_SUPPORTED;

	TRACE_USBHOST(printf("HIDBoot::Init : bAddress: %lu\r\n", bAddress);)
	TRACE_USBHOST(printf("HIDBoot::Init : bNumEP: %lu\r\n", bNumEP);)

	// Assign epInfo to epinfo pointer
	rcode = pUsb->setEpInfoEntry(bAddress, bNumEP, epInfo);

	TRACE_USBHOST(printf("HIDBoot::Init : bConfNum: %lu\r\n", bConfNum);)

	// Set Configuration Value
	rcode = pUsb->setConf(bAddress, 0, bConfNum);

	if (rcode)
		goto FailSetConfDescr;

	TRACE_USBHOST(printf("HIDBoot::Init : bIfaceNum: %lu\r\n", bIfaceNum);)

	rcode = SetProtocol(bIfaceNum, HID_BOOT_PROTOCOL);

	if (rcode)
		goto FailSetProtocol;

	if (BOOT_PROTOCOL == 1)
	{
		rcode = SetIdle(bIfaceNum, 0, 0);

		if (rcode)
			goto FailSetIdle;
	}

	TRACE_USBHOST(printf("HIDBoot::Init : HID device configured successfully\r\n");)

	bPollEnable = true;
	return 0;

FailGetDevDescr:
	TRACE_USBHOST(printf("HIDBoot::Init getDevDescr : ");)
	goto Fail;

FailSetDevTblEntry:
	TRACE_USBHOST(printf("HIDBoot::Init setDevTblEn : ");)
	goto Fail;

FailSetProtocol:
	TRACE_USBHOST(printf("HIDBoot::Init SetProto : ");)
	goto Fail;

FailSetIdle:
	TRACE_USBHOST(printf("HIDBoot::Init SetIdle : ");)
	goto Fail;

FailSetConfDescr:
	TRACE_USBHOST(printf("HIDBoot::Init setConf : ");)
	goto Fail;

Fail:
	TRACE_USBHOST(printf("error code: %lu\r\n", rcode);)
	Release();
	return rcode;
}

/**
 * \brief Extract interrupt-IN endpoint information from configuration
 * descriptor.
 *
 * \param conf Configuration number.
 * \param iface Interface number.
 * \param alt Alternate setting.
 * \param proto Protocol version used.
 * \param pep Pointer to endpoint descriptor.
 */
template <const uint8_t BOOT_PROTOCOL>
void HIDBoot<BOOT_PROTOCOL>::EndpointXtract(uint32_t conf, uint32_t iface, uint32_t alt, uint32_t proto, const USB_ENDPOINT_DESCRIPTOR *pep)
{
	// If the first configuration satisfies, the others are not considered.
	if (bNumEP > 1 && conf != bConfNum)
		return;

	bConfNum = conf;
	bIfaceNum = iface;

	uint32_t index = 0;
	uint32_t pipe = 0;

	if ((pep->bmAttributes & 0x03) == 3 && (pep->bEndpointAddress & 0x80) == 0x80)
	{
		index = epInterruptInIndex;

		// Fill in the endpoint info structure
		epInfo[index].deviceEpNum		= (pep->bEndpointAddress & 0x0F);
		epInfo[index].maxPktSize	= (uint8_t)pep->wMaxPacketSize;
		epInfo[index].epAttribs		= 0;

		TRACE_USBHOST(printf("HIDBoot::EndpointXtract : Found new endpoint\r\n");)
		TRACE_USBHOST(printf("HIDBoot::EndpointXtract : deviceEpNum: %lu\r\n", epInfo[index].deviceEpNum);)
		TRACE_USBHOST(printf("HIDBoot::EndpointXtract : maxPktSize: %lu\r\n", epInfo[index].maxPktSize);)
		TRACE_USBHOST(printf("HIDBoot::EndpointXtract : index: %lu\r\n", index);)

		// Ensure pipe allocation is okay
		pipe = UHD_Pipe_Alloc(bAddress, epInfo[index].deviceEpNum, UOTGHS_HSTPIPCFG_PTYPE_INTRPT, UOTGHS_HSTPIPCFG_PTOKEN_IN, epInfo[index].maxPktSize, 10, UOTGHS_HSTPIPCFG_PBK_1_BANK);
		if (pipe == 0)
		{
			TRACE_USBHOST(printf("HIDBoot::EndpointXtract : Pipe allocation failure\r\n");)
			// Enumeration failed
			return;
		}

		epInfo[index].hostPipeNum = pipe;

		bNumEP++;
	}
}

/**
 * \brief Release USB allocated resources (pipes and address).
 *
 * \note Release call is made from USBHost.task() on disconnection events.
 * \note Release call is made from Init() on enumeration failure.
 *
 * \return Always 0.
 */
template <const uint8_t BOOT_PROTOCOL>
uint32_t HIDBoot<BOOT_PROTOCOL>::Release()
{
	// Free allocated host pipes
	UHD_Pipe_Free(epInfo[epInterruptInIndex].hostPipeNum);

	// Free allocated USB address
	pUsb->GetAddressPool().FreeAddress(bAddress);

	bConfNum			= 0;
	bIfaceNum			= 0;
	bNumEP				= 1;
	bAddress			= 0;
	qNextPollTime		= 0;
	bPollEnable			= false;

	return 0;
}

/**
 * \brief Poll USB device activity.
 *
 * \note Poll call is periodically made from USBHost.task().
 *
 * \return 0 on success, error code otherwise.
 */
template <const uint8_t BOOT_PROTOCOL>
uint32_t HIDBoot<BOOT_PROTOCOL>::Poll()
{
	uint32_t rcode = 0;

	if (!bPollEnable)
		return 0;

	if (qNextPollTime <= millis())
	{
		qNextPollTime = millis() + 10;

		const uint32_t const_buff_len = 16;
		uint8_t buf[const_buff_len];

		uint32_t read = epInfo[epInterruptInIndex].maxPktSize;

		rcode = pUsb->inTransfer(bAddress, epInfo[epInterruptInIndex].deviceEpNum, &read, buf);

		if (rcode)
		{
			return rcode;
		}

		if (pRptParser)
			pRptParser->Parse((HID*)this, 0, (uint32_t)read, buf);
	}

	return rcode;
}

#endif /* HIDBOOT_H_INCLUDED */
