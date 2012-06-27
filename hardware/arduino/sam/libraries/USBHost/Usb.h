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

#ifndef USB_H_INCLUDED
#define USB_H_INCLUDED

//#define TRACE_USBHOST(x)	x
#define TRACE_USBHOST(x)

#include <stdint.h>
#include "usb_ch9.h"
#include "address.h"

/* Common setup data constant combinations  */
#define bmREQ_GET_DESCR     USB_SETUP_DEVICE_TO_HOST|USB_SETUP_TYPE_STANDARD|USB_SETUP_RECIPIENT_DEVICE     // Get descriptor request type
#define bmREQ_SET           USB_SETUP_HOST_TO_DEVICE|USB_SETUP_TYPE_STANDARD|USB_SETUP_RECIPIENT_DEVICE     // Set request type for all but 'set feature' and 'set interface'
#define bmREQ_CL_GET_INTF   USB_SETUP_DEVICE_TO_HOST|USB_SETUP_TYPE_CLASS|USB_SETUP_RECIPIENT_INTERFACE     // Get interface request type

// USB Device Classes
#define USB_CLASS_USE_CLASS_INFO	0x00	// Use Class Info in the Interface Descriptors
#define USB_CLASS_AUDIO				0x01	// Audio
#define USB_CLASS_COM_AND_CDC_CTRL	0x02	// Communications and CDC Control
#define USB_CLASS_HID				0x03	// HID
#define USB_CLASS_PHYSICAL			0x05	// Physical
#define USB_CLASS_IMAGE				0x06	// Image
#define USB_CLASS_PRINTER			0x07	// Printer
#define USB_CLASS_MASS_STORAGE		0x08	// Mass Storage
#define USB_CLASS_HUB				0x09	// Hub
#define USB_CLASS_CDC_DATA			0x0a	// CDC-Data
#define USB_CLASS_SMART_CARD		0x0b	// Smart-Card
#define USB_CLASS_CONTENT_SECURITY	0x0d	// Content Security
#define USB_CLASS_VIDEO				0x0e	// Video
#define USB_CLASS_PERSONAL_HEALTH	0x0f	// Personal Healthcare
#define USB_CLASS_DIAGNOSTIC_DEVICE	0xdc	// Diagnostic Device
#define USB_CLASS_WIRELESS_CTRL		0xe0	// Wireless Controller
#define USB_CLASS_MISC				0xef	// Miscellaneous
#define USB_CLASS_APP_SPECIFIC		0xfe	// Application Specific
#define USB_CLASS_VENDOR_SPECIFIC	0xff	// Vendor Specific

// Additional Error Codes
#define USB_DEV_CONFIG_ERROR_DEVICE_NOT_SUPPORTED	0xD1
#define USB_DEV_CONFIG_ERROR_DEVICE_INIT_INCOMPLETE	0xD2
#define USB_ERROR_UNABLE_TO_REGISTER_DEVICE_CLASS	0xD3
#define USB_ERROR_OUT_OF_ADDRESS_SPACE_IN_POOL		0xD4
#define USB_ERROR_HUB_ADDRESS_OVERFLOW				0xD5
#define USB_ERROR_ADDRESS_NOT_FOUND_IN_POOL			0xD6
#define USB_ERROR_EPINFO_IS_NULL					0xD7
#define USB_ERROR_INVALID_ARGUMENT					0xD8
#define USB_ERROR_CLASS_INSTANCE_ALREADY_IN_USE		0xD9
#define USB_ERROR_INVALID_MAX_PKT_SIZE				0xDA
#define USB_ERROR_EP_NOT_FOUND_IN_TBL				0xDB
#define USB_ERROR_TRANSFER_TIMEOUT					0xFF

#define USB_XFER_TIMEOUT		5000    //USB transfer timeout in milliseconds, per section 9.2.6.1 of USB 2.0 spec
//#define USB_NAK_LIMIT			32000   //NAK limit for a transfer. 0 means NAKs are not counted
#define USB_RETRY_LIMIT			3       //retry limit for a transfer
#define USB_SETTLE_DELAY		200     //settle delay in milliseconds

#define USB_NUMDEVICES			16		//number of USB devices
//#define HUB_MAX_HUBS			7		// maximum number of hubs that can be attached to the host controller
#define HUB_PORT_RESET_DELAY	20		// hub port reset delay 10 ms recomended, can be up to 20 ms

/* USB state machine states */
#define USB_STATE_MASK                                      0xf0

#define USB_STATE_DETACHED                                  0x10
#define USB_DETACHED_SUBSTATE_INITIALIZE                    0x11
#define USB_DETACHED_SUBSTATE_WAIT_FOR_DEVICE               0x12
#define USB_DETACHED_SUBSTATE_ILLEGAL                       0x13
#define USB_ATTACHED_SUBSTATE_SETTLE                        0x20
#define USB_ATTACHED_SUBSTATE_RESET_DEVICE                  0x30
#define USB_ATTACHED_SUBSTATE_WAIT_RESET_COMPLETE           0x40
#define USB_ATTACHED_SUBSTATE_WAIT_SOF                      0x50
#define USB_ATTACHED_SUBSTATE_GET_DEVICE_DESCRIPTOR_SIZE    0x60
#define USB_STATE_ADDRESSING                                0x70
#define USB_STATE_CONFIGURING                               0x80
#define USB_STATE_RUNNING                                   0x90
#define USB_STATE_ERROR                                     0xa0
#define USB_STATE_MASK                                      0xf0

// USB Setup Packet Structure
typedef struct
{
    union
	{                          // offset   description
        uint8_t bmRequestType;         //   0      Bit-map of request type
        struct
		{
            uint8_t    recipient:  5;  //          Recipient of the request
            uint8_t    type:       2;  //          Type of request
            uint8_t    direction:  1;  //          Direction of data X-fer
        };
    } ReqType_u;
    uint8_t    bRequest;               //   1      Request
    union
	{
        uint16_t    wValue;             //   2      Depends on bRequest
        struct
		{
			uint8_t    wValueLo;
			uint8_t    wValueHi;
        };
    } wVal_u;
    uint16_t    wIndex;                 //   4      Depends on bRequest
    uint16_t    wLength;                //   6      Depends on bRequest
} SETUP_PKT, *PSETUP_PKT;

/**
 * \class USBReadParser
 *
 * \brief Base class used for USB descriptor parsing.
 */
class USBReadParser
{
public:
	virtual void Parse(const uint32_t len, const uint8_t *pbuf, const uint32_t &offset) = 0;
};

/**
 * \class USBDeviceConfig
 *
 * \brief Device configuration class used for managing device life cycle.
 */
class USBDeviceConfig
{
public:
	//! @brief Perform final enumeration stage.
	virtual uint32_t Init(uint32_t parent, uint32_t port, uint32_t lowspeed) = 0;

	//! @brief Free USB allocated resources (pipes and address).
	virtual uint32_t Release()		= 0;

	//! @brief Poll USB device. Call is made for each connected USB device on USBHost.task() call.
	virtual uint32_t Poll()			= 0;

	//! @brief Retrieve USB device address.
	virtual uint32_t GetAddress()	= 0;
};

/**
 * \class USBHost
 *
 * \brief Main USB host class.
 */
class USBHost
{
	AddressPoolImpl<USB_NUMDEVICES>		addrPool;
	USBDeviceConfig*					devConfig[USB_NUMDEVICES];
	uint32_t							devConfigIndex;
	uint32_t							bmHubPre;

    public:
        USBHost(void);

		//void SetHubPreMask()	{ bmHubPre |= bmHUBPRE; };
		//void ResetHubPreMask()	{ bmHubPre &= (~bmHUBPRE); };

		AddressPool& GetAddressPool()
		{
			return (AddressPool&)addrPool;
		};

		uint32_t RegisterDeviceClass(USBDeviceConfig *pdev)
		{
			for (uint32_t i = 0; i < USB_NUMDEVICES; ++i)
			{
				if (!devConfig[i])
				{
					devConfig[i] = pdev;
					return 0;
				}
			}
			return USB_ERROR_UNABLE_TO_REGISTER_DEVICE_CLASS;
		};

		void ForEachUsbDevice(UsbDeviceHandleFunc pfunc)
		{
			addrPool.ForEachUsbDevice(pfunc);
		};

		uint32_t getUsbTaskState(void);
        void setUsbTaskState(uint32_t state);

		EpInfo* getEpInfoEntry(uint32_t addr, uint32_t ep);
		uint32_t setEpInfoEntry(uint32_t addr, uint32_t epcount, EpInfo* eprecord_ptr);

		/* Control requests */
        uint32_t getDevDescr(uint32_t addr, uint32_t ep, uint32_t nbytes, uint8_t* dataptr);
        uint32_t getConfDescr(uint32_t addr, uint32_t ep, uint32_t nbytes, uint32_t conf, uint8_t* dataptr);
		uint32_t getConfDescr(uint32_t addr, uint32_t ep, uint32_t conf, USBReadParser *p);
		uint32_t getStrDescr(uint32_t addr, uint32_t ep, uint32_t nbytes, uint8_t index, uint16_t langid, uint8_t* dataptr);
		uint32_t setAddr(uint32_t oldaddr, uint32_t ep, uint32_t newaddr);
		uint32_t setConf(uint32_t addr, uint32_t ep, uint32_t conf_value);
		uint32_t ctrlReq(uint32_t addr, uint32_t ep, uint8_t bmReqType, uint8_t bRequest, uint8_t wValLo, uint8_t wValHi,
						uint16_t wInd, uint16_t total, uint32_t nbytes, uint8_t* dataptr, USBReadParser *p);

        /* Transfer requests */
        uint32_t inTransfer(uint32_t addr, uint32_t ep, uint32_t *nbytesptr, uint8_t* data);
		uint32_t outTransfer(uint32_t addr, uint32_t ep, uint32_t nbytes, uint8_t* data);
        uint32_t dispatchPkt(uint32_t token, uint32_t ep, uint32_t nak_limit);

        void Task(void);

		uint32_t DefaultAddressing(uint32_t parent, uint32_t port, uint32_t lowspeed);
		uint32_t Configuring(uint32_t parent, uint32_t port, uint32_t lowspeed);
		uint32_t ReleaseDevice(uint32_t addr);

    private:
        void init();
		uint32_t setPipeAddress(uint32_t addr, uint32_t ep, EpInfo **ppep, uint32_t &nak_limit);
		uint32_t OutTransfer(EpInfo *pep, uint32_t nak_limit, uint32_t nbytes, uint8_t *data);
		uint32_t InTransfer(EpInfo *pep, uint32_t nak_limit, uint32_t *nbytesptr, uint8_t* data);
};

#endif /* USB_H_INCLUDED */
