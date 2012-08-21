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

#include "Arduino.h"
#include "USBAPI.h"
#include <stdio.h>

//#define TRACE_CORE(x)	x
#define TRACE_CORE(x)

static const uint32_t EndPoints[] =
{
	EP_TYPE_CONTROL,

#ifdef CDC_ENABLED
	EP_TYPE_BULK_IN,                // CDC_ENDPOINT_IN
	EP_TYPE_BULK_OUT,               // CDC_ENDPOINT_OUT
	EP_TYPE_INTERRUPT_IN,           // CDC_ENDPOINT_ACM
#endif

#ifdef HID_ENABLED
	EP_TYPE_INTERRUPT_IN_HID        // HID_ENDPOINT_INT
#endif
};

/** Pulse generation counters to keep track of the number of milliseconds remaining for each pulse type */
#define TX_RX_LED_PULSE_MS 100
volatile uint8_t TxLEDPulse; /**< Milliseconds remaining for data Tx LED pulse */
volatile uint8_t RxLEDPulse; /**< Milliseconds remaining for data Rx LED pulse */

//==================================================================
//==================================================================

extern const uint16_t STRING_LANGUAGE[];
extern const uint16_t STRING_IPRODUCT[];
extern const uint16_t STRING_IMANUFACTURER[];
extern const DeviceDescriptor USB_DeviceDescriptor;
extern const DeviceDescriptor USB_DeviceDescriptorA;

const uint16_t STRING_LANGUAGE[2] = {
	(3<<8) | (2+2),
	0x0409	// English
};

const uint16_t STRING_IPRODUCT[17] = {
	(3<<8) | (2+2*16),
#if USB_PID == USB_PID_LEONARDO
	'A','r','d','u','i','n','o',' ','L','e','o','n','a','r','d','o'
#elif USB_PID == USB_PID_MICRO
	'A','r','d','u','i','n','o',' ','M','i','c','r','o',' ',' ',' '
#elif USB_PID == USB_PID_DUE
	'A','r','d','u','i','n','o',' ','D','u','e',' ',' ',' ',' ','X'
#else
#error "Need an USB PID"
#endif
};

const uint16_t STRING_IMANUFACTURER[12] = {
	(3<<8) | (2+2*11),
	'A','r','d','u','i','n','o',' ','L','L','C'
};

#ifdef CDC_ENABLED
#define DEVICE_CLASS 0x02
#else
#define DEVICE_CLASS 0x00
#endif

//	DEVICE DESCRIPTOR
const DeviceDescriptor USB_DeviceDescriptor =
	D_DEVICE(0x00,0x00,0x00,64,USB_VID,USB_PID,0x100,IMANUFACTURER,IPRODUCT,0,1);

const DeviceDescriptor USB_DeviceDescriptorA =
	D_DEVICE(DEVICE_CLASS,0x00,0x00,64,USB_VID,USB_PID,0x100,IMANUFACTURER,IPRODUCT,0,1);

//==================================================================
//==================================================================

volatile uint32_t _usbConfiguration = 0;
volatile uint32_t _usbInitialized = 0;
uint32_t _cdcComposite = 0;

//==================================================================
//==================================================================

#define USB_RECV_TIMEOUT
class LockEP
{
	irqflags_t flags;
public:
	LockEP(uint32_t ep) : flags(cpu_irq_save())
	{
	}
	~LockEP()
	{
		cpu_irq_restore(flags);
	}
};

//	Number of bytes, assumes a rx endpoint
uint32_t USBD_Available(uint32_t ep)
{
	LockEP lock(ep);
	return UDD_FifoByteCount(ep & 0xF);
}

//	Non Blocking receive
//	Return number of bytes read
uint32_t USBD_Recv(uint32_t ep, void* d, uint32_t len)
{
	if (!_usbConfiguration || len < 0)
		return -1;

	LockEP lock(ep);
	uint32_t n = UDD_FifoByteCount(ep & 0xF);
	len = min(n,len);
	n = len;
	uint8_t* dst = (uint8_t*)d;
	while (n--)
		*dst++ = UDD_Recv8(ep & 0xF);
	if (len && !UDD_FifoByteCount(ep & 0xF)) // release empty buffer
		UDD_ReleaseRX(ep & 0xF);

	return len;
}

//	Recv 1 byte if ready
uint32_t USBD_Recv(uint32_t ep)
{
	uint8_t c;
	if (USBD_Recv(ep & 0xF, &c, 1) != 1)
		return -1;
	else
		return c;
}

//	Space in send EP
uint32_t USBD_SendSpace(uint32_t ep)
{
	LockEP lock(ep);
	if (!UDD_ReadWriteAllowed(ep & 0xF))
		return 0;
	return 64 - UDD_FifoByteCount(ep & 0xF);
}

//	Blocking Send of data to an endpoint
uint32_t USBD_Send(uint32_t ep, const void* d, uint32_t len)
{
	if (!_usbConfiguration)
		return -1;

	int r = len;
	const uint8_t* data = (const uint8_t*)d;
	uint8_t timeout = 250; // 250ms timeout on send? TODO

	while (len)
	{
		uint8_t n = USBD_SendSpace(ep);
		if (n == 0)
		{
			if (!(--timeout))
				return -1;
			delay(1);
			continue;
		}

		if (n > len)
			n = len;
		len -= n;

		UDD_Send(ep & 0xF, data, n);

		if (!UDD_ReadWriteAllowed(ep & 0xF) || ((len == 0) && (ep & TRANSFER_RELEASE)))	// Release full buffer
			UDD_ReleaseTX(ep & 0xF);
	}
	//TXLED1;					// light the TX LED
	//TxLEDPulse = TX_RX_LED_PULSE_MS;
	return r;
}

int _cmark;
int _cend;

void USBD_InitControl(int end)
{
	_cmark = 0;
	_cend = end;
}

//	Clipped by _cmark/_cend
int USBD_SendControl(uint8_t flags, const void* d, uint32_t len)
{
	const uint8_t* data = (const uint8_t*)d;
	uint32_t length = len;
	uint32_t sent = 0;
	uint32_t pos = 0;

	TRACE_CORE(printf("=> USBD_SendControl TOTAL len=%lu\r\n", len);)

	if (_cmark < _cend)
	{
		while (len > 0)
		{
			sent = UDD_Send(EP0, data + pos, len);
			TRACE_CORE(printf("=> USBD_SendControl sent=%lu\r\n", sent);)
			pos += sent;
			len -= sent;
		}
	}

	_cmark += length;

	return length;
}

//	Does not timeout or cross fifo boundaries
//	Will only work for transfers <= 64 bytes
//	TODO
int USBD_RecvControl(void* d, uint32_t len)
{
	UDD_WaitOUT();
	UDD_Recv(EP0, (uint8_t*)d, len);
	UDD_ClearOUT();

	return len;
}

//	Handle CLASS_INTERFACE requests
bool USBD_ClassInterfaceRequest(Setup& setup)
{
	uint8_t i = setup.wIndex;

	TRACE_CORE(printf("=> USBD_ClassInterfaceRequest\r\n");)

#ifdef CDC_ENABLED
	if (CDC_ACM_INTERFACE == i)
	{
		return CDC_Setup(setup);
	}
#endif

#ifdef HID_ENABLED
	if (HID_INTERFACE == i)
	{
		return HID_Setup(setup);
	}
#endif

	return false;
}

int USBD_SendInterfaces(void)
{
	int total = 0;
	uint8_t interfaces = 0;

#ifdef CDC_ENABLED
	total = CDC_GetInterface(&interfaces);
#endif

#ifdef HID_ENABLED
	total += HID_GetInterface(&interfaces);
#endif

	total = total; // Get rid of compiler warning
	TRACE_CORE(printf("=> USBD_SendInterfaces, total=%d interfaces=%d\r\n", total, interfaces);)
	return interfaces;
}

//	Construct a dynamic configuration descriptor
//	This really needs dynamic endpoint allocation etc
//	TODO
static bool USBD_SendConfiguration(int maxlen)
{
	//	Count and measure interfaces
	USBD_InitControl(0);
	//TRACE_CORE(printf("=> USBD_SendConfiguration _cmark1=%d\r\n", _cmark);)
	int interfaces = USBD_SendInterfaces();
	//TRACE_CORE(printf("=> USBD_SendConfiguration _cmark2=%d\r\n", _cmark);)
	//TRACE_CORE(printf("=> USBD_SendConfiguration sizeof=%d\r\n", sizeof(ConfigDescriptor));)

_Pragma("pack(1)")
	ConfigDescriptor config = D_CONFIG(_cmark + sizeof(ConfigDescriptor),interfaces);
_Pragma("pack()")
	//TRACE_CORE(printf("=> USBD_SendConfiguration clen=%d\r\n", config.clen);)

	//TRACE_CORE(printf("=> USBD_SendConfiguration maxlen=%d\r\n", maxlen);)

	//	Now send them
	USBD_InitControl(maxlen);
	USBD_SendControl(0,&config,sizeof(ConfigDescriptor));
	USBD_SendInterfaces();
	return true;
}

static bool USBD_SendDescriptor(Setup& setup)
{
	uint8_t t = setup.wValueH;
	uint8_t desc_length = 0;
	const uint8_t* desc_addr = 0;

	if (USB_CONFIGURATION_DESCRIPTOR_TYPE == t)
	{
		TRACE_CORE(printf("=> USBD_SendDescriptor : USB_CONFIGURATION_DESCRIPTOR_TYPE length=%d\r\n", setup.wLength);)
		return USBD_SendConfiguration(setup.wLength);
	}

	USBD_InitControl(setup.wLength);
#ifdef HID_ENABLED
	if (HID_REPORT_DESCRIPTOR_TYPE == t)
	{
		TRACE_CORE(puts("=> USBD_SendDescriptor : HID_REPORT_DESCRIPTOR_TYPE\r\n");)
		return HID_GetDescriptor(t);
	}
#endif

	if (USB_DEVICE_DESCRIPTOR_TYPE == t)
	{
		TRACE_CORE(puts("=> USBD_SendDescriptor : USB_DEVICE_DESCRIPTOR_TYPE\r\n");)
		if (setup.wLength == 8)
		{
			_cdcComposite = 1;
		}
		desc_addr = _cdcComposite ?  (const uint8_t*)&USB_DeviceDescriptorA : (const uint8_t*)&USB_DeviceDescriptor;
	}
	else if (USB_STRING_DESCRIPTOR_TYPE == t)
	{
		TRACE_CORE(puts("=> USBD_SendDescriptor : USB_STRING_DESCRIPTOR_TYPE\r\n");)
		if (setup.wValueL == 0)
			desc_addr = (const uint8_t*)&STRING_LANGUAGE;
		else if (setup.wValueL == IPRODUCT)
			desc_addr = (const uint8_t*)&STRING_IPRODUCT;
		else if (setup.wValueL == IMANUFACTURER)
			desc_addr = (const uint8_t*)&STRING_IMANUFACTURER;
		else
			return false;
	}

	if (desc_addr == 0)
	{
		return false;
	}

	if (desc_length == 0)
	{
		desc_length = *desc_addr;
	}

	TRACE_CORE(printf("=> USBD_SendDescriptor : desc_addr=%p desc_length=%d\r\n", desc_addr, desc_length);)
	USBD_SendControl(0, desc_addr, desc_length);

	return true;
}

//	Endpoint 0 interrupt
static void USB_ISR(void)
{
    //  End of Reset
    if (Is_udd_reset())
    {
		TRACE_CORE(printf(">>> End of Reset\r\n");)

		// Reset USB address to 0
		udd_configure_address(0);
		udd_enable_address();

		// Configure EP 0
        UDD_InitEP(0, EP_TYPE_CONTROL);
		udd_enable_setup_received_interrupt(0);
		udd_enable_endpoint_interrupt(0);

        _usbConfiguration = 0;
		udd_ack_reset();
    }

#ifdef CDC_ENABLED
  	if (Is_udd_endpoint_interrupt(CDC_RX))
	{
		udd_ack_out_received(CDC_RX);

		// Handle received bytes
		while (USBD_Available(CDC_RX))
			Serial.accept();

		udd_ack_fifocon(CDC_RX);
	}

	if (Is_udd_sof())
	{
		udd_ack_sof();
		USBD_Flush(CDC_TX);
	}
#endif

	// EP 0 Interrupt
	if (Is_udd_endpoint_interrupt(0))
	{

		if (!UDD_ReceivedSetupInt())
		{
			return;
		}

		Setup setup;
		UDD_Recv(EP0, (uint8_t*)&setup, 8);
		UDD_ClearSetupInt();

		uint8_t requestType = setup.bmRequestType;
		if (requestType & REQUEST_DEVICETOHOST)
		{
			TRACE_CORE(puts(">>> EP0 Int: IN Request\r\n");)
			UDD_WaitIN();
		}
		else
		{
			TRACE_CORE(puts(">>> EP0 Int: OUT Request\r\n");)
			UDD_ClearIN();
		}

		bool ok = true;
		if (REQUEST_STANDARD == (requestType & REQUEST_TYPE))
		{
			// Standard Requests
			uint8_t r = setup.bRequest;
			if (GET_STATUS == r)
			{
				TRACE_CORE(puts(">>> EP0 Int: GET_STATUS\r\n");)
				UDD_Send8(EP0, 0); // TODO
				UDD_Send8(EP0, 0);
			}
			else if (CLEAR_FEATURE == r)
			{
			}
			else if (SET_FEATURE == r)
			{
			}
			else if (SET_ADDRESS == r)
			{
				TRACE_CORE(puts(">>> EP0 Int: SET_ADDRESS\r\n");)
				UDD_WaitIN();
				UDD_SetAddress(setup.wValueL);
			}
			else if (GET_DESCRIPTOR == r)
			{
				TRACE_CORE(puts(">>> EP0 Int: GET_DESCRIPTOR\r\n");)
				ok = USBD_SendDescriptor(setup);
			}
			else if (SET_DESCRIPTOR == r)
			{
				TRACE_CORE(puts(">>> EP0 Int: SET_DESCRIPTOR\r\n");)
				ok = false;
			}
			else if (GET_CONFIGURATION == r)
			{
				TRACE_CORE(puts(">>> EP0 Int: GET_CONFIGURATION\r\n");)
				UDD_Send8(EP0, 1);
			}
			else if (SET_CONFIGURATION == r)
			{
				if (REQUEST_DEVICE == (requestType & REQUEST_RECIPIENT))
				{
					TRACE_CORE(printf(">>> EP0 Int: SET_CONFIGURATION REQUEST_DEVICE %d\r\n", setup.wValueL);)

					UDD_InitEndpoints(EndPoints, (sizeof(EndPoints) / sizeof(EndPoints[0])));
					_usbConfiguration = setup.wValueL;

#ifdef CDC_ENABLED
					// Enable interrupt for CDC reception from host (OUT packet)
					udd_enable_out_received_interrupt(CDC_RX);
					udd_enable_endpoint_interrupt(CDC_RX);
#endif
				}
				else
				{
					TRACE_CORE(puts(">>> EP0 Int: SET_CONFIGURATION failed!\r\n");)
					ok = false;
				}
			}
			else if (GET_INTERFACE == r)
			{
				TRACE_CORE(puts(">>> EP0 Int: GET_INTERFACE\r\n");)
			}
			else if (SET_INTERFACE == r)
			{
				TRACE_CORE(puts(">>> EP0 Int: SET_INTERFACE\r\n");)
			}
		}
		else
		{
			TRACE_CORE(puts(">>> EP0 Int: ClassInterfaceRequest\r\n");)

			UDD_WaitIN(); // Workaround: need tempo here, else CDC serial won't open correctly

			USBD_InitControl(setup.wLength); // Max length of transfer
			ok = USBD_ClassInterfaceRequest(setup);
		}

		if (ok)
		{
			TRACE_CORE(puts(">>> EP0 Int: Send packet\r\n");)
			UDD_ClearIN();
		}
		else
		{
			TRACE_CORE(puts(">>> EP0 Int: Stall\r\n");)
			UDD_Stall();
		}
	}
}

void USBD_Flush(uint32_t ep)
{
	if (UDD_FifoByteCount(ep))
		UDD_ReleaseTX(ep);
}

//	VBUS or counting frames
//	Any frame counting?
uint32_t USBD_Connected(void)
{
	uint8_t f = UDD_GetFrameNumber();

	delay(3);

	return f != UDD_GetFrameNumber();
}


//=======================================================================
//=======================================================================

USBDevice_ USBDevice;

USBDevice_::USBDevice_()
{
	UDD_SetStack(&USB_ISR);

	if (UDD_Init() == 0UL)
	{
		_usbInitialized=1UL;
	}
}

bool USBDevice_::attach(void)
{
  if (_usbInitialized != 0UL)
  {
    UDD_Attach();
	_usbConfiguration = 0;
	return true;
  }
  else
  {
    return false;
  }
}

bool USBDevice_::detach(void)
{
	if (_usbInitialized != 0UL)
	{
		UDD_Detach();
		return true;
	}
	else
	{
		return false;
	}
}

//	Check for interrupts
//	TODO: VBUS detection
bool USBDevice_::configured()
{
	return _usbConfiguration;
}

void USBDevice_::poll()
{
}
