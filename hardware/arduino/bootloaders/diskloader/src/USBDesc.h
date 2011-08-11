

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


#ifndef CDC_ENABLED

#define MSC_INTERFACE		0	// MSC Interface
#define MSC_ENDPOINT_OUT	1
#define MSC_ENDPOINT_IN		2

#define INTERFACE_COUNT		1	// 1 for msc

#else

#define CDC_ACM_INTERFACE	0	// CDC ACM
#define CDC_DATA_INTERFACE	1	// CDC Data
#define CDC_ENDPOINT_ACM	1
#define CDC_ENDPOINT_OUT	2
#define CDC_ENDPOINT_IN		3

#define HID_INTERFACE		2	// HID Interface
#define HID_ENDPOINT_INT	4

#ifdef MSC_ENABLED

#define MSC_INTERFACE		3	// MSC Interface
#define MSC_ENDPOINT_OUT	5
#define MSC_ENDPOINT_IN		6
#define INTERFACE_COUNT		4	// 2 for cdc + 1 for hid + 1 for msc

#else

#define INTERFACE_COUNT		3	// 2 for cdc + 1 for hid

#endif

#endif

typedef struct
{
	ConfigDescriptor	config;
#ifdef CDC_ENABLED
	CDCDescriptor		cdc;
#endif
	HIDDescriptor		hid;
#ifdef MSC_ENABLED
	MSCDescriptor		msc;
#endif
} Config;

extern Config USB_ConfigDescriptor PROGMEM;
extern DeviceDescriptor USB_DeviceDescriptor PROGMEM;
extern DeviceDescriptor USB_DeviceDescriptorA PROGMEM;

extern const u16 STRING_LANGUAGE[2] PROGMEM;
extern const u16 STRING_SERIAL[13] PROGMEM;

#define IMANUFACTURER	0
#define IPRODUCT		0
#define ISERIAL			1	// Only need this for MSC


#define MSC_TX MSC_ENDPOINT_IN
#define MSC_RX MSC_ENDPOINT_OUT
#define CDC_TX CDC_ENDPOINT_IN
#define CDC_RX CDC_ENDPOINT_OUT