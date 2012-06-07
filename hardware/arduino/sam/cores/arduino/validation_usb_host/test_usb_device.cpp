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

#include "variant.h"
#include <stdio.h>
#include "descriptor_parser.h"

#define LOBYTE(x) ((char*)(&(x)))[0]
#define HIBYTE(x) ((char*)(&(x)))[1]
#define BUFSIZE 256
#define DEVADDR 1

#define getReportDescr(addr, ep, nbytes, buf) Usb.ctrlReq(addr, ep, bmREQ_HIDREPORT, USB_REQUEST_GET_DESCRIPTOR, 0x00, HID_DESCRIPTOR_REPORT, 0x0000, nbytes, buf)

USBHost Usb;

/* Forward declarations */
void HIDreport_parse(uint8_t* buffer, uint8_t pkt_size);
void print_mainbitfield(uint8_t byte_toparse);
void printunkdescr(uint8_t* descr_ptr);
void printhid_descr(uint8_t* descr_ptr);
void printepdescr(uint8_t* descr_ptr);
void printintfdescr(uint8_t* descr_ptr);
void printconfdescr(uint8_t* descr_ptr);
uint32_t getconfdescr(uint32_t addr, uint32_t conf);
void classname_parse(byte class_number);
uint32_t getstrdescr(uint32_t addr, uint32_t idx);
uint32_t getdevdescr(uint32_t addr);

_Pragma("pack(1)")

typedef struct
{
	uint8_t bDescriptorType;
	uint16_t wDescriptorLength;
} HID_CLASS_DESCRIPTOR;

_Pragma("pack()")

void setup()
{
	cpu_irq_enable();
	printf("\r\nProgram started:\r\n");
	delay(200);
}

void loop()
{
	uint8_t tmpbyte = 0;

	Usb.Task();

	// If state configuring or higher
	if (Usb.getUsbTaskState() >= USB_STATE_CONFIGURING)
	{
		// Printing device descriptor
		printf("\r\nDevice addressed...\r\n");
		printf("Requesting device descriptor.\r\n");

		// Number of configurations, 0 if error
		tmpbyte = getdevdescr(DEVADDR);
		if (tmpbyte == 0)
		{
			printf("\r\nDevice descriptor cannot be retrieved. Program Halted!\r\n");
			while (1)
				;
		}

		// Print configuration descriptors for all configurations
		for (uint8_t i = 0; i < tmpbyte; i++)
		{
			getconfdescr(DEVADDR, i);
		}

		while (1)
			;
	}
}

/**
 * Get device descriptor.
 * Return number of configurations or zero on error.
 */
uint32_t getdevdescr(uint32_t addr)
{
	USB_DEVICE_DESCRIPTOR buf;
	uint32_t rcode;

	rcode = Usb.getDevDescr(addr, 0, 0x12, (uint8_t*)&buf);
	if (rcode)
	{
		printf("\r\n%s %lu\r\n", rcode_error_msg, rcode);
		return 0;
	}
	printf("\r\nDevice descriptor: \r\n");

	// Descriptor length
	printf("%s0x%x\r\n", descr_len, buf.bLength);

	// Descriptor type
	printf("%s0x%x\r\n", descr_type, buf.bDescriptorType);

	// USB Version
	printf("USB version:\t\t0x%x\r\n", buf.bcdUSB);

	// Device class
	printf("%s0x%x", class_str, buf.bDeviceClass);
	classname_parse(buf.bDeviceClass);
	printf("\r\n");

	// Device Subclass
	printf("%s0x%x\r\n", subclass_str, buf.bDeviceSubClass);

	// Device Protocol
	printf("%s0x%x\r\n", protocol_str, buf.bDeviceProtocol);

	// Max.packet size
	printf("%s0x%x\r\n", maxpktsize_str, buf.bMaxPacketSize0);

	// VID
	printf("Vendor  ID:\t\t0x%x\r\n", buf.idVendor);

	// PID
	printf("Product ID:\t\t0x%x\r\n", buf.idProduct);

	// Revision
	printf("Revision ID:\t\t0x%x\r\n", buf.bcdDevice);

	// Mfg.string
	printf("Mfg.string index:\t0x%x", buf.iManufacturer);
	getstrdescr(addr, buf.iManufacturer);

	// Prod.string
	printf("Prod.string index:\t0x%x", buf.iProduct);
	getstrdescr(addr, buf.iProduct);

	// Serial number string
	printf("Serial number index:\t0x%x", buf.iSerialNumber);
	getstrdescr(addr, buf.iSerialNumber);

	// Number of configurations
	printf("Number of conf.:\t0x%x\r\n", buf.bNumConfigurations);

	return buf.bNumConfigurations;
}

/**
 * Get string descriptor.
 * Takes device address and string index.
 */
uint32_t getstrdescr(uint32_t addr, uint32_t idx)
{
	uint8_t buf[BUFSIZE];
	uint32_t rcode;
	uint32_t length;
	uint32_t i;
	uint16_t langid;

	if (idx == 0)
	{
		// Don't try to get index zero
		printf("\r\n");
		return 0;
	}

	rcode = Usb.getStrDescr(addr, 0, 1, 0, 0, buf);
	// Get language table length
	if (rcode)
	{
		printf("\r\nError retrieving LangID table length!\r\n");
		return rcode;
	}

	// Length is the first byte
	length = buf[0];
	// Get language table
	rcode = Usb.getStrDescr(addr, 0, length, 0, 0, buf);
	if (rcode)
	{
		printf("\r\nError retrieving LangID table!\r\n");
		return rcode;
	}

	// Get first langid
	langid = buf[3] << 8;
	// Bytes are swapped to account for endiannes
	langid |= (buf[2] & 0xff);

	rcode = Usb.getStrDescr(addr, 0, 1, idx, langid, buf);
	if (rcode)
	{
		printf("\r\nError retrieving string length!\r\n");
		return rcode;
	}

	length = (buf[0] < 254 ? buf[0] : 254);
	printf(" Length: %lu", length);
	rcode = Usb.getStrDescr(addr, 0, length, idx, langid, buf);
	if (rcode)
	{
		printf("\r\nError retrieving string!\r\n");
		return rcode;
	}

	printf(" Contents: ");
	for (i = 2; i < length; i += 2)
	{
		printf("%c", buf[i]);
	}
	printf("\r\n");

	return idx;
}


/**
 * Returns string to class name
 */
void classname_parse(uint8_t class_number)
{
	switch(class_number) {
		case 0x00:
			printf(" Use class information in the Interface Descriptor");
			break;
		case 0x01:
			printf(" Audio");
			break;
		case 0x02:
			printf(" Communications and CDC Control");
			break;
		case 0x03:
			printf(" HID (Human Interface Device)");
			break;
		case 0x05:
			printf(" Physical");
			break;
		case 0x06:
			printf(" Image");
			break;
		case 0x07:
			printf(" Printer");
			break;
		case 0x08:
			printf(" Mass Storage");
			break;
		case 0x09:
			printf(" Hub");
			break;
		case 0x0a:
			printf(" CDC-Data");
			break;
		case 0x0b:
			printf(" Smart Card");
			break;
		case 0x0d:
			printf(" Content Security");
			break;
		case 0x0e:
			printf(" Video");
			break;
		case 0x0f:
			printf(" Personal Healthcare");
			break;
		case 0xdc:
			printf("Diagnostic Device");
			break;
		case 0xe0:
			printf(" Wireless Controller");
			break;
		case 0xef:
			printf(" Miscellaneous");
			break;
		case 0xfe:
			printf(" Application Specific");
			break;
		case 0xff:
			printf(" Vendor Specific");
			break;
		default:
			break;
	}
}


/**
 * Print configuration descriptor.
 */
uint32_t getconfdescr(uint32_t addr, uint32_t conf)
{
	uint8_t buf[BUFSIZE];
	uint8_t* buf_ptr = buf;
	uint32_t rcode = 0;
	uint8_t confdescr_length = 0;
	uint8_t confdescr_type = 0;
	uint32_t total_length = 0;

	printf("\r\nConfiguration number %lu:\r\n", conf);

	rcode = Usb.getConfDescr(addr, 0, 4, conf, buf);  //get total length
	if (rcode)
	{
		printf("\r\nError retrieving configuration length. Error code %lu\r\n", rcode);
		return(0);
	}

	total_length = buf[3] << 8;
	total_length |= (buf[2] & 0xff);

	if (total_length > BUFSIZE)
	{
		// Check if total length is larger than buffer
		printf("Total length truncated to %d bytes\r\n", BUFSIZE);
		total_length = BUFSIZE;
	}

	// Get the whole descriptor
	rcode = Usb.getConfDescr(addr, 0, total_length, conf, buf);

	while (buf_ptr < buf + total_length)
	{
		// Parsing descriptors
		confdescr_length = *(buf_ptr);
		confdescr_type = *(buf_ptr + 1);
		switch (confdescr_type)
		{
			case(USB_DESCRIPTOR_CONFIGURATION):
				printconfdescr(buf_ptr);
				break;
			case(USB_DESCRIPTOR_INTERFACE):
				printintfdescr(buf_ptr);
				break;
			case(USB_DESCRIPTOR_ENDPOINT):
				printepdescr(buf_ptr);
				break;
			case(HID_DESCRIPTOR_HID):
				printhid_descr(buf_ptr);
				break;
			default:
				printunkdescr(buf_ptr);
			break;
		}

		// Advance buffer pointer
		buf_ptr = (buf_ptr + confdescr_length);
	}

	return 0;
}

/**
 * Print configuration descriptor.
 */
void printconfdescr(uint8_t* descr_ptr)
{
	USB_CONFIGURATION_DESCRIPTOR* conf_ptr = (USB_CONFIGURATION_DESCRIPTOR*)descr_ptr;
	uint8_t tmpbyte = 0;

	printf("\r\n\nConfiguration descriptor:\r\n");
	printf("Total length:\t\t%d\r\n", conf_ptr->wTotalLength);

	printf("\r\nNumber of interfaces:\t%d\r\n", conf_ptr->bNumInterfaces);

	printf("Configuration value:\t%d\r\n", conf_ptr->bConfigurationValue);

	printf("Configuration string:\t");
	tmpbyte = conf_ptr->iConfiguration;
	printf("0x%x", tmpbyte);
	getstrdescr(DEVADDR, tmpbyte);
	printf("Attributes:\t\t");
	tmpbyte = conf_ptr->bmAttributes;
	printf("0x%x", tmpbyte);

	if (tmpbyte & 0x40)
	{
		// D6
		printf(" Self-powered\r\n");
	}

	if (tmpbyte & 0x20)
	{
		// D5
		printf(" Remote Wakeup\r\n");
	}

	printf("Max.power:\t\t");
	tmpbyte = conf_ptr->bMaxPower;
	printf("0x%x", tmpbyte);
	printf(" ");
	printf("%d", (tmpbyte * 2));
	printf("ma\r\n");
}

/**
 * Print interface descriptor.
 */
void printintfdescr(uint8_t* descr_ptr)
{
	USB_INTERFACE_DESCRIPTOR* intf_ptr = (USB_INTERFACE_DESCRIPTOR*)descr_ptr;
	uint8_t tmpbyte = 0;

	printf("\r\nInterface descriptor:\r\n");
	printf("Interface number:\t%d\r\n", intf_ptr->bInterfaceNumber);
	printf("Alternate setting:\t%d\r\n", intf_ptr->bAlternateSetting);
	printf("Endpoints:\t\t%d\r\n", intf_ptr->bNumEndpoints);

	printf("%s", class_str);
	tmpbyte = intf_ptr->bInterfaceClass;
	printf("0x%x", tmpbyte);
	classname_parse(tmpbyte);
	printf("\r\n");

	printf("%s%d\r\n", subclass_str, intf_ptr->bInterfaceSubClass);
	printf("%s%d\r\n", protocol_str, intf_ptr->bInterfaceProtocol);

	printf("Interface string:\t");
	tmpbyte = intf_ptr->iInterface;
	printf("0x%x", tmpbyte);
	getstrdescr(DEVADDR, tmpbyte);
}

/**
 * Print endpoint descriptor.
 */
void printepdescr(uint8_t* descr_ptr)
{
	USB_ENDPOINT_DESCRIPTOR* ep_ptr = (USB_ENDPOINT_DESCRIPTOR*)descr_ptr;
	uint8_t tmpbyte = 0;

	printf("\r\nEndpoint descriptor:\r\n");
	printf("Endpoint address:\t");
	tmpbyte = ep_ptr->bEndpointAddress;
	printf("0x%x", tmpbyte & 0x0f);
	printf(" Direction: ");
	(tmpbyte & 0x80) ? printf("IN\r\n") : printf("OUT\r\n");

	printf("Attributes:\t\t");
	tmpbyte = ep_ptr->bmAttributes;
	printf("0x%x", tmpbyte);
	printf(" Transfer type: %s", transfer_types[(tmpbyte & 0x03)]);

	if ((tmpbyte & 0x03) == 1)
	{
		// Isochronous Transfer
		printf(", Sync Type: ");
		printf("%s", sync_types[(tmpbyte & 0x0c)]);
		printf(", Usage Type: ");
		printf("%s", usage_types[(tmpbyte & 0x30)]);
	}
	printf("\r\n");
	printf("%s%d\r\n", maxpktsize_str, ep_ptr->wMaxPacketSize);

	printf("Polling interval:\t");
	tmpbyte = ep_ptr->bInterval;
	printf("0x%x", tmpbyte);
	printf(" ");
	printf("%d", tmpbyte);
	printf(" ms\r\n");
}

/**
 * Print HID descriptor.
 */
void printhid_descr(uint8_t* descr_ptr)
{
	//PARSE pf = HIDreport_parse;
	USB_HID_DESCRIPTOR* hid_ptr = (USB_HID_DESCRIPTOR*)descr_ptr;
	uint8_t tmpbyte = 0;

	printf("\r\nHID descriptor:\r\n");
	printf("Descriptor length:\t");
	tmpbyte = hid_ptr->bLength;
	printf("0x%x %d bytes\r\n", tmpbyte, tmpbyte);
	printf("HID version:\t\t0x%x\r\n", hid_ptr->bcdHID);
	tmpbyte = hid_ptr->bCountryCode;
	printf("Country Code:\t\t%d ", tmpbyte);
	(tmpbyte > 35) ? printf("Reserved\r\n") : printf("%s\r\n", HID_Country_Codes[tmpbyte]);
	tmpbyte = hid_ptr->bNumDescriptors;
	printf("Class Descriptors:\t%d\r\n", tmpbyte);

	// Printing class descriptors
	// Advance buffer pointer
	descr_ptr += 6;
	for (uint8_t i = 0; i < tmpbyte; i++)
	{
		uint8_t tmpdata = 0;
		HID_CLASS_DESCRIPTOR* hidclass_ptr = (HID_CLASS_DESCRIPTOR*)descr_ptr;
		tmpdata = hidclass_ptr->bDescriptorType;
		printf("Class Descriptor Type:\t0x%x", tmpdata);

		if ((tmpdata < 0x21) || (tmpdata > 0x2f))
		{
			printf(" Invalid");
		}

		switch(tmpdata)
		{
			case 0x21:
				printf(" HID\r\n");
				break;
			case 0x22:
				printf(" Report\r\n");
				break;
			case 0x23:
				printf(" Physical\r\n");
				break;
			default:
				printf(" Reserved\r\n");
				break;
		}

		printf("Class Descriptor Size:  %d bytes\r\n", hidclass_ptr->wDescriptorLength);

		printf("\r\nHID report descriptor:\r\n");
		uint8_t buf[hidclass_ptr->wDescriptorLength];
		getReportDescr(DEVADDR, 0 , hidclass_ptr->wDescriptorLength, (uint8_t*)&buf);
		HIDreport_parse(buf, hidclass_ptr->wDescriptorLength);

		// Advance to the next record
		descr_ptr += 3;
	}
	printf("\r\n");
}

/**
 * Print unknown descriptor.
 */
void printunkdescr(uint8_t* descr_ptr)
{
	uint8_t length = *descr_ptr;
	uint32_t i = 0;

	printf("\r\nUnknown descriptor:\r\n");
	printf("Length:\t\t%d\r\n", *descr_ptr);
	printf("Type:\t\t %d\r\n", *(descr_ptr + 1));
	printf("Contents:\t");
	descr_ptr += 2;

	for (i = 0; i < length; i++)
	{
		printf("%d", *descr_ptr);
		descr_ptr++;
	}

	printf("\r\n");
}

/**
 * Print bitfields in main items.
 */
void print_mainbitfield(uint8_t byte_toparse)
{
	(byte_toparse & 0x01) ? printf("Constant,") : printf("Data,");  //bit 0
	(byte_toparse & 0x02) ? printf("Variable,") : printf("Array,");  //bit 1
	(byte_toparse & 0x04) ? printf("Relative,") : printf("Absolute,");  //...
	(byte_toparse & 0x08) ? printf("Wrap,") : printf("No Wrap,");
	(byte_toparse & 0x10) ? printf("Non Linear,") : printf("Linear,");
	(byte_toparse & 0x20) ? printf("No preferred,") : printf("Preferred State,");
	(byte_toparse & 0x40) ? printf("Null State,") : printf("No Null Position,");  //bit 6
	(byte_toparse & 0x40) ? printf("Volatile(ignore for Input),") : printf("Non-volatile(Ignore for Input),");  //bit 7
}

/**
 * HID Report Desriptor Parser Callback.
 * Called repeatedly from Control transfer function.
 */
void HIDreport_parse(uint8_t* buffer, uint8_t pkt_size)
{
#define B_SIZE 0x03        //bSize bitmask
#define B_TYPE 0x0c        //bType bitmask
#define B_TAG  0xf0        //bTag bitmask

	// Parser states
	enum STATE { ITEM_START, DATA_PARSE };
	static STATE state = ITEM_START;
	static uint8_t databytes_left = 0;
	static uint8_t databytes_left2 = 0;
	static uint8_t prefix;              //item prefix - type and tag
	uint8_t byte_toparse;
	uint8_t bType;
	uint8_t tmpbyte;
	uint32_t i = 0, j = 0;

	while (1)
	{
		if (i < pkt_size)
		{
			i = i + 1;
			byte_toparse = buffer[i];
		}
		else
		{
			return;
		}

		switch(state)
		{
			// Start of the record
			case ITEM_START:
				// Store prefix for databyte parsing
				prefix = byte_toparse >> 2;
				tmpbyte = byte_toparse & B_SIZE;

				// Get item length
				(tmpbyte == 0x03) ? databytes_left = 4 : databytes_left = tmpbyte;
				if (databytes_left)
				{
					// Read bytes after prefix
					state = DATA_PARSE;
				}
				printf("bSize: %d", databytes_left);
				databytes_left2 = databytes_left;

				// Get item type
				bType = (byte_toparse & B_TYPE) >> 2;
				printf("  bType: %s", btypes[bType]);

				// Get item tag
				printf("\t\tbTag: ");
				tmpbyte = (byte_toparse & B_TAG) >> 4 ;
				switch(bType)
				{
					case 0:  // Main
						if (tmpbyte < 0x08)
						{
							printf("Invalid Tag");
						}
						else if (tmpbyte > 0x0c)
						{
							printf("%s", reserved_msg);
						}
						else
						{
							printf("%s", maintags[tmpbyte - 8]);
						}
						break;

					case 1:  // Global
						(tmpbyte > 0x0b) ? printf("%s", reserved_msg) : printf("%s", globaltags[tmpbyte]);
						break;

					case 2:  // Local
						(tmpbyte > 0x0a) ? printf("%s", reserved_msg) : printf("%s", localtags[tmpbyte]);
						break;

					default:
						break;
				}
				break;

			case DATA_PARSE:
				switch(prefix)
				{
					case 0x20:  // Main Input
					case 0x24:  // Main Output
					case 0x2c:  // Main Feature
						// TODO: add parsing 8th bit
						print_mainbitfield(byte_toparse);
						break;

					case 0x28:    //Main Collection
						if ((byte_toparse > 0x06) && (byte_toparse < 0x80))
						{
							printf("%s", reserved_msg);
						}
						else if ((byte_toparse > 0x7f) && (byte_toparse <= 0xff))
						{
							printf("Vendor-defined");
						}
						else
						{
							printf("%s", collections[byte_toparse]);
						}
						break;

					//case 0x30: //Main End Collection
					case 0x01:    //Global Usage Page
						switch(byte_toparse)
						{
							// See HID Usage Tables doc v.1.12 page 14
							case 0x00:
							case 0x01:
							case 0x02:
							case 0x03:
							case 0x04:
							case 0x05:
							case 0x06:
							case 0x07:
							case 0x08:
							case 0x09:
							case 0x0a:
							case 0x0b:
							case 0x0c:
							case 0x0d:
							case 0x0e:
							case 0x0f:
							case 0x10:
								printf("%s", usage_pages[byte_toparse]);
								break;

							case 0x14:
								printf("Alphanumeric Display");
								break;

							case 0x40:
								printf("Medical Instruments");
								break;

							case 0x80:
							case 0x81:
							case 0x82:
							case 0x83:
								printf("Monitor page");
								break;

							case 0x84:
							case 0x85:
							case 0x86:
							case 0x87:
								printf("Power page");
								break;

							case 0x8c:
								printf("Bar Code Scanner page");
								break;

							case 0x8d:
								printf("Scale page");
								break;

							case 0x8e:
								printf("Magnetic Stripe Reading (MSR) Devices");
								break;

							case 0x8f:
								printf("Reserved Point of Sale pages");
								break;

							case 0x90:
								printf("Camera Control Page");
								break;

							case 0x91:
								printf("Arcade Page");
								break;

							default:
								break;
						}
				}

			databytes_left--;

			if (!databytes_left)
			{
				printf(" (data=0x");
				for (j = 0; j < databytes_left2; ++j)
					printf("%02x", buffer[i - j]);
				printf(")\r\n");

				state = ITEM_START;
			}
			break;
		}
	}
}
