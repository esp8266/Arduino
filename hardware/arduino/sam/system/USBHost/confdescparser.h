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

#ifndef CONFDESCPARSER_H_INCLUDED
#define CONFDESCPARSER_H_INCLUDED

#include <stdint.h>
#include "parsetools.h"

/**
 * \class Abstract UsbConfigXtracter definition.
 *
 * \note This class is used for extracting USB endpoint descriptors.
 */
class UsbConfigXtracter
{
public:
	virtual void EndpointXtract(uint32_t conf, uint32_t iface, uint32_t alt, uint32_t proto, const USB_ENDPOINT_DESCRIPTOR *ep) = 0;
};

#define CP_MASK_COMPARE_CLASS			1
#define CP_MASK_COMPARE_SUBCLASS		2
#define CP_MASK_COMPARE_PROTOCOL		4
#define CP_MASK_COMPARE_ALL				7

/**
 * \class ConfigDescParser definition.
 *
 * \note This class is used for parsing configuration descriptors.
 */
template <const uint8_t CLASS_ID, const uint8_t SUBCLASS_ID, const uint8_t PROTOCOL_ID, const uint8_t MASK>
class ConfigDescParser : public USBReadParser
{
	UsbConfigXtracter		*theXtractor;
    MultiValueBuffer		theBuffer;
	MultiByteValueParser	valParser;
	ByteSkipper				theSkipper;
	uint8_t					varBuffer[16 /*sizeof(USB_CONFIGURATION_DESCRIPTOR)*/];

	uint32_t				stateParseDescr;	// ParseDescriptor state

	uint32_t				dscrLen;			// Descriptor length
	uint32_t				dscrType;			// Descriptor type

	bool					isGoodInterface;	// Apropriate interface flag
	uint32_t				confValue;			// Configuration value
	uint32_t				protoValue;			// Protocol value
	uint32_t				ifaceNumber;		// Interface number
	uint32_t				ifaceAltSet;		// Interface alternate settings

	bool ParseDescriptor(uint8_t **pp, uint32_t *pcntdn);

	void PrintHidDescriptor(const USB_HID_DESCRIPTOR *pDesc);

public:
	ConfigDescParser(UsbConfigXtracter *xtractor);
	virtual void Parse(const uint32_t len, const uint8_t *pbuf, const uint32_t &offset);
};

/**
 * \brief ConfigDescParser class constructor.
 *
 * \param xtractor Is saved as ConfigDescParser attribute and later used as a
 * callback for parsing the endpoint descriptors.
 *
 * \note During enumeration stage, all supported USB classes invoke
 * ConfigDescParser with "this" as parameter, meaning that one class is also
 * responsible for parsing its endpoint descriptors. This makes sense since
 * each USB class handles different number of endpoints and configurations.
 * For instance see ADK::Init from ADK class.
 */
template <const uint8_t CLASS_ID, const uint8_t SUBCLASS_ID, const uint8_t PROTOCOL_ID, const uint8_t MASK>
ConfigDescParser<CLASS_ID, SUBCLASS_ID, PROTOCOL_ID, MASK>::ConfigDescParser(UsbConfigXtracter *xtractor) :
	theXtractor(xtractor),
	stateParseDescr(0),
	dscrLen(0),
	dscrType(0)
{
	theBuffer.pValue = varBuffer;
	valParser.Initialize(&theBuffer);
	theSkipper.Initialize(&theBuffer);
};

/**
 * \brief Parse a complete USB configuration descriptor.
 *
 * \param len Buffer length.
 * \param pbuf Buffer containing the configuration descriptor.
 * \param offset Current offset position.
 */
template <const uint8_t CLASS_ID, const uint8_t SUBCLASS_ID, const uint8_t PROTOCOL_ID, const uint8_t MASK>
void ConfigDescParser<CLASS_ID, SUBCLASS_ID, PROTOCOL_ID, MASK>::Parse(const uint32_t len, const uint8_t *pbuf, const uint32_t &offset)
{
	uint32_t	cntdn	= len;
	uint8_t		*p		= (uint8_t*)pbuf;

	while (cntdn)
		if (!ParseDescriptor(&p, &cntdn))
			return;
}

/**
 * \brief Parse a USB configuration descriptor.
 * Takes values for class, subclass, protocol fields in interface descriptor
 * and compare masks for them. When the match is found, calls EndpointXtract
 * passing buffer containing endpoint descriptor.
 *
 * \note This method should not be called directly, use Parse() instead.
 *
 * \param pcntdn Buffer length.
 * \param pp Buffer containing the configuration descriptor.
 *
 * \return true if data remains in the buffer for parsing, false otherwise.
 */
template <const uint8_t CLASS_ID, const uint8_t SUBCLASS_ID, const uint8_t PROTOCOL_ID, const uint8_t MASK>
bool ConfigDescParser<CLASS_ID, SUBCLASS_ID, PROTOCOL_ID, MASK>::ParseDescriptor(uint8_t **pp, uint32_t *pcntdn)
{
	switch (stateParseDescr)
	{
		case 0:
			theBuffer.valueSize = 2;
			valParser.Initialize(&theBuffer);
			stateParseDescr		= 1;
		case 1:
			if (!valParser.Parse(pp, pcntdn))
				return false;
			dscrLen			= *((uint8_t*)theBuffer.pValue);
			dscrType		= *((uint8_t*)theBuffer.pValue + 1);
			stateParseDescr	= 2;
		case 2:
			// This is a sort of hack. Assuming that two bytes are already in the buffer
			//	the pointer is positioned two bytes ahead in order for the rest of descriptor
			//	to be read right after the size and the type fields.
			// This should be used carefuly. varBuffer should be used directly to handle data
			//	in the buffer.
			theBuffer.pValue	= varBuffer + 2;
			stateParseDescr		= 3;
		case 3:
			switch (dscrType)
			{
				case USB_DESCRIPTOR_INTERFACE:
					isGoodInterface = false;
				case USB_DESCRIPTOR_CONFIGURATION:
					theBuffer.valueSize = sizeof(USB_CONFIGURATION_DESCRIPTOR) - 2;
					break;
				case USB_DESCRIPTOR_ENDPOINT:
					theBuffer.valueSize = sizeof(USB_ENDPOINT_DESCRIPTOR) - 2;
					break;
				case HID_DESCRIPTOR_HID:
					theBuffer.valueSize = dscrLen - 2;
					break;
			}
			valParser.Initialize(&theBuffer);
			stateParseDescr		= 4;
		case 4:
			switch (dscrType)
			{
				case USB_DESCRIPTOR_CONFIGURATION:
					if (!valParser.Parse(pp, pcntdn))
						return false;
					confValue = ((USB_CONFIGURATION_DESCRIPTOR*)varBuffer)->bConfigurationValue;
					break;
				case USB_DESCRIPTOR_INTERFACE:
					if (!valParser.Parse(pp, pcntdn))
						return false;
					if ((MASK & CP_MASK_COMPARE_CLASS) && ((USB_INTERFACE_DESCRIPTOR*)varBuffer)->bInterfaceClass != CLASS_ID)
						break;
					if ((MASK & CP_MASK_COMPARE_SUBCLASS) && ((USB_INTERFACE_DESCRIPTOR*)varBuffer)->bInterfaceSubClass != SUBCLASS_ID)
						break;
					if ((MASK & CP_MASK_COMPARE_PROTOCOL) && ((USB_INTERFACE_DESCRIPTOR*)varBuffer)->bInterfaceProtocol != PROTOCOL_ID)
						break;

					isGoodInterface = true;
					ifaceNumber = ((USB_INTERFACE_DESCRIPTOR*)varBuffer)->bInterfaceNumber;
					ifaceAltSet = ((USB_INTERFACE_DESCRIPTOR*)varBuffer)->bAlternateSetting;
					protoValue  = ((USB_INTERFACE_DESCRIPTOR*)varBuffer)->bInterfaceProtocol;
					break;
				case USB_DESCRIPTOR_ENDPOINT:
					if (!valParser.Parse(pp, pcntdn))
						return false;
					if (isGoodInterface)
						if (theXtractor)
							theXtractor->EndpointXtract(confValue, ifaceNumber, ifaceAltSet, protoValue, (USB_ENDPOINT_DESCRIPTOR*)varBuffer);
					break;
				//case HID_DESCRIPTOR_HID:
				//	if (!valParser.Parse(pp, pcntdn))
				//		return false;
				//	PrintHidDescriptor((const USB_HID_DESCRIPTOR*)varBuffer);
				//	break;
				default:
					if (!theSkipper.Skip(pp, pcntdn, dscrLen - 2))
						return false;
			}
			theBuffer.pValue = varBuffer;
			stateParseDescr = 0;
	}
	return true;
}

/**
 * \brief Print HID descriptor.
 *
 * \note TRACE_USBHOST macro must be enabled. See Usb.h for reference.
 *
 * \param pDesc Pointer to HID descriptor.
 */
template <const uint8_t CLASS_ID, const uint8_t SUBCLASS_ID, const uint8_t PROTOCOL_ID, const uint8_t MASK>
void ConfigDescParser<CLASS_ID, SUBCLASS_ID, PROTOCOL_ID, MASK>::PrintHidDescriptor(const USB_HID_DESCRIPTOR *pDesc)
{
	TRACE_USBHOST(printf("ConfigDescParser::PrintHidDescriptor : bDescLength: %d\r\n", pDesc->bLength);)
	TRACE_USBHOST(printf("ConfigDescParser::PrintHidDescriptor : bDescriptorType: %d\r\n", pDesc->bDescriptorType);)
	TRACE_USBHOST(printf("ConfigDescParser::PrintHidDescriptor : bcdHID: %d\r\n", pDesc->bcdHID);)
	TRACE_USBHOST(printf("ConfigDescParser::PrintHidDescriptor : bCountryCode: %d\r\n", pDesc->bCountryCode);)
	TRACE_USBHOST(printf("ConfigDescParser::PrintHidDescriptor : bNumDescriptors: %d\r\n", pDesc->bNumDescriptors);)
	TRACE_USBHOST(printf("ConfigDescParser::PrintHidDescriptor : bDescrType: %d\r\n", pDesc->bDescrType);)
	TRACE_USBHOST(printf("ConfigDescParser::PrintHidDescriptor : wDescriptorLength: %d\r\n", pDesc->wDescriptorLength);)

	for (uint32_t i = 0; i < pDesc->bNumDescriptors; ++i)
	{
		HID_CLASS_DESCRIPTOR_LEN_AND_TYPE	*pLT = (HID_CLASS_DESCRIPTOR_LEN_AND_TYPE*)&(pDesc->bDescrType);

		TRACE_USBHOST(printf("ConfigDescParser::PrintHidDescriptor : bDescrType: %d\r\n", pLT[i].bDescrType);)
		TRACE_USBHOST(printf("ConfigDescParser::PrintHidDescriptor : wDescriptorLength: %d\r\n", pLT[i].wDescriptorLength);)
	}
}

#endif /* CONFDESCPARSER_H_INCLUDED */
