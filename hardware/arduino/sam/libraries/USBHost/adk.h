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

/* Google ADK interface support header */

#ifndef ADK_H_INCLUDED
#define ADK_H_INCLUDED

#include <stdint.h>
#include "usb_ch9.h"
#include "Usb.h"
#include "hid.h"
#include "Arduino.h"
#include "confdescparser.h"

#define ADK_VID   0x18D1
#define ADK_PID   0x2D00
#define ADB_PID   0x2D01

#define XOOM  //enables repeating getProto() and getConf() attempts
              //necessary for slow devices such as Motorola XOOM
              //defined by default, can be commented out to save memory

/* Requests */

#define ADK_GETPROTO      51  //check USB accessory protocol version
#define ADK_SENDSTR       52  //send identifying string
#define ADK_ACCSTART      53  //start device in accessory mode

#define bmREQ_ADK_GET     USB_SETUP_DEVICE_TO_HOST|USB_SETUP_TYPE_VENDOR|USB_SETUP_RECIPIENT_DEVICE
#define bmREQ_ADK_SEND    USB_SETUP_HOST_TO_DEVICE|USB_SETUP_TYPE_VENDOR|USB_SETUP_RECIPIENT_DEVICE

#define ACCESSORY_STRING_MANUFACTURER   0
#define ACCESSORY_STRING_MODEL          1
#define ACCESSORY_STRING_DESCRIPTION    2
#define ACCESSORY_STRING_VERSION        3
#define ACCESSORY_STRING_URI            4
#define ACCESSORY_STRING_SERIAL         5

#define ADK_MAX_ENDPOINTS 3 //endpoint 0, bulk_IN, bulk_OUT

/**
 * \class ADK definition.
 */
class ADK : public USBDeviceConfig, public UsbConfigXtracter
{
private:
	/* ID strings */
	const char* manufacturer;
	const char* model;
	const char* description;
	const char* version;
	const char* uri;
	const char* serial;

	/* ADK proprietary requests */
	uint32_t getProto(uint8_t* adkproto);
	uint32_t sendStr(uint32_t index, const char* str);
	uint32_t switchAcc(void);

protected:
	static const uint32_t epDataInIndex;			// DataIn endpoint index
	static const uint32_t epDataOutIndex;			// DataOUT endpoint index

	/* Mandatory members */
	USBHost		*pUsb;
	uint32_t	bAddress;							// Device USB address
	uint32_t	bConfNum;							// configuration number

	uint32_t	bNumEP;								// total number of EP in the configuration
	bool		ready;

	/* Endpoint data structure describing the device EP */
	EpInfo		epInfo[ADK_MAX_ENDPOINTS];

public:
	ADK(USBHost *pUsb, const char* pmanufacturer,
                  const char* pmodel,
                  const char* pdescription,
                  const char* pversion,
                  const char* puri,
                  const char* pserial);

	// Methods for receiving and sending data
	uint32_t read(uint32_t *nreadbytes, uint32_t datalen, uint8_t *dataptr);
	uint32_t write(uint32_t datalen, uint8_t *dataptr);


	// USBDeviceConfig implementation
	virtual uint32_t Init(uint32_t parent, uint32_t port, uint32_t lowspeed);
	virtual uint32_t Release();
	virtual uint32_t Poll() { return 0; };	// not implemented
	virtual uint32_t GetAddress() { return bAddress; };
	virtual bool isReady() { return ready; };

	// UsbConfigXtracter implementation
	virtual void EndpointXtract(uint32_t conf, uint32_t iface, uint32_t alt, uint32_t proto, const USB_ENDPOINT_DESCRIPTOR *ep);
};

/**
 * \brief Get ADK protocol version.
 *
 * \param adkproto Empty buffer to be filled by getProto (2 bytes) with the ADK
 * protocol version value.
 *
 * \return 0 on success, error code otherwise.
 */
inline uint32_t ADK::getProto(uint8_t* adkproto)
{
	return (pUsb->ctrlReq(bAddress, 0, bmREQ_ADK_GET, ADK_GETPROTO, 0, 0, 0, 2, 2, adkproto, NULL));
}

/**
 * \brief Send ADK string.
 *
 * \param index String index.
 * \param str String to send.
 *
 * \return 0 on success, error code otherwise.
 */
inline uint32_t ADK::sendStr(uint32_t index, const char* str)
{
	return (pUsb->ctrlReq(bAddress, 0, bmREQ_ADK_SEND, ADK_SENDSTR, 0, 0, index, strlen(str) + 1, strlen(str) + 1, (uint8_t*)str, NULL));
}

/**
 * \brief Send a switch to accessory mode request.
 *
 * \return 0 on success, error code otherwise.
 */
inline uint32_t ADK::switchAcc(void)
{
	return (pUsb->ctrlReq(bAddress, 0, bmREQ_ADK_SEND, ADK_ACCSTART, 0, 0, 0, 0, 0, NULL, NULL));
}

#endif /* ADK_H_INCLUDED */
