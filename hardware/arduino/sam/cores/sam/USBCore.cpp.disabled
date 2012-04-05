

/* Copyright (c) 2010, Peter Barrett
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
#define USBCON

#include "Platform.h"
#include "USBAPI.h"
#include "USBDesc.h"
#include "USBCore.h"


#if defined(USBCON)

extern const uint8_t _initEndpoints[] ;
const uint8_t _initEndpoints[] =
{
	0,

#ifdef CDC_ENABLED
	EP_TYPE_BULK_IN,			// CDC_ENDPOINT_IN
	EP_TYPE_BULK_OUT,			// CDC_ENDPOINT_OUT
	EP_TYPE_INTERRUPT_IN,		// CDC_ENDPOINT_ACM
#endif

#ifdef HID_ENABLED
	EP_TYPE_INTERRUPT_IN		// HID_ENDPOINT_INT
#endif
};

/** Pulse generation counters to keep track of the number of milliseconds remaining for each pulse type */
#define TX_RX_LED_PULSE_MS 100
volatile uint8_t TxLEDPulse; /**< Milliseconds remaining for data Tx LED pulse */
volatile uint8_t RxLEDPulse; /**< Milliseconds remaining for data Rx LED pulse */

//==================================================================
//==================================================================

extern const uint16_t STRING_LANGUAGE[] ;
extern const uint16_t STRING_IPRODUCT[] ;
extern const uint16_t STRING_IMANUFACTURER[] ;
extern const DeviceDescriptor USB_DeviceDescriptor ;
extern const DeviceDescriptor USB_DeviceDescriptorA ;

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
	'A','r','d','u','i','n','o',' ','D','u','e',' ',' ',' ',' ',' '
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

volatile uint8_t _usbConfiguration = 0;
uint8_t _cdcComposite = 0;


//==================================================================
//==================================================================

//	Recv 1 byte if ready
int USBD_Recv(uint8_t ep)
{
	uint8_t c;

	if (USB_Recv(ep,&c,1) != 1)
  {
		return -1;
  }
  else
  {
	  return c;
  }
}

//	Non Blocking receive
//	Return number of bytes read
int USBD_Recv(uint8_t ep, void* d, int len)
{
  uint8_t n ;
	uint8_t* dst ;

	if (!_usbConfiguration || len < 0)
  {
		return -1;
  }

	n = FifoByteCount(ep);
  
	len = min(n,len);
	n = len;
	dst = (uint8_t*)d;
	while (n--)
  {
		*dst++ = Recv8(ep);
  }
//	if (len && !FifoByteCount())	// release empty buffer
//		ReleaseRX();

	return len;
}

static bool USBD_SendControl(uint8_t d)
{
	if ( _cmark < _cend )
	{
		if ( !WaitForINOrOUT() )
    {
			return false;
    }
      
		Send8( d ) ;
    
		if ( !((_cmark + 1) & 0x3F) )
    {
			ClearIN();	// Fifo is full, release this packet
    }
	}
	_cmark++;
  
	return true ;
}

//	Clipped by _cmark/_cend
int USBD_SendControl(uint8_t flags, const void* d, int len)
{
	int sent = len;
	const uint8_t* data = (const uint8_t*)d ;

	while ( len-- )
	{
		uint8_t c = *data++ ;
    
		if ( !SendControl( c ) )
    {
			return -1;
    }
	}

	return sent;
}

//	Does not timeout or cross fifo boundaries
//	Will only work for transfers <= 64 bytes
//	TODO
int USBD_RecvControl(void* d, int len)
{
	WaitOUT() ;
	Recv( (uint8_t*)d, len ) ;
	ClearOUT() ;

	return len ;
}

//	Handle CLASS_INTERFACE requests
bool USBD_ClassInterfaceRequest(Setup& setup)
{
	uint8_t i = setup.wIndex;

#ifdef CDC_ENABLED
	if ( CDC_ACM_INTERFACE == i )
  {
		return CDC_Setup(setup);
  }
#endif

#ifdef HID_ENABLED
	if ( HID_INTERFACE == i )
  {
		return HID_Setup(setup);
  }
#endif

	return false ;
}

int USBD_SendInterfaces(void)
{
	int total = 0;
	uint8_t interfaces = 0;

#ifdef CDC_ENABLED
	total = CDC_GetInterface(&interfaces) ;
#endif

#ifdef HID_ENABLED
	total += HID_GetInterface(&interfaces) ;
#endif

	return interfaces;
}

//	Construct a dynamic configuration descriptor
//	This really needs dynamic endpoint allocation etc
//	TODO
static bool USBD_SendConfiguration(int maxlen)
{
	//	Count and measure interfaces
	InitControl(0);
	int interfaces = SendInterfaces();
	ConfigDescriptor config = D_CONFIG(_cmark + sizeof(ConfigDescriptor),interfaces);

	//	Now send them
	InitControl(maxlen);
	USB_SendControl(0,&config,sizeof(ConfigDescriptor));
	SendInterfaces();
	return true;
}

static bool USBD_SendDescriptor(Setup& setup)
{
	uint8_t t = setup.wValueH;
	uint8_t desc_length = 0;
	const uint8_t* desc_addr = 0;

	if ( USB_CONFIGURATION_DESCRIPTOR_TYPE == t )
  {
		return SendConfiguration(setup.wLength);
  }

	InitControl(setup.wLength);
#ifdef HID_ENABLED
	if ( HID_REPORT_DESCRIPTOR_TYPE == t )
  {
		return HID_GetDescriptor( t ) ;
  }
#endif

	if (USB_DEVICE_DESCRIPTOR_TYPE == t)
	{
		if ( setup.wLength == 8 )
    {
			_cdcComposite = 1;
    }
		desc_addr = _cdcComposite ?  (const uint8_t*)&USB_DeviceDescriptorA : (const uint8_t*)&USB_DeviceDescriptor;
	}
	else if (USB_STRING_DESCRIPTOR_TYPE == t)
	{
		if (setup.wValueL == 0)
			desc_addr = (const uint8_t*)&STRING_LANGUAGE;
		else if (setup.wValueL == IPRODUCT)
			desc_addr = (const uint8_t*)&STRING_IPRODUCT;
		else if (setup.wValueL == IMANUFACTURER)
			desc_addr = (const uint8_t*)&STRING_IMANUFACTURER;
		else
			return false;
	}

	if ( desc_addr == 0 )
  {
		return false ;
  }
  
	if ( desc_length == 0 )
  {
		desc_length = *desc_addr;
  }

	USB_SendControl(TRANSFER_PGM,desc_addr,desc_length);

	return true;
}

//	Endpoint 0 interrupt
void USB_ISR()
{
  SetEP(0) ;
  
	if ( !ReceivedSetupInt() )
  {
		return;
  }

	Setup setup ;
	Recv((uint8_t*)&setup,8);
	ClearSetupInt();

	uint8_t requestType = setup.bmRequestType;
	if (requestType & REQUEST_DEVICETOHOST)
  {
		WaitIN();
  }
	else
  {
		ClearIN();
  }

  bool ok = true ;
	if (REQUEST_STANDARD == (requestType & REQUEST_TYPE))
	{
		//	Standard Requests
		uint8_t r = setup.bRequest;
		if (GET_STATUS == r)
		{
			Send8(0);		// TODO
			Send8(0);
		}
		else if (CLEAR_FEATURE == r)
		{
		}
		else if (SET_FEATURE == r)
		{
		}
		else if (SET_ADDRESS == r)
		{
			WaitIN();
			UDPHS->UDPHS_CTRL |= UDPHS_CTRL_DEV_ADDR(setup.wValueL) | UDPHS_CTRL_FADDR_EN;
		}
		else if (GET_DESCRIPTOR == r)
		{
			ok = SendDescriptor(setup);
		}
		else if (SET_DESCRIPTOR == r)
		{
			ok = false;
		}
		else if (GET_CONFIGURATION == r)
		{
			Send8(1);
		}
		else if (SET_CONFIGURATION == r)
		{
			if (REQUEST_DEVICE == (requestType & REQUEST_RECIPIENT))
			{
				InitEndpoints();
				_usbConfiguration = setup.wValueL;
			} 
      else
      {
				ok = false;
      }
		}
		else if (GET_INTERFACE == r)
		{
		}
		else if (SET_INTERFACE == r)
		{
		}
	}
	else
	{
		InitControl(setup.wLength);		//	Max length of transfer
		ok = ClassInterfaceRequest(setup);
	}

	if (ok)
  {
		ClearIN();
  }
	else
	{
		Stall();
	}
}

void USBD_Flush(uint8_t ep)
{
	SetEP(ep);
//	if (FifoByteCount())
//		ReleaseTX();
}

//	General interrupt
// USB device interrupt handler
/*
// Manages device resume, suspend, end of bus reset.
// Forwards endpoint interrupts to the appropriate handler.
//  General interrupt
ISR(USB_GEN_vect)
{
    uint8_t udint = UDINT;
    UDINT = 0;

    //  End of Reset
    if (udint & (1<<EORSTI))
    {
        InitEP(0,EP_TYPE_CONTROL,EP_SINGLE_64); // init ep0
        _usbConfiguration = 0;          // not configured yet
        UEIENX = 1 << RXSTPE;           // Enable interrupts for ep0
    }

    //  Start of Frame - happens every millisecond so we use it for TX and RX LED one-shot timing, too
    if (udint & (1<<SOFI))
    {
#ifdef CDC_ENABLED
        USB_Flush(CDC_TX);              // Send a tx frame if found
#endif

        // check whether the one-shot period has elapsed.  if so, turn off the LED
        if (TxLEDPulse && !(--TxLEDPulse))
            TXLED0;
        if (RxLEDPulse && !(--RxLEDPulse))
            RXLED0;
    }
}

*/



//	VBUS or counting frames
//	Any frame counting?
uint8_t USBD_Connected(void)
{
	uint8_t f = UDFNUML;
	delay(3);
	return f != UDFNUML;
}


//=======================================================================
//=======================================================================

USB_ USB;

USB_::USB_()
{
}

void USB_::attach(void)
{
  USBD_Attach() ;
}

void USB_::detach(void)
{
    UDPHS->UDPHS_CTRL |= UDPHS_CTRL_DETACH; // detach
    UDPHS->UDPHS_CTRL &= ~UDPHS_CTRL_PULLD_DIS; // Enable Pull Down
}

//	Check for interrupts
//	TODO: VBUS detection
bool USB_::configured()
{
	return _usbConfiguration;
}

void USB_::poll()
{
}

#endif /* if defined(USBCON) */
