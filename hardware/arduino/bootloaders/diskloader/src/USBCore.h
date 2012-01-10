
// Copyright (c) 2010, Peter Barrett 
/*
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

#ifndef __USBCORE_H__
#define __USBCORE_H__

#define GET_STATUS			0
#define CLEAR_FEATURE		1
#define SET_FEATURE			3
#define SET_ADDRESS			5
#define GET_DESCRIPTOR		6
#define GET_CONFIGURATION	8
#define SET_CONFIGURATION	9
#define GET_INTERFACE		10
#define SET_INTERFACE		11

// bmRequestType
#define HOSTTODEVICE	0x00
#define DEVICETOHOST	0x80
#define STANDARD		0x00
#define CLASS			0x20
#define VENDOR			0x40
#define DEVICE			0x00
#define INTERFACE		0x01
#define ENDPOINT		0x02
#define OTHER			0x03

#define CDC_SET_LINE_CODING		0x20
#define CDC_GET_LINE_CODING		0x21
#define CDC_SET_CONTROL_LINE_STATE	0x22

//	Descriptors

#define USB_DEVICE_DESC_SIZE 18
#define USB_CONFIGUARTION_DESC_SIZE 9
#define USB_INTERFACE_DESC_SIZE 9
#define USB_ENDPOINT_DESC_SIZE 7

#define USB_DEVICE_DESCRIPTOR_TYPE             1
#define USB_CONFIGURATION_DESCRIPTOR_TYPE      2
#define USB_STRING_DESCRIPTOR_TYPE             3
#define USB_INTERFACE_DESCRIPTOR_TYPE          4
#define USB_ENDPOINT_DESCRIPTOR_TYPE           5

#define USB_DEVICE_CLASS_COMMUNICATIONS        0x02
#define USB_DEVICE_CLASS_HUMAN_INTERFACE       0x03
#define USB_DEVICE_CLASS_STORAGE               0x08
#define USB_DEVICE_CLASS_VENDOR_SPECIFIC       0xFF

#define USB_CONFIG_POWERED_MASK                0x40
#define USB_CONFIG_BUS_POWERED                 0x80
#define USB_CONFIG_SELF_POWERED                0xC0
#define USB_CONFIG_REMOTE_WAKEUP               0x20

// bMaxPower in Configuration Descriptor
#define USB_CONFIG_POWER_MA(mA)                ((mA)/2)

// bEndpointAddress in Endpoint Descriptor
#define USB_ENDPOINT_DIRECTION_MASK            0x80
#define USB_ENDPOINT_OUT(addr)                 ((addr) | 0x00)
#define USB_ENDPOINT_IN(addr)                  ((addr) | 0x80)

#define USB_ENDPOINT_TYPE_MASK                 0x03
#define USB_ENDPOINT_TYPE_CONTROL              0x00
#define USB_ENDPOINT_TYPE_ISOCHRONOUS          0x01
#define USB_ENDPOINT_TYPE_BULK                 0x02
#define USB_ENDPOINT_TYPE_INTERRUPT            0x03

#define TOBYTES(x) ((x) & 0xFF),(((x) >> 8) & 0xFF)

#define CDC_V1_10                               0x0110
#define CDC_COMMUNICATION_INTERFACE_CLASS       0x02

#define CDC_CALL_MANAGEMENT                     0x01
#define CDC_ABSTRACT_CONTROL_MODEL              0x02
#define CDC_HEADER                              0x00
#define CDC_ABSTRACT_CONTROL_MANAGEMENT         0x02
#define CDC_UNION                               0x06
#define CDC_CS_INTERFACE                        0x24
#define CDC_CS_ENDPOINT                         0x25
#define CDC_DATA_INTERFACE_CLASS                0x0A


//	Device
typedef struct {
	uint8_t len;				// 18
	uint8_t dtype;			// 1 USB_DEVICE_DESCRIPTOR_TYPE
	uint16_t usbVersion;		// 0x200
	uint8_t	deviceClass;
	uint8_t	deviceSubClass;
	uint8_t	deviceProtocol;
	uint8_t	packetSize0;	// Packet 0
	uint16_t	idVendor;
	uint16_t	idProduct;
	uint16_t	deviceVersion;	// 0x100
	uint8_t	iManufacturer;
	uint8_t	iProduct;
	uint8_t	iSerialNumber;
	uint8_t	bNumConfigurations;
} DeviceDescriptor;

//	Config
typedef struct {
	uint8_t	len;			// 9
	uint8_t	dtype;			// 2
	uint16_t clen;			// total length
	uint8_t	numInterfaces;
	uint8_t	config;
	uint8_t	iconfig;
	uint8_t	attributes;
	uint8_t	maxPower;
} ConfigDescriptor;

//	String

//	Interface
typedef struct
{
	uint8_t len;		// 9
	uint8_t dtype;	// 4
	uint8_t number;
	uint8_t alternate;
	uint8_t numEndpoints;
	uint8_t interfaceClass;
	uint8_t interfaceSubClass;
	uint8_t protocol;
	uint8_t iInterface;
} InterfaceDescriptor;

//	Endpoint
typedef struct
{
	uint8_t len;		// 7
	uint8_t dtype;	// 5
	uint8_t addr;
	uint8_t attr;
	uint16_t packetSize;
	uint8_t interval;
} EndpointDescriptor;

// Interface Association Descriptor
// Used to bind 2 interfaces together in CDC compostite device
typedef struct
{
	uint8_t len;				// 8
	uint8_t dtype;			// 11
	uint8_t firstInterface;
	uint8_t interfaceCount;
	uint8_t functionClass;
	uint8_t funtionSubClass;
	uint8_t functionProtocol;
	uint8_t iInterface;
} IADDescriptor;

//	CDC CS interface descriptor
typedef struct
{
	uint8_t len;		// 5
	uint8_t dtype;	// 0x24
	uint8_t subtype;
	uint8_t d0;
	uint8_t d1;
} CDCCSInterfaceDescriptor;

typedef struct
{
	uint8_t len;		// 4
	uint8_t dtype;	// 0x24
	uint8_t subtype;
	uint8_t d0;
} CDCCSInterfaceDescriptor4;

typedef struct 
{
	IADDescriptor				iad;	// Only needed on compound device

	//	Control
	InterfaceDescriptor			cif;	// 
	CDCCSInterfaceDescriptor	header;
	CDCCSInterfaceDescriptor	callManagement;
	CDCCSInterfaceDescriptor4	controlManagement;
	CDCCSInterfaceDescriptor	functionalDescriptor;
	EndpointDescriptor			cifin;

	//	Data
	InterfaceDescriptor			dif;
	EndpointDescriptor			in;
	EndpointDescriptor			out;
} CDCDescriptor;

typedef struct
{
	uint8_t len;			// 9
	uint8_t dtype;		// 0x21
	uint8_t addr;
	uint8_t	versionL;	// 0x101
	uint8_t	versionH;	// 0x101
	uint8_t	country;
	uint8_t	desctype;	// 0x22 report
	uint8_t	descLenL;
	uint8_t	descLenH;
} HIDDescDescriptor;

typedef struct 
{
	InterfaceDescriptor			hid;
	HIDDescDescriptor			desc;
	EndpointDescriptor			in;
} HIDDescriptor;

#define D_DEVICE(_class,_subClass,_proto,_packetSize0,_vid,_pid,_version,_im,_ip,_is,_configs) \
	{ 18, 1, 0x200, _class,_subClass,_proto,_packetSize0,_vid,_pid,_version,_im,_ip,_is,_configs }

#define D_CONFIG(_totalLength,_interfaces) \
	{ 9, 2, _totalLength,_interfaces, 1, 0, USB_CONFIG_BUS_POWERED, USB_CONFIG_POWER_MA(100) }

#define D_INTERFACE(_n,_numEndpoints,_class,_subClass,_protocol) \
	{ 9, 4, _n, 0, _numEndpoints, _class,_subClass, _protocol, 0 }

#define D_ENDPOINT(_addr,_attr,_packetSize, _interval) \
	{ 7, 5, _addr,_attr,_packetSize, _interval }

#define D_IAD(_firstInterface, _count, _class, _subClass, _protocol) \
	{ 8, 11, _firstInterface, _count, _class, _subClass, _protocol, 0 }

#define D_HIDREPORT(_descriptorLength) \
	{ 9, 0x21, 0x1, 0x1, 0, 1, 0x22, _descriptorLength, 0 }

#define D_CDCCS(_subtype,_d0,_d1)	{ 5, 0x24, _subtype, _d0, _d1 }
#define D_CDCCS4(_subtype,_d0)		{ 4, 0x24, _subtype, _d0 }

#endif