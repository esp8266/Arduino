

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

#if 0

#if defined(USBCON)

#define NUM_IT_MAX 3

#define EP_SINGLE_64 0x32	// EP0
#define EP_DOUBLE_64 0x36	// Other endpoints


// Endpoiont 0:
#define EP_TYPE_CONTROL				UDPHS_EPTCFG_EPT_SIZE_64 \
                                  | UDPHS_EPTCFG_EPT_TYPE_CTRL8 \
                                  | UDPHS_EPTCFG_BK_NUMBER_1
#ifdef CDC_ENABLED
#define EP_TYPE_BULK_IN				UDPHS_EPTCFG_EPT_SIZE_512 \
                                  | UDPHS_EPTCFG_EPT_DIR \
                                  | UDPHS_EPTCFG_EPT_TYPE_BULK \
                                  | UDPHS_EPTCFG_BK_NUMBER_2
#define EP_TYPE_BULK_OUT			UDPHS_EPTCFG_EPT_SIZE_512 \
                                  | UDPHS_EPTCFG_EPT_TYPE_BULK \
                                  | UDPHS_EPTCFG_BK_NUMBER_2
#define EP_TYPE_INTERRUPT_IN		UDPHS_EPTCFG_EPT_SIZE_64 \
                                  | UDPHS_EPTCFG_EPT_DIR \
                                  | UDPHS_EPTCFG_EPT_TYPE_INT \
                                  | UDPHS_EPTCFG_BK_NUMBER_2
#endif

#ifdef HID_ENABLED
#define EP_TYPE_INTERRUPT_IN_HID	UDPHS_EPTCFG_EPT_SIZE_64 \
                                  | UDPHS_EPTCFG_EPT_DIR \
                                  | UDPHS_EPTCFG_EPT_TYPE_INT \
                                  | UDPHS_EPTCFG_BK_NUMBER_2
#endif

#define EP_TYPE_INTERRUPT_OUT		UDPHS_EPTCFG_EPT_SIZE_64 \
                                  | UDPHS_EPTCFG_EPT_TYPE_INT \
                                  | UDPHS_EPTCFG_EPT_TYPE_INT \
                                  | UDPHS_EPTCFG_BK_NUMBER_1
#define EP_TYPE_ISOCHRONOUS_IN		UDPHS_EPTCFG_EPT_SIZE_1024 \
                                  | UDPHS_EPTCFG_EPT_DIR \
                                  | UDPHS_EPTCFG_EPT_TYPE_ISO \
                                  | UDPHS_EPTCFG_BK_NUMBER_3
#define EP_TYPE_ISOCHRONOUS_OUT		UDPHS_EPTCFG_EPT_SIZE_1024 \
                                  | UDPHS_EPTCFG_EPT_TYPE_ISO \
                                  | UDPHS_EPTCFG_BK_NUMBER_3

extern const u8 _initEndpoints[] ;
const u8 _initEndpoints[] =
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
volatile u8 TxLEDPulse; /**< Milliseconds remaining for data Tx LED pulse */
volatile u8 RxLEDPulse; /**< Milliseconds remaining for data Rx LED pulse */

//==================================================================
//==================================================================

extern const u16 STRING_LANGUAGE[] ;
extern const u16 STRING_IPRODUCT[] ;
extern const u16 STRING_IMANUFACTURER[] ;
extern const DeviceDescriptor USB_DeviceDescriptor ;
extern const DeviceDescriptor USB_DeviceDescriptorA ;

const u16 STRING_LANGUAGE[2] = {
	(3<<8) | (2+2),
	0x0409	// English
};

const u16 STRING_IPRODUCT[17] = {
	(3<<8) | (2+2*16),
#if USB_PID == USB_PID_LEONARDO
	'A','r','d','u','i','n','o',' ','L','e','o','n','a','r','d','o'
#elif USB_PID == USB_PID_MICRO
	'A','r','d','u','i','n','o',' ','M','i','c','r','o',' ',' ',' '
#elif USB_PID == ARDUINO_MODEL_USB_PID
	'A','r','d','u','i','n','o',' ','D','u','e',' ',' ',' ',' ',' '
#else
#error "Need an USB PID"
#endif
};

const u16 STRING_IMANUFACTURER[12] = {
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

volatile u8 _usbConfiguration = 0;
// Global variable for endpoint number
unsigned int NumEndpoint=0;


#include "../../../system/libsam/cmsis/sam3u/include/sam3u.h"

#ifndef TXLED1
#define TXLED0
#define RXLED0
#define TXLED1 
#define RXLED1 
#endif


/// Max size of the FMA FIFO
#define EPT_VIRTUAL_SIZE     16384
#define SHIFT_INTERUPT    8

static inline void WaitIN(void)
{
//	while (!(UEINTX & (1<<TXINI)));
    while (!(UDPHS->UDPHS_EPT[0].UDPHS_EPTSTA & UDPHS_EPTSTA_TX_PK_RDY));
}

static inline void ClearIN(void)
{
//	UEINTX = ~(1<<TXINI);
   UDPHS->UDPHS_EPT[NumEndpoint].UDPHS_EPTCLRSTA = UDPHS_EPTCLRSTA_TX_COMPLT; 
}

static inline void WaitOUT(void)
{
//	while (!(UEINTX & (1<<RXOUTI)))
//		;
    // Waiting for Status stage
    while (UDPHS_EPTSTA_RX_BK_RDY != (UDPHS->UDPHS_EPT[NumEndpoint].UDPHS_EPTSTA & UDPHS_EPTSTA_RX_BK_RDY));
}

static inline u8 WaitForINOrOUT()
{
//	while (!(UEINTX & ((1<<TXINI)|(1<<RXOUTI))))
//		;
//	return (UEINTX & (1<<RXOUTI)) == 0;
    while (!(UDPHS->UDPHS_EPT[NumEndpoint].UDPHS_EPTSTA & (UDPHS_EPTSTA_RX_BK_RDY | UDPHS_EPTSTA_TX_PK_RDY)));
  	return (UDPHS->UDPHS_EPT[NumEndpoint].UDPHS_EPTSTA & UDPHS_EPTSTA_RX_BK_RDY) == 0;
}

static inline void ClearOUT(void)
{
//	UEINTX = ~(1<<RXOUTI);
    UDPHS->UDPHS_EPT[NumEndpoint].UDPHS_EPTCLRSTA = UDPHS_EPTCLRSTA_RX_BK_RDY;
}

/*
static void UDPHS_ClearRxFlag( unsigned char bEndpoint )
{
    UDPHS->UDPHS_EPT[NumEndpoint].UDPHS_EPTCLRSTA = UDPHS_EPTCLRSTA_RX_BK_RDY;
}
*/

#define UDPHS_EPTFIFO (0x20180000) // (UDPHS_EPTFIFO) Base Address

static void Recv(volatile u8* data, u8 count)
{
    u8     *pFifo;

    pFifo = (u8*)((u32 *)UDPHS_EPTFIFO + (EPT_VIRTUAL_SIZE * NumEndpoint));

    while (count--)
		*data++ = pFifo[0]; // UEDATX;

	RXLED1;					// light the RX LED
	RxLEDPulse = TX_RX_LED_PULSE_MS;
}


static inline u8 Recv8()
{
    u8     *pFifo;

	RXLED1;					// light the RX LED
	RxLEDPulse = TX_RX_LED_PULSE_MS;

    pFifo = (u8*)((u32 *)UDPHS_EPTFIFO + (EPT_VIRTUAL_SIZE * NumEndpoint));

//	return UEDATX;
    return (pFifo[0]);
}

static inline void Send8(u8 d)
{
    u8     *pFifo;
    pFifo = (u8*)((u32 *)UDPHS_EPTFIFO + (EPT_VIRTUAL_SIZE * NumEndpoint));
//	UEDATX = d;
    pFifo[0] =d;
}

static inline void SetEP(u8 ep)
{
//	UENUM = ep;
	NumEndpoint = ep & 7;
}

static inline u16 FifoByteCount()
{
//	return UEBCLX;
    // SAM3X
    //return ((UOTGHS->UOTGHS_DEVEPTISR[ep] & UOTGHS_DEVEPTISR_BYCT_Msk) >> UOTGHS_DEVEPTISR_BYCT_Pos);
    // SAM3U //AT91C_UDPHS_BYTE_COUNT (0x7FF << 20)
    return ((UDPHS->UDPHS_EPT[NumEndpoint].UDPHS_EPTSTA & (0x7FF << 20)) >> 20);
}

static inline u8 ReceivedSetupInt()
{
//	return UEINTX & (1<<RXSTPI);
    return ( UDPHS_EPTSTA_RX_SETUP == (UDPHS->UDPHS_EPT[NumEndpoint].UDPHS_EPTSTA & UDPHS_EPTSTA_RX_SETUP) );
}

static inline void ClearSetupInt()
{
//	UEINTX = ~((1<<RXSTPI) | (1<<RXOUTI) | (1<<TXINI));
    UDPHS->UDPHS_EPT[NumEndpoint].UDPHS_EPTCLRSTA = UDPHS_EPTSTA_RX_SETUP | UDPHS_EPTCLRSTA_RX_BK_RDY | UDPHS_EPTCLRSTA_TX_COMPLT;
}

static inline void Stall()
{
//	UECONX = (1<<STALLRQ) | (1<<EPEN);
    UDPHS->UDPHS_EPT[NumEndpoint].UDPHS_EPTSETSTA = UDPHS_EPTSETSTA_FRCESTALL;
}

static inline u8 ReadWriteAllowed()
{
	//return UEINTX & (1<<RWAL);
    return 1;
}

static inline u8 Stalled()
{
//	return UEINTX & (1<<STALLEDI);
    // Check if the data has been STALLed
    return ( UDPHS_EPTSTA_FRCESTALL == (UDPHS->UDPHS_EPT[NumEndpoint].UDPHS_EPTSTA & UDPHS_EPTSTA_FRCESTALL));
}

static inline u8 FifoFree()
{
//	return UEINTX & (1<<FIFOCON);
    return( 0 != (UDPHS->UDPHS_EPT[NumEndpoint].UDPHS_EPTSTA & UDPHS_EPTSTA_TX_PK_RDY ));
}

//static inline void ReleaseRX()
//{
//	UEINTX = 0x6B;	// FIFOCON=0 NAKINI=1 RWAL=1 NAKOUTI=0 RXSTPI=1 RXOUTI=0 STALLEDI=1 TXINI=1
//}

//static inline void ReleaseTX()
//{
//	UEINTX = 0x3A;	// FIFOCON=0 NAKINI=0 RWAL=1 NAKOUTI=1 RXSTPI=1 RXOUTI=0 STALLEDI=1 TXINI=0
//}
#define UDFNUML ((UDPHS->UDPHS_FNUM & UDPHS_FNUM_FRAME_NUMBER_Msk)>>3)

static inline u8 FrameNumber()
{
	return UDFNUML;
}

//==================================================================
//==================================================================

u8 USBGetConfiguration(void)
{
	return _usbConfiguration;
}

#define USB_RECV_TIMEOUT

//	Number of bytes, assumes a rx endpoint
u8 USB_Available(u8 ep)
{
	SetEP(ep);
	return FifoByteCount();
}

//	Non Blocking receive
//	Return number of bytes read
int USB_Recv(u8 ep, void* d, int len)
{
	if (!_usbConfiguration || len < 0)
		return -1;

	SetEP(ep);
	u8 n = FifoByteCount();
	len = min(n,len);
	n = len;
	u8* dst = (u8*)d;
	while (n--)
		*dst++ = Recv8();
//	if (len && !FifoByteCount())	// release empty buffer
//		ReleaseRX();

	return len;
}

//	Recv 1 byte if ready
int USB_Recv(u8 ep)
{
	u8 c;
	if (USB_Recv(ep,&c,1) != 1)
		return -1;
	return c;
}

//	Space in send EP
u8 USB_SendSpace(u8 ep)
{
	SetEP(ep);
	if (!ReadWriteAllowed())
		return 0;
	return 64 - FifoByteCount();
}

//	Blocking Send of data to an endpoint
int USB_Send(u8 ep, const void* d, int len)
{
	if (!_usbConfiguration)
		return -1;

	int r = len;
	const u8* data = (const u8*)d;
	u8 zero = ep & TRANSFER_ZERO;
	u8 timeout = 250;		// 250ms timeout on send? TODO
	while (len)
	{
		u8 n = USB_SendSpace(ep);
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
		{
			SetEP(ep);
			if (ep & TRANSFER_ZERO)
			{
				while (n--)
					Send8(0);
			}
			else if (ep & TRANSFER_PGM)
			{
				while (n--)
					Send8(*data++);
			}
			else
			{
				while (n--)
					Send8(*data++);
			}
//			if (!ReadWriteAllowed() || ((len == 0) && (ep & TRANSFER_RELEASE)))	// Release full buffer
//				ReleaseTX();
		}
	}
	TXLED1;					// light the TX LED
	TxLEDPulse = TX_RX_LED_PULSE_MS;
	return r;
}


//static
//void InitEP(u8 index, u8 type, u8 size)
//{
//	UENUM = index;
//	UECONX = 1;
//	UECFG0X = type;
//	UECFG1X = size;
//}


static
void InitEndpoints()
{
	for (u8 i = 1; i < sizeof(_initEndpoints); i++)
	{
        // Reset Endpoint Fifos
        UDPHS->UDPHS_EPT[i].UDPHS_EPTCLRSTA = UDPHS_EPTCLRSTA_TOGGLESQ | UDPHS_EPTCLRSTA_FRCESTALL;
        UDPHS->UDPHS_EPTRST = 1<<i;

		//UECONX = 1;
		//UECFG0X = pgm_read_byte(_initEndpoints+i);
        UDPHS->UDPHS_EPT[i].UDPHS_EPTCFG = _initEndpoints[i];

        while( (signed int)UDPHS_EPTCFG_EPT_MAPD != (signed int)((UDPHS->UDPHS_EPT[i].UDPHS_EPTCFG) & (unsigned int)UDPHS_EPTCFG_EPT_MAPD) )
        ;
        UDPHS->UDPHS_EPT[i].UDPHS_EPTCTLENB = UDPHS_EPTCTLENB_EPT_ENABL;

        //		UECFG1X = EP_DOUBLE_64;
	}
///\//	UERST = 0x7E;	// And reset them
///\//	UERST = 0;
}

//	Handle CLASS_INTERFACE requests
static
bool ClassInterfaceRequest(Setup& setup)
{
	u8 i = setup.wIndex;

#ifdef CDC_ENABLED
	if (CDC_ACM_INTERFACE == i)
		return CDC_Setup(setup);
#endif

#ifdef HID_ENABLED
	if (HID_INTERFACE == i)
		return HID_Setup(setup);
#endif
	return false;
}

int _cmark;
int _cend;
void InitControl(int end)
{
	SetEP(0);
    UDPHS->UDPHS_EPT[0].UDPHS_EPTCFG = _initEndpoints[0];
    while( (signed int)UDPHS_EPTCFG_EPT_MAPD != (signed int)((UDPHS->UDPHS_EPT[0].UDPHS_EPTCFG) & (unsigned int)UDPHS_EPTCFG_EPT_MAPD) )
    ;
    UDPHS->UDPHS_EPT[0].UDPHS_EPTCTLENB = UDPHS_EPTCTLENB_RX_BK_RDY 
                                         | UDPHS_EPTCTLENB_RX_SETUP
                                         | UDPHS_EPTCTLENB_EPT_ENABL;

	_cmark = 0;
	_cend = end;
}

static
bool SendControl(u8 d)
{
	if (_cmark < _cend)
	{
		if (!WaitForINOrOUT())
			return false;
		Send8(d);
		if (!((_cmark + 1) & 0x3F))
			ClearIN();	// Fifo is full, release this packet
	}
	_cmark++;
	return true;
};

//	Clipped by _cmark/_cend
int USB_SendControl(u8 flags, const void* d, int len)
{
	int sent = len;
	const u8* data = (const u8*)d;
	bool pgm = flags & TRANSFER_PGM;
	while (len--)
	{
		u8 c = pgm ? *data++ : *data++;
		if (!SendControl(c))
			return -1;
	}
	return sent;
}

//	Does not timeout or cross fifo boundaries
//	Will only work for transfers <= 64 bytes
//	TODO
int USB_RecvControl(void* d, int len)
{
	WaitOUT();
	Recv((u8*)d,len);
	ClearOUT();
	return len;
}

int SendInterfaces()
{
	int total = 0;
	u8 interfaces = 0;

#ifdef CDC_ENABLED
	total = CDC_GetInterface(&interfaces);
#endif

#ifdef HID_ENABLED
	total += HID_GetInterface(&interfaces);
#endif

	return interfaces;
}

//	Construct a dynamic configuration descriptor
//	This really needs dynamic endpoint allocation etc
//	TODO
static
bool SendConfiguration(int maxlen)
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

u8 _cdcComposite = 0;

static
bool SendDescriptor(Setup& setup)
{
	u8 t = setup.wValueH;
	if (USB_CONFIGURATION_DESCRIPTOR_TYPE == t)
		return SendConfiguration(setup.wLength);

	InitControl(setup.wLength);
#ifdef HID_ENABLED
	if (HID_REPORT_DESCRIPTOR_TYPE == t)
		return HID_GetDescriptor(t);
#endif

	u8 desc_length = 0;
	const u8* desc_addr = 0;
	if (USB_DEVICE_DESCRIPTOR_TYPE == t)
	{
		if (setup.wLength == 8)
			_cdcComposite = 1;
		desc_addr = _cdcComposite ?  (const u8*)&USB_DeviceDescriptorA : (const u8*)&USB_DeviceDescriptor;
	}
	else if (USB_STRING_DESCRIPTOR_TYPE == t)
	{
		if (setup.wValueL == 0)
			desc_addr = (const u8*)&STRING_LANGUAGE;
		else if (setup.wValueL == IPRODUCT)
			desc_addr = (const u8*)&STRING_IPRODUCT;
		else if (setup.wValueL == IMANUFACTURER)
			desc_addr = (const u8*)&STRING_IMANUFACTURER;
		else
			return false;
	}

	if (desc_addr == 0)
		return false;
	if (desc_length == 0)
		desc_length = *desc_addr;

	USB_SendControl(TRANSFER_PGM,desc_addr,desc_length);
	return true;
}

//	Endpoint 0 interrupt
//ISR(USB_COM_vect)
void USB_ISR()
{
    SetEP(0);
	if (!ReceivedSetupInt())
		return;

	Setup setup;
	Recv((u8*)&setup,8);
	ClearSetupInt();

	u8 requestType = setup.bmRequestType;
	if (requestType & REQUEST_DEVICETOHOST)
		WaitIN();
	else
		ClearIN();

    bool ok = true;
	if (REQUEST_STANDARD == (requestType & REQUEST_TYPE))
	{
		//	Standard Requests
		u8 r = setup.bRequest;
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
			} else
				ok = false;
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
		ClearIN();
	else
	{
		Stall();
	}
}

void USB_Flush(u8 ep)
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
    u8 udint = UDINT;
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

//ISR(USB_GEN_vect)
void USB_GEN_ISR()
{
    unsigned int  status;
    unsigned char numIT;

    // Get interrupts status
    status = UDPHS->UDPHS_INTSTA & UDPHS->UDPHS_IEN;

    // Handle all UDPHS interrupts
    while (status != 0) {

        //	Start of Frame - happens every millisecond so we use it for TX and RX LED one-shot timing, too
        if ((status & UDPHS_IEN_INT_SOF) != 0) {

#ifdef CDC_ENABLED
            USB_Flush(CDC_TX);				// Send a tx frame if found
#endif

            // check whether the one-shot period has elapsed.  if so, turn off the LED
            if (TxLEDPulse && !(--TxLEDPulse))
                TXLED0;
            if (RxLEDPulse && !(--RxLEDPulse))
                RXLED0;

            // Acknowledge interrupt
            UDPHS->UDPHS_CLRINT = UDPHS_CLRINT_INT_SOF;
            status &= ~UDPHS_IEN_INT_SOF;
        }
        // Suspend
        // This interrupt is always treated last (hence the '==')
        else if (status == UDPHS_IEN_DET_SUSPD) {

            //UDPHS_DisableBIAS();

            // Enable wakeup
            UDPHS->UDPHS_IEN |= UDPHS_IEN_WAKE_UP | UDPHS_IEN_ENDOFRSM;
            UDPHS->UDPHS_IEN &= ~UDPHS_IEN_DET_SUSPD;

            // Acknowledge interrupt
            UDPHS->UDPHS_CLRINT = UDPHS_CLRINT_DET_SUSPD | UDPHS_CLRINT_WAKE_UP;

            //UDPHS_DisableUsbClock();

        }
        // Resume
        else if( ((status & UDPHS_IEN_WAKE_UP) != 0)      // line activity
              || ((status & UDPHS_IEN_ENDOFRSM) != 0))  { // pc wakeup
            {

                //UDPHS_EnableUsbClock();
                //UDPHS_EnableBIAS();

                UDPHS->UDPHS_CLRINT = UDPHS_CLRINT_WAKE_UP | UDPHS_CLRINT_ENDOFRSM
                		| UDPHS_CLRINT_DET_SUSPD;

                UDPHS->UDPHS_IEN |= UDPHS_IEN_ENDOFRSM | UDPHS_IEN_DET_SUSPD;
                UDPHS->UDPHS_CLRINT = UDPHS_CLRINT_WAKE_UP | UDPHS_CLRINT_ENDOFRSM;
                UDPHS->UDPHS_IEN &= ~UDPHS_IEN_WAKE_UP;
            }
        }
        // End of Reset
        else if ((status & UDPHS_IEN_ENDRESET) == UDPHS_IEN_ENDRESET) {

        	InitControl(0);	// init ep0
            _usbConfiguration = 0;			// not configured yet
            //UEIENX = 1 << RXSTPE;			// Enable interrupts for ep0

            //UDPHS_ResetEndpoints();
            //UDPHS_DisableEndpoints();
            //USBD_ConfigureEndpoint(0);
            UDPHS->UDPHS_IEN |= (1<<SHIFT_INTERUPT<<0);

            // Flush and enable the Suspend interrupt
            UDPHS->UDPHS_CLRINT = UDPHS_CLRINT_WAKE_UP | UDPHS_CLRINT_DET_SUSPD;

            //// Enable the Start Of Frame (SOF) interrupt if needed
            UDPHS->UDPHS_IEN |= UDPHS_IEN_INT_SOF;

            // Acknowledge end of bus reset interrupt
            UDPHS->UDPHS_CLRINT = UDPHS_CLRINT_ENDRESET;

            UDPHS->UDPHS_IEN |= UDPHS_IEN_DET_SUSPD;
        }
        // Handle upstream resume interrupt
        else if (status & UDPHS_IEN_UPSTR_RES) {

            // - Acknowledge the IT
            UDPHS->UDPHS_CLRINT = UDPHS_CLRINT_UPSTR_RES;
        }
        // Endpoint interrupts
        else {
            // Handle endpoint interrupts
            for (numIT = 0; numIT < NUM_IT_MAX; numIT++) {

                if ((status & (1 << SHIFT_INTERUPT << numIT)) != 0) {
                	USB_ISR();
                    //EndpointHandler(numIT); // TODO: interrupt for bulk
                }
            }
        }
        // Retrieve new interrupt status
        status = UDPHS->UDPHS_INTSTA & UDPHS->UDPHS_IEN;
    }
}

//	VBUS or counting frames
//	Any frame counting?
u8 USBConnected()
{
	u8 f = UDFNUML;
	delay(3);
	return f != UDFNUML;
}


//=======================================================================
//=======================================================================

USB_ USB;

USB_::USB_()
{
}

void USB_::attach()
{/*
	_usbConfiguration = 0;

	//UHWCON = 0x01;						// power internal reg
	//USBCON = (1<<USBE)|(1<<FRZCLK);		// clock frozen, usb enabled
	//PLLCSR = 0x12;						// Need 16 MHz xtal
	//while (!(PLLCSR & (1<<PLOCK)))		// wait for lock pll
	//	;
    PMC->PMC_PCER = (1 << ID_UDPHS);
    // Enable 480MHZ
    //AT91C_BASE_CKGR->CKGR_UCKR |= (AT91C_CKGR_PLLCOUNT & (3 << 20)) | AT91C_CKGR_UPLLEN;
    CKGR->CKGR_UCKR |= ((0xf << 20) & (3 << 20)) | AT91C_CKGR_UPLLEN;
    // Wait until UTMI PLL is locked
    while ((PMC->PMC_SR & PMC_LOCKU) == 0);

    // Reset and enable IP UDPHS
    UDPHS->UDPHS_CTRL &= ~UDPHS_CTRL_EN_UDPHS;
    UDPHS->UDPHS_CTRL |= UDPHS_CTRL_EN_UDPHS;

	//USBCON = ((1<<USBE)|(1<<OTGPADE));	// start USB clock
    UDPHS->UDPHS_IEN = 0;
    UDPHS->UDPHS_CLRINT = UDPHS_CLRINT_UPSTR_RES
                                   | UDPHS_CLRINT_ENDOFRSM
                                   | UDPHS_CLRINT_WAKE_UP
                                   | UDPHS_CLRINT_ENDRESET
                                   | UDPHS_CLRINT_INT_SOF
                                   | UDPHS_CLRINT_MICRO_SOF
                                   | UDPHS_CLRINT_DET_SUSPD;

    // Enable interrupts for EOR (End of Reset), wake up and SOF (start of frame)
    //UDIEN = (1<<EORSTE)|(1<<SOFE);
    UDPHS->UDPHS_IEN = UDPHS_IEN_ENDOFRSM
                                | UDPHS_IEN_WAKE_UP
                                | UDPHS_IEN_DET_SUSPD;

	// enable attach resistor
	//UDCON = 0;
    UDPHS->UDPHS_CTRL &= ~UDPHS_CTRL_DETACH;   // Pull Up on DP
    UDPHS->UDPHS_CTRL |= UDPHS_CTRL_PULLD_DIS; // Disable Pull Down

	TX_RX_LED_INIT;
*/}

void USB_::detach()
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

#endif
