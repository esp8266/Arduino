

/* Copyright (c) 2011, Peter Barrett  
**  
** Permission to use, copy, modify, and/or distribute this software for  
** any purpose with or without fee is hereby granted, provided that the  
** above copyright notice and this permission notice appear in all copies.  
** 
** THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL  
** WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED  
** WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR  
** BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES  
** OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,  
** WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  
** ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS  
** SOFTWARE.  
*/

#include "Platform.h"

//====================================================================================================
//====================================================================================================
//	Actual device descriptors

const u16 STRING_LANGUAGE[2] = {
	(3<<8) | (2+2),
	0x0409	// English
};

const u16 STRING_SERIAL[13] = {
	(3<<8) | (2+2*12),
	USB_SERIAL_STRING
};

const u16 STRING_IPRODUCT[28] = {
	(3<<8) | (2+2*27),
#if USB_PID == USB_PID_LEONARDO	
	'A','r','d','u','i','n','o',' ','L','e','o','n','a','r','d','o',' ','b','o','o','t','l','o','a','d','e','r'
#elif USB_PID == USB_PID_MICRO
	'A','r','d','u','i','n','o',' ','M','i','c','r','o',' ','b','o','o','t','l','o','a','d','e','r',' ',' ',' '
#endif
};

const u16 STRING_IMANUFACTURER[12] = {
	(3<<8) | (2+2*11),
	'A','r','d','u','i','n','o',' ','L','L','C'
};


//#ifdef CDC_ENABLED
DeviceDescriptor USB_DeviceDescriptorA = D_DEVICE(0X02,0X00,0X00,64,USB_VID,USB_PID,0x100,0,IPRODUCT,ISERIAL,1);
//#else
DeviceDescriptor USB_DeviceDescriptor = D_DEVICE(0x00,0x00,0x00,64,USB_VID,USB_PID,0x100,0,IPRODUCT,ISERIAL,1);
//#endif


Config USB_ConfigDescriptor =
{
	D_CONFIG(sizeof(Config),INTERFACE_COUNT),

#ifdef CDC_ENABLED
	//	CDC
	{
		D_IAD(0,2,CDC_COMMUNICATION_INTERFACE_CLASS,CDC_ABSTRACT_CONTROL_MODEL,1),

		//	CDC communication interface
		D_INTERFACE(CDC_ACM_INTERFACE,1,CDC_COMMUNICATION_INTERFACE_CLASS,CDC_ABSTRACT_CONTROL_MODEL,0),
		D_CDCCS(CDC_HEADER,0x10,0x01),								// Header (1.10 bcd)
		D_CDCCS(CDC_CALL_MANAGEMENT,1,1),							// Device handles call management
		D_CDCCS4(CDC_ABSTRACT_CONTROL_MANAGEMENT,2),				// SET_LINE_CODING, GET_LINE_CODING, SET_CONTROL_LINE_STATE supported
		D_CDCCS(CDC_UNION,CDC_ACM_INTERFACE,CDC_DATA_INTERFACE),	// Communication interface is master, data interface is slave 0
		D_ENDPOINT(USB_ENDPOINT_IN (CDC_ENDPOINT_ACM),USB_ENDPOINT_TYPE_INTERRUPT,0x10,0x40),

		//	CDC data interface
		D_INTERFACE(CDC_DATA_INTERFACE,2,CDC_DATA_INTERFACE_CLASS,0,0),
		D_ENDPOINT(USB_ENDPOINT_OUT(CDC_ENDPOINT_OUT),USB_ENDPOINT_TYPE_BULK,0x40,0),
		D_ENDPOINT(USB_ENDPOINT_IN (CDC_ENDPOINT_IN ),USB_ENDPOINT_TYPE_BULK,0x40,0)
	},
#endif
	// HID
	{
		D_INTERFACE(HID_INTERFACE,1,3,0,0),
		D_HIDREPORT(30),
		D_ENDPOINT(USB_ENDPOINT_IN (HID_ENDPOINT_INT),USB_ENDPOINT_TYPE_INTERRUPT,0x40,0x40)
	}
};

