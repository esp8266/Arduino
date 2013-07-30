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
#include "Reset.h"
#include <stdio.h>

//#define TRACE_CORE(x)	x
#define TRACE_CORE(x)

static const uint32_t EndPoints[] =
{
	EP_TYPE_CONTROL,

#ifdef CDC_ENABLED
	EP_TYPE_INTERRUPT_IN,           // CDC_ENDPOINT_ACM
	EP_TYPE_BULK_OUT,               // CDC_ENDPOINT_OUT
	EP_TYPE_BULK_IN,                // CDC_ENDPOINT_IN
#endif

#ifdef HID_ENABLED
	EP_TYPE_INTERRUPT_IN_HID        // HID_ENDPOINT_INT
#endif
};

/** Pulse generation counters to keep track of the number of milliseconds remaining for each pulse type */
#define TX_RX_LED_PULSE_MS 100
volatile uint8_t TxLEDPulse; /**< Milliseconds remaining for data Tx LED pulse */
volatile uint8_t RxLEDPulse; /**< Milliseconds remaining for data Rx LED pulse */
static char isRemoteWakeUpEnabled = 0;
static char isEndpointHalt = 0;
//==================================================================
//==================================================================

extern const uint16_t STRING_LANGUAGE[];
extern const uint8_t STRING_PRODUCT[];
extern const uint8_t STRING_MANUFACTURER[];
extern const DeviceDescriptor USB_DeviceDescriptor;
extern const DeviceDescriptor USB_DeviceDescriptorA;

const uint16_t STRING_LANGUAGE[2] = {
	(3<<8) | (2+2),
	0x0409	// English
};

#ifndef USB_PRODUCT
// Use a hardcoded product name if none is provided
#if USB_PID == USB_PID_DUE
#define USB_PRODUCT "Arduino Due"
#else
#define USB_PRODUCT "USB IO Board"
#endif
#endif

const uint8_t STRING_PRODUCT[] = USB_PRODUCT;

#if USB_VID == 0x2341
#define USB_MANUFACTURER "Arduino LLC"
#elif !defined(USB_MANUFACTURER)
// Fall through to unknown if no manufacturer name was provided in a macro
#define USB_MANUFACTURER "Unknown"
#endif

const uint8_t STRING_MANUFACTURER[12] = USB_MANUFACTURER;

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

const DeviceDescriptor USB_DeviceQualifier =
	D_QUALIFIER(0x00,0x00,0x00,64,1);

//! 7.1.20 Test Mode Support
static const unsigned char test_packet_buffer[] = {
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,                // JKJKJKJK * 9
    0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,                     // JJKKJJKK * 8
    0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,                     // JJJJKKKK * 8
    0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // JJJJJJJKKKKKKK * 8
    0x7F,0xBF,0xDF,0xEF,0xF7,0xFB,0xFD,                          // JJJJJJJK * 8
    0xFC,0x7E,0xBF,0xDF,0xEF,0xF7,0xFB,0xFD,0x7E                 // {JKKKKKKK * 10}, JK
};

//==================================================================
//==================================================================

volatile uint32_t _usbConfiguration = 0;
volatile uint32_t _usbInitialized = 0;
uint32_t _usbSetInterface = 0;
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
//uint32_t USBD_SendSpace(uint32_t ep)
//{
	//LockEP lock(ep);
////	if (!UDD_ReadWriteAllowed(ep & 0xF))
    ////{
        ////printf("pb "); // UOTGHS->UOTGHS_DEVEPTISR[%d]=0x%X\n\r", ep, UOTGHS->UOTGHS_DEVEPTISR[ep]);
		////return 0;
    ////}

    //if(ep==0) return 64 - UDD_FifoByteCount(ep & 0xF);  // EP0_SIZE  jcb
    //else return 512 - UDD_FifoByteCount(ep & 0xF);  // EPX_SIZE  jcb
//}

//	Blocking Send of data to an endpoint
uint32_t USBD_Send(uint32_t ep, const void* d, uint32_t len)
{
    uint32_t n;
	int r = len;
	const uint8_t* data = (const uint8_t*)d;

    if (!_usbConfiguration)
    {
    	TRACE_CORE(printf("pb conf\n\r");)
		return -1;
    }

	while (len)
	{
        if(ep==0) n = EP0_SIZE;
        else n =  EPX_SIZE;
		if (n > len)
			n = len;
		len -= n;

		UDD_Send(ep & 0xF, data, n);
		data += n;
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

// Send a USB descriptor string. The string is stored as a
// plain ASCII string but is sent out as UTF-16 with the
// correct 2-byte prefix
static bool USB_SendStringDescriptor(const uint8_t *string, int wLength) {
	uint16_t buff[64];
	int l = 1;
	wLength-=2;
	while (*string && wLength>0) {
		buff[l++] = (uint8_t)(*string++);
		wLength-=2;
	}
	buff[0] = (3<<8) | (l*2);
	return USBD_SendControl(0, (uint8_t*)buff, l*2);
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

int USBD_SendOtherInterfaces(void)
{
	int total = 0;
	uint8_t interfaces = 0;

#ifdef CDC_ENABLED
	total = CDC_GetOtherInterface(&interfaces);
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

static bool USBD_SendOtherConfiguration(int maxlen)
{
	//	Count and measure interfaces
	USBD_InitControl(0);
	//TRACE_CORE(printf("=> USBD_SendConfiguration _cmark1=%d\r\n", _cmark);)
	int interfaces = USBD_SendOtherInterfaces();
	//TRACE_CORE(printf("=> USBD_SendConfiguration _cmark2=%d\r\n", _cmark);)
	//TRACE_CORE(printf("=> USBD_SendConfiguration sizeof=%d\r\n", sizeof(ConfigDescriptor));)

_Pragma("pack(1)")
	ConfigDescriptor config = D_OTHERCONFIG(_cmark + sizeof(ConfigDescriptor),interfaces);
_Pragma("pack()")
	//TRACE_CORE(printf("=> USBD_SendConfiguration clen=%d\r\n", config.clen);)

	//TRACE_CORE(printf("=> USBD_SendConfiguration maxlen=%d\r\n", maxlen);)

	//	Now send them
	USBD_InitControl(maxlen);
	USBD_SendControl(0,&config,sizeof(ConfigDescriptor));
	USBD_SendOtherInterfaces();
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
        if( *desc_addr > setup.wLength ) {
            desc_length = setup.wLength;
        }
	}
	else if (USB_STRING_DESCRIPTOR_TYPE == t)
	{
		TRACE_CORE(puts("=> USBD_SendDescriptor : USB_STRING_DESCRIPTOR_TYPE\r\n");)
		if (setup.wValueL == 0) {
			desc_addr = (const uint8_t*)&STRING_LANGUAGE;
		}
		else if (setup.wValueL == IPRODUCT) {
			return USB_SendStringDescriptor(STRING_PRODUCT, setup.wLength);
		}
		else if (setup.wValueL == IMANUFACTURER) {
			return USB_SendStringDescriptor(STRING_MANUFACTURER, setup.wLength);
		}
		else {
			return false;
		}
		if( *desc_addr > setup.wLength ) {
			desc_length = setup.wLength;
		}
	}
	else if (USB_DEVICE_QUALIFIER == t)
	{
		// Device qualifier descriptor requested
		desc_addr = (const uint8_t*)&USB_DeviceQualifier;
        if( *desc_addr > setup.wLength ) {
            desc_length = setup.wLength;
        }
    }
    else if (USB_OTHER_SPEED_CONFIGURATION == t)
    {
		// Other configuration descriptor requested
		return USBD_SendOtherConfiguration(setup.wLength);
    }
    else
    {
        //printf("Device ERROR");
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


static void USB_SendZlp( void )
{
    while( UOTGHS_DEVEPTISR_TXINI != (UOTGHS->UOTGHS_DEVEPTISR[0] & UOTGHS_DEVEPTISR_TXINI ) )
    {
        if((UOTGHS->UOTGHS_DEVISR & UOTGHS_DEVISR_SUSP) == UOTGHS_DEVISR_SUSP)
        {
            return;
        }
    }
    UOTGHS->UOTGHS_DEVEPTICR[0] = UOTGHS_DEVEPTICR_TXINIC;
}


static void Test_Mode_Support( uint8_t wIndex )
{
    uint8_t i;
	uint8_t *ptr_dest = (uint8_t *) &udd_get_endpoint_fifo_access8(2);

	switch( wIndex )
	{
		case 4:
			//Test mode Test_Packet:
			//Upon command, a port must repetitively transmit the following test packet until
			//the exit action is taken. This enables the testing of rise and fall times, eye
			//patterns, jitter, and any other dynamic waveform specifications.
			//The test packet is made up by concatenating the following strings.
			//(Note: For J/K NRZI data, and for NRZ data, the bit on the left is the first one
			//transmitted. "S" indicates that a bit stuff occurs, which inserts an "extra" NRZI data bit.
			//"* N" is used to indicate N occurrences of a string of bits or symbols.)
			//A port in Test_Packet mode must send this packet repetitively. The inter-packet timing
			//must be no less than the minimum allowable inter-packet gap as defined in Section 7.1.18 and
			//no greater than 125 us.

			// Send ZLP
			USB_SendZlp();

			UOTGHS->UOTGHS_DEVDMA[0].UOTGHS_DEVDMACONTROL = 0; // raz
			UOTGHS->UOTGHS_DEVDMA[1].UOTGHS_DEVDMACONTROL = 0; // raz

			// Configure endpoint 2, 64 bytes, direction IN, type BULK, 1 bank
			UOTGHS->UOTGHS_DEVEPTCFG[2] = UOTGHS_DEVEPTCFG_EPSIZE_64_BYTE
												 | UOTGHS_DEVEPTCFG_EPDIR_IN
												 | UOTGHS_DEVEPTCFG_EPTYPE_BLK
												 | UOTGHS_DEVEPTCFG_EPBK_1_BANK;
			// Check if the configuration is ok
			UOTGHS->UOTGHS_DEVEPTCFG[2] |= UOTGHS_DEVEPTCFG_ALLOC;
			while((UOTGHS->UOTGHS_DEVEPTISR[2]&UOTGHS_DEVEPTISR_CFGOK)==0) {}
			UOTGHS->UOTGHS_DEVEPT |= UOTGHS_DEVEPT_EPEN2;
			// Write FIFO
			for( i=0; i<sizeof(test_packet_buffer); i++)
			{
				ptr_dest[i] = test_packet_buffer[i];;
			}
			// Tst PACKET
			UOTGHS->UOTGHS_DEVCTRL |= UOTGHS_DEVCTRL_TSTPCKT;
			// Send packet
			UOTGHS->UOTGHS_DEVEPTICR[2] = UOTGHS_DEVEPTICR_TXINIC;
			UOTGHS->UOTGHS_DEVEPTIDR[2] = UOTGHS_DEVEPTIDR_FIFOCONC;
			for(;;);
//      break;

		case 1:
			//Test mode Test_J:
			//Upon command, a port's transceiver must enter the high-speed J state and remain in that
			//state until the exit action is taken. This enables the testing of the high output drive
			//level on the D+ line.
			// Send a ZLP
			USB_SendZlp();
			UOTGHS->UOTGHS_DEVCTRL |= UOTGHS_DEVCTRL_TSTJ;
			for(;;);
//      break;

		case 2:
			//Test mode Test_K:
			//Upon command, a port's transceiver must enter the high-speed K state and remain in
			//that state until the exit action is taken. This enables the testing of the high output drive
			//level on the D- line.
			// Send a ZLP
			USB_SendZlp();
			UOTGHS->UOTGHS_DEVCTRL |= UOTGHS_DEVCTRL_TSTK;
			for(;;);
//		break;

		case 3:
			//Test mode Test_SE0_NAK:
			//Upon command, a port's transceiver must enter the high-speed receive mode
			//and remain in that mode until the exit action is taken. This enables the testing
			//of output impedance, low level output voltage, and loading characteristics.
			//In addition, while in this mode, upstream facing ports (and only upstream facing ports)
			//must respond to any IN token packet with a NAK handshake (only if the packet CRC is
			//determined to be correct) within the normal allowed device response time. This enables testing of
			//the device squelch level circuitry and, additionally, provides a general purpose stimulus/response
			//test for basic functional testing.

			// Send a ZLP
			USB_SendZlp();
			UOTGHS->UOTGHS_DEVIDR = UOTGHS_DEVIDR_SUSPEC
							   | UOTGHS_DEVIDR_MSOFEC
							   | UOTGHS_DEVIDR_SOFEC
							   | UOTGHS_DEVIDR_EORSTEC
							   | UOTGHS_DEVIDR_WAKEUPEC
							   | UOTGHS_DEVIDR_EORSMEC
							   | UOTGHS_DEVIDR_UPRSMEC
							   | UOTGHS_DEVIDR_PEP_0
							   | UOTGHS_DEVIDR_PEP_1
							   | UOTGHS_DEVIDR_PEP_2
							   | UOTGHS_DEVIDR_PEP_3
							   | UOTGHS_DEVIDR_PEP_4
							   | UOTGHS_DEVIDR_PEP_5
							   | UOTGHS_DEVIDR_PEP_6
							   | UOTGHS_DEVIDR_DMA_1
							   | UOTGHS_DEVIDR_DMA_2
							   | UOTGHS_DEVIDR_DMA_3
							   | UOTGHS_DEVIDR_DMA_4
							   | UOTGHS_DEVIDR_DMA_5
							   | UOTGHS_DEVIDR_DMA_6;
			for(;;);
//		break;
	}
}


//unsigned int iii=0;
//	Endpoint 0 interrupt
static void USB_ISR(void)
{
//    printf("ISR=0x%X\n\r", UOTGHS->UOTGHS_DEVISR); // jcb
//    if( iii++ > 1500 ) while(1); // jcb
    // End of bus reset
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
		if (USBD_Available(CDC_RX))
			SerialUSB.accept();
	}

	if (Is_udd_sof())
	{
		udd_ack_sof();
	//	USBD_Flush(CDC_TX); // jcb
	}
#endif

	// EP 0 Interrupt
	if (Is_udd_endpoint_interrupt(0) )
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
                if( setup.bmRequestType == 0 )  // device
                {
                    // Send the device status
     				TRACE_CORE(puts(">>> EP0 Int: GET_STATUS\r\n");)
                    // Check current configuration for power mode (if device is configured)
                    // TODO
                    // Check if remote wake-up is enabled
                    // TODO
                    UDD_Send8(EP0, 0); // TODO
	    			UDD_Send8(EP0, 0);
                }
                // if( setup.bmRequestType == 2 ) // Endpoint:
                else
                {
                    // Send the endpoint status
                    // Check if the endpoint if currently halted
                    if( isEndpointHalt == 1 )
    				UDD_Send8(EP0, 1); // TODO
                    else
    				UDD_Send8(EP0, 0); // TODO
	    			UDD_Send8(EP0, 0);
                }
			}
			else if (CLEAR_FEATURE == r)
			{
               // Check which is the selected feature
                if( setup.wValueL == 1) // DEVICEREMOTEWAKEUP
                {
                    // Enable remote wake-up and send a ZLP
                    if( isRemoteWakeUpEnabled == 1 )
	    			UDD_Send8(EP0, 1);
                    else
	    			UDD_Send8(EP0, 0);
                    UDD_Send8(EP0, 0);
                }
                else // if( setup.wValueL == 0) // ENDPOINTHALT
                {
                    isEndpointHalt = 0;  // TODO
    				UDD_Send8(EP0, 0);
	    			UDD_Send8(EP0, 0);
                }

 			}
			else if (SET_FEATURE == r)
			{
                // Check which is the selected feature
                if( setup.wValueL == 1) // DEVICEREMOTEWAKEUP
                {
                    // Enable remote wake-up and send a ZLP
                    isRemoteWakeUpEnabled = 1;
	    			UDD_Send8(EP0, 0);
                }
                if( setup.wValueL == 0) // ENDPOINTHALT
                {
                    // Halt endpoint
                    isEndpointHalt = 1;
                    //USBD_Halt(USBGenericRequest_GetEndpointNumber(pRequest));
	    			UDD_Send8(EP0, 0);
                }
                if( setup.wValueL == 2) // TEST_MODE
                {
                    // 7.1.20 Test Mode Support, 9.4.9 SetFeature
                    if( (setup.bmRequestType == 0 /*USBGenericRequest_DEVICE*/) &&
                        ((setup.wIndex & 0x000F) == 0) )
                    {
                        // the lower byte of wIndex must be zero
                        // the most significant byte of wIndex is used to specify the specific test mode

                        UOTGHS->UOTGHS_DEVIDR &= ~UOTGHS_DEVIDR_SUSPEC;
                        UOTGHS->UOTGHS_DEVCTRL |= UOTGHS_DEVCTRL_SPDCONF_HIGH_SPEED; // remove suspend ?

                        Test_Mode_Support( (setup.wIndex & 0xFF00)>>8 );
                    }
                }
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
				UDD_Send8(EP0, _usbConfiguration);
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
				UDD_Send8(EP0, _usbSetInterface);
			}
			else if (SET_INTERFACE == r)
			{
                _usbSetInterface = setup.wValueL;
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
