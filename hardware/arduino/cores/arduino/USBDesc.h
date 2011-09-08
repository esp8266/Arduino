

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

#define CDC_ENABLED
#define HID_ENABLED
//#define MSC_ENABLED


#ifdef CDC_ENABLED
#define CDC_INTERFACE_COUNT	2
#define CDC_ENPOINT_COUNT	3
#else
#define CDC_INTERFACE_COUNT	0
#define CDC_ENPOINT_COUNT	0
#endif

#ifdef HID_ENABLED
#define HID_INTERFACE_COUNT	1
#define HID_ENPOINT_COUNT	1
#else
#define HID_INTERFACE_COUNT	0
#define HID_ENPOINT_COUNT	0
#endif

#ifdef MSC_ENABLED
#define MSC_INTERFACE_COUNT	1
#define MSC_ENPOINT_COUNT	2
#else
#define MSC_INTERFACE_COUNT	0
#define MSC_ENPOINT_COUNT	0
#endif

#define CDC_ACM_INTERFACE	0	// CDC ACM
#define CDC_DATA_INTERFACE	1	// CDC Data
#define CDC_FIRST_ENDPOINT	1
#define CDC_ENDPOINT_ACM	(CDC_FIRST_ENDPOINT)							// CDC First
#define CDC_ENDPOINT_OUT	(CDC_FIRST_ENDPOINT+1)
#define CDC_ENDPOINT_IN		(CDC_FIRST_ENDPOINT+2)

#define HID_INTERFACE		(CDC_ACM_INTERFACE + CDC_INTERFACE_COUNT)		// HID Interface
#define HID_FIRST_ENDPOINT	(CDC_FIRST_ENDPOINT + CDC_ENPOINT_COUNT)
#define HID_ENDPOINT_INT	(HID_FIRST_ENDPOINT)

#define MSC_INTERFACE		(HID_INTERFACE + HID_INTERFACE_COUNT)			// MSC Interface
#define MSC_FIRST_ENDPOINT	(HID_FIRST_ENDPOINT + HID_ENPOINT_COUNT)
#define MSC_ENDPOINT_OUT	(MSC_FIRST_ENDPOINT)
#define MSC_ENDPOINT_IN		(MSC_FIRST_ENDPOINT+1)

#define INTERFACE_COUNT		(MSC_INTERFACE + MSC_INTERFACE_COUNT)

#ifdef CDC_ENABLED
#define CDC_RX CDC_ENDPOINT_OUT
#define CDC_TX CDC_ENDPOINT_IN
#endif

#ifdef HID_ENABLED
#define HID_TX HID_ENDPOINT_INT
#endif

#ifdef MSC_ENABLED
#define MSC_RX MSC_ENDPOINT_OUT
#define MSC_TX MSC_ENDPOINT_IN
#endif


#define IMANUFACTURER	0
#define IPRODUCT		0
#define ISERIAL			1	// Only need this for MSC


#define WRITABLE_DIRECTORY	// undef saved 56 + 512 RAM

#define FAT_DISK_LABEL	'b','o','o','t','l','o','a','d','e','r',' '		// 11 chars (undef saves 12)
#define FAT_FILE_NAME	'F','I','R','M','W','A','R','E','B','I','N'		// 11 chars
#define MSC_DISK_SERIAL	'0','0','0','0','0','0','0','0','1','7','0','1'	// 12 chars

#define USB_VID 0x2341	// arduino LLC vid
#define USB_PID ARDUINO_MODEL_PID	// passed in at compile time - 0x0034 for Leonardo, 0x0035 for MIcro
#define FAT_OEM_NAME	'l','e','o','n','a','r','d','o'					// 8 chars
#define PRODUCT_NAME	'A','r','d','u','i','n','o','l','l','c'			// 10


