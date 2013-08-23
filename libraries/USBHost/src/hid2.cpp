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

#include "hid.h"

/**
 * \brief Get HID report descriptor and parse it.
 *
 * \param ep USB device endpoint.
 * \param parser Parser used to decode report.
 *
 * \return 0 on success, error code otherwise.
 */
uint32_t HID::GetReportDescr(uint32_t ep, USBReadParser *parser)
{
	const uint32_t	constBufLen = 64;
	uint8_t			buf[constBufLen];

	return (pUsb->ctrlReq(bAddress, ep, bmREQ_HIDREPORT, USB_REQUEST_GET_DESCRIPTOR, 0x00,
			HID_DESCRIPTOR_REPORT, 0x0000, 128, constBufLen, buf, (USBReadParser*)parser));
}

/**
 * \brief Set HID report descriptor.
 *
 * \param ep USB device endpoint.
 * \param iface Interface number.
 * \param report_type HID report type.
 * \param report_id HID report ID.
 * \param nbytes Buffer length.
 * \param dataptr Buffer containing the HID report to send.
 *
 * \return 0 on success, error code otherwise.
 */
uint32_t HID::SetReport(uint32_t ep, uint32_t iface, uint32_t report_type, uint32_t report_id, uint32_t nbytes, uint8_t* dataptr)
{
    return (pUsb->ctrlReq(bAddress, ep, bmREQ_HIDOUT, HID_REQUEST_SET_REPORT, report_id, report_type, iface, nbytes, nbytes, dataptr, NULL));
}

/**
 * \brief Get HID report descriptor.
 *
 * \param ep USB device endpoint.
 * \param iface Interface number.
 * \param report_type HID report type.
 * \param report_id HID report ID.
 * \param nbytes Buffer length.
 * \param dataptr Buffer containing the HID report to send.
 *
 * \return 0 on success, error code otherwise.
 */
uint32_t HID::GetReport(uint32_t ep, uint32_t iface, uint32_t report_type, uint32_t report_id, uint32_t nbytes, uint8_t* dataptr)
{
	return (pUsb->ctrlReq(bAddress, ep, bmREQ_HIDIN, HID_REQUEST_GET_REPORT, report_id, report_type, iface, nbytes, nbytes, dataptr, NULL));
}

/**
 * \brief Get HID idle status.
 *
 * \param iface Interface number.
 * \param report_id HID report ID.
 * \param dataptr Buffer to receive data. Size must be >= 1.
 *
 * \return 0 on success, error code otherwise.
 */
uint32_t HID::GetIdle(uint32_t iface, uint32_t report_id, uint8_t* dataptr)
{
    return (pUsb->ctrlReq(bAddress, 0, bmREQ_HIDIN, HID_REQUEST_GET_IDLE, report_id, 0, iface, 0x0001, 0x0001, dataptr, NULL));
}

/**
 * \brief Set HID idle status.
 *
 * \param iface Interface number.
 * \param report_id HID report ID.
 * \param duration Status duration.
 *
 * \return 0 on success, error code otherwise.
 */
uint32_t HID::SetIdle(uint32_t iface, uint32_t report_id, uint32_t duration)
{
    return (pUsb->ctrlReq(bAddress, 0, bmREQ_HIDOUT, HID_REQUEST_SET_IDLE, report_id, duration, iface, 0x0000, 0x0000, NULL, NULL));
}

/**
 * \brief Set HID protocol.
 *
 * \param iface Interface number.
 * \param protocol Protocol value.
 *
 * \return 0 on success, error code otherwise.
 */
uint32_t HID::SetProtocol(uint32_t iface, uint32_t protocol)
{
	return (pUsb->ctrlReq(bAddress, 0, bmREQ_HIDOUT, HID_REQUEST_SET_PROTOCOL, protocol, 0x00, iface, 0x0000, 0x0000, NULL, NULL));
}

/**
 * \brief Get HID protocol.
 *
 * \param iface Interface number.
 * \param dataptr Buffer used to store protocol value. Size must be >= 1.
 *
 * \return 0 on success, error code otherwise.
 */
uint32_t HID::GetProtocol(uint32_t iface, uint8_t* dataptr)
{
	return (pUsb->ctrlReq(bAddress, 0, bmREQ_HIDIN, HID_REQUEST_GET_PROTOCOL, 0x00, 0x00, iface, 0x0001, 0x0001, dataptr, NULL));
}

/**
 * \brief Print HID descriptor.
 *
 * \note TRACE_USBHOST macro must be enabled. See Usb.h for reference.
 *
 * \param pDesc Pointer to HID descriptor.
 */
void HID::PrintHidDescriptor(const USB_HID_DESCRIPTOR *pDesc)
{
	TRACE_USBHOST(printf("HID::PrintHidDescriptor : bDescLength: %d\r\n", pDesc->bLength);)
	TRACE_USBHOST(printf("HID::PrintHidDescriptor : bDescriptorType: %d\r\n", pDesc->bDescriptorType);)
	TRACE_USBHOST(printf("HID::PrintHidDescriptor : bcdHID: %d\r\n", pDesc->bcdHID);)
	TRACE_USBHOST(printf("HID::PrintHidDescriptor : bCountryCode: %d\r\n", pDesc->bCountryCode);)
	TRACE_USBHOST(printf("HID::PrintHidDescriptor : bNumDescriptors: %d\r\n", pDesc->bNumDescriptors);)
	TRACE_USBHOST(printf("HID::PrintHidDescriptor : bDescrType: %d\r\n", pDesc->bDescrType);)
	TRACE_USBHOST(printf("HID::PrintHidDescriptor : wDescriptorLength: %d\r\n", pDesc->wDescriptorLength);)
}
