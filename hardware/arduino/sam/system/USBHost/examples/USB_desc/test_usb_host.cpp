#include "variant.h"
#include <stdio.h>
#include "descriptor_strings.h"

USBHost Usb;

/* Forward declarations */
void PrintAllAddresses(UsbDevice *pdev);
void PrintAddress(uint32_t addr);
void PrintDescriptors(uint32_t addr);
void PrintAllDescriptors(UsbDevice *pdev);
uint32_t getdevdescr(uint32_t addr, uint32_t &num_conf);
//void printhubdescr(uint8_t *descrptr, uint32_t addr);
uint32_t getconfdescr(uint32_t addr, uint32_t conf);
void printconfdescr(uint8_t* descr_ptr);
void printintfdescr(uint8_t* descr_ptr);
void printepdescr(uint8_t* descr_ptr);
void printunkdescr(uint8_t* descr_ptr);

void setup()
{
	cpu_irq_enable();
	printf("\r\nProgram started:\r\n");
	delay(200);
}

void loop()
{
	Usb.Task();

	if( Usb.getUsbTaskState() == USB_STATE_RUNNING )
	{

		Usb.ForEachUsbDevice(&PrintAllDescriptors);
		Usb.ForEachUsbDevice(&PrintAllAddresses);

		// Stop here
		while( 1 )
			;
	}
}

void PrintAllAddresses(UsbDevice *pdev)
{
    UsbDeviceAddress adr;
    adr.devAddress = pdev->address;
    printf("\r\nAddr: %lu (%lu.%lu.%lu)", adr.devAddress, adr.bmHub, adr.bmParent, adr.bmAddress);
}

void PrintAddress(uint32_t addr)
{
    UsbDeviceAddress adr;
    adr.devAddress = addr;
    printf("\r\nAddr: %lu (%lu.%lu.%lu)", adr.devAddress, adr.bmHub, adr.bmParent, adr.bmAddress);
}

void PrintDescriptors(uint32_t addr)
{
    uint32_t rcode = 0;
    uint32_t num_conf = 0;

	// Get device descriptor
    rcode = getdevdescr(addr, num_conf);
    if (rcode)
    {
		printf("%s%lu\r\n", Gen_Error_str, rcode);
    }
    printf("\r\n");

    for (uint32_t i = 0; i < num_conf; ++i)
    {
		// Get configuration descriptor
        rcode = getconfdescr(addr, i);
        if (rcode)
        {
			printf("%s%lu\r\n", Gen_Error_str, rcode);
        }
        printf("\r\n");
    }
}

void PrintAllDescriptors(UsbDevice *pdev)
{
    printf("\r\nDevice address %lu:\r\n", pdev->address);
    PrintDescriptors(pdev->address);
}

uint32_t getdevdescr(uint32_t addr, uint32_t &num_conf)
{
	USB_DEVICE_DESCRIPTOR buf;
	uint32_t rcode;

	rcode = Usb.getDevDescr(addr, 0, 0x12, (uint8_t*)&buf);
	if (rcode)
	{
		return rcode;
	}

	printf("%s", Dev_Header_str);
	printf("%s0x%x", Dev_Length_str, buf.bLength);
	printf("%s0x%x", Dev_Type_str, buf.bDescriptorType);
	printf("%s0x%x", Dev_Version_str, buf.bcdUSB);
	printf("%s0x%x", Dev_Class_str, buf.bDeviceClass);
	printf("%s0x%x", Dev_Subclass_str, buf.bDeviceSubClass);
	printf("%s0x%x", Dev_Protocol_str, buf.bDeviceProtocol);
	printf("%s0x%x", Dev_Pktsize_str, buf.bMaxPacketSize0);
	printf("%s0x%x", Dev_Vendor_str, buf.idVendor);
	printf("%s0x%x", Dev_Product_str, buf.idProduct);
	printf("%s0x%x", Dev_Revision_str, buf.bcdDevice);
	printf("%s0x%x", Dev_Mfg_str, buf.iManufacturer);
	printf("%s0x%x", Dev_Prod_str, buf.iProduct);
	printf("%s0x%x", Dev_Serial_str, buf.iSerialNumber);
	printf("%s0x%x", Dev_Nconf_str, buf.bNumConfigurations);
	num_conf = buf.bNumConfigurations;

	return 0;
}

/*
void printhubdescr(uint8_t *descrptr, uint32_t addr)
{
    HubDescriptor  *pHub = (HubDescriptor*) descrptr;
    uint8_t        len = *((uint8_t*)descrptr);

    printf("%s", PSTR("\r\n\r\nHub Descriptor:\r\n"));
    printf("%s", PSTR("bDescLength:\t\t"));
    printfln(pHub->bDescLength, HEX);

    printf("%s", PSTR("bDescriptorType:\t"));
    printfln(pHub->bDescriptorType, HEX);

    printf("%s", PSTR("bNbrPorts:\t\t"));
    printfln(pHub->bNbrPorts, HEX);

    printf("%s", PSTR("LogPwrSwitchMode:\t"));
    printfln(pHub->LogPwrSwitchMode, BIN);

    printf("%s", PSTR("CompoundDevice:\t\t"));
    printfln(pHub->CompoundDevice, BIN);

    printf("%s", PSTR("OverCurrentProtectMode:\t"));
    printfln(pHub->OverCurrentProtectMode, BIN);

    printf("%s", PSTR("TTThinkTime:\t\t"));
    printfln(pHub->TTThinkTime, BIN);

    printf("%s", PSTR("PortIndicatorsSupported:"));
    printfln(pHub->PortIndicatorsSupported, BIN);

    printf("%s", PSTR("Reserved:\t\t"));
    printfln(pHub->Reserved, HEX);

    printf("%s", PSTR("bPwrOn2PwrGood:\t\t"));
    printfln(pHub->bPwrOn2PwrGood, HEX);

    printf("%s", PSTR("bHubContrCurrent:\t"));
    printfln(pHub->bHubContrCurrent, HEX);

    for (uint8_t i=7; i<len; i++)
        print_hex(descrptr[i], 8);

    //for (uint8_t i=1; i<=pHub->bNbrPorts; i++)
    //    PrintHubPortStatus(&Usb, addr, i, 1);
}*/

uint32_t getconfdescr(uint32_t addr, uint32_t conf)
{
	uint8_t buf[BUFSIZE];
	uint8_t* buf_ptr = buf;
	uint32_t rcode = 0;
	uint32_t descr_length = 0;
	uint32_t descr_type = 0;
	uint32_t total_length = 0;

	// Get total length
	rcode = Usb.getConfDescr(addr, 0, 4, conf, buf);
	if (rcode)
	{
		printf("\r\nError retrieving configuration length. Error code %lu\r\n", rcode);
		return(0);
	}

	total_length = buf[3] << 8;
	total_length |= (buf[2] & 0xff);

	if( total_length > 256 )
	{
		// Check if total length is larger than buffer
		printf("%s", Conf_Trunc_str);
		total_length = 256;
	}

	// Get the whole descriptor
	rcode = Usb.getConfDescr(addr, 0, total_length, conf, buf);
	while (buf_ptr < buf + total_length)
	{
		// Parsing descriptors
		descr_length = *(buf_ptr);
		descr_type = *(buf_ptr + 1);
		switch (descr_type)
		{
			case USB_DESCRIPTOR_CONFIGURATION:
				printconfdescr(buf_ptr);
				break;
			case USB_DESCRIPTOR_INTERFACE:
				printintfdescr(buf_ptr);
				break;
			case USB_DESCRIPTOR_ENDPOINT:
				printepdescr(buf_ptr);
				break;
			/*case 0x29:
			printhubdescr(buf_ptr, addr);
			break;*/
			default:
				printunkdescr(buf_ptr);
				break;
		}
		// Advance buffer pointer
		buf_ptr = (buf_ptr + descr_length);
	}
	return 0;
}

/* function to print configuration descriptor */
void printconfdescr(uint8_t* descr_ptr)
{
	USB_CONFIGURATION_DESCRIPTOR* conf_ptr = (USB_CONFIGURATION_DESCRIPTOR*)descr_ptr;

	printf("%s", Conf_Header_str);
	printf("%s0x%x", Conf_Totlen_str, conf_ptr->wTotalLength);
	printf("%s0x%x", Conf_Nint_str, conf_ptr->bNumInterfaces);
	printf("%s0x%x", Conf_Value_str, conf_ptr->bConfigurationValue);
	printf("%s0x%x", Conf_String_str, conf_ptr->iConfiguration);
	printf("%s0x%x", Conf_Attr_str, conf_ptr->bmAttributes);
	printf("%s0x%x", Conf_Pwr_str, conf_ptr->bMaxPower);
}

/* function to print interface descriptor */
void printintfdescr(uint8_t* descr_ptr)
{
	USB_INTERFACE_DESCRIPTOR* intf_ptr = (USB_INTERFACE_DESCRIPTOR*)descr_ptr;

	printf("%s", Int_Header_str);
	printf("%s0x%x", Int_Number_str, intf_ptr->bInterfaceNumber);
	printf("%s0x%x", Int_Alt_str, intf_ptr->bAlternateSetting);
	printf("%s0x%x", Int_Endpoints_str, intf_ptr->bNumEndpoints);
	printf("%s0x%x", Int_Class_str, intf_ptr->bInterfaceClass);
	printf("%s0x%x", Int_Subclass_str, intf_ptr->bInterfaceSubClass);
	printf("%s0x%x", Int_Protocol_str, intf_ptr->bInterfaceProtocol);
	printf("%s0x%x", Int_String_str, intf_ptr->iInterface);
}

/* function to print endpoint descriptor */
void printepdescr(uint8_t* descr_ptr)
{
	USB_ENDPOINT_DESCRIPTOR* ep_ptr = (USB_ENDPOINT_DESCRIPTOR*)descr_ptr;

	printf("%s", End_Header_str);
	printf("%s0x%x", End_Address_str, ep_ptr->bEndpointAddress);
	printf("%s0x%x", End_Attr_str, ep_ptr->bmAttributes);
	printf("%s0x%x", End_Pktsize_str, ep_ptr->wMaxPacketSize);
	printf("%s0x%x", End_Interval_str, ep_ptr->bInterval);
}

/* function to print unknown descriptor */
void printunkdescr(uint8_t* descr_ptr)
{
	uint8_t length = *descr_ptr;
	uint32_t i;

	printf("%s", Unk_Header_str);
	printf("%s0x%x", Unk_Length_str, *descr_ptr);
	printf("%s0x%x", Unk_Type_str, *(descr_ptr + 1));
	printf("%s0x", Unk_Contents_str);
	descr_ptr += 2;
	for( i = 0; i < length; i++ )
	{
		printf("%02x", *descr_ptr);
		descr_ptr++;
	}
}
