

#ifndef __USBAPI__
#define __USBAPI__

#if defined(USBCON)

//================================================================================
//================================================================================
//	USB

class USB_
{
public:
	USB_();
	bool configured();

	void attach();
	void detach();	// Serial port goes down too...
	void poll();
};
extern USB_ USB;

//================================================================================
//================================================================================
//	Serial over CDC (Serial1 is the physical port)

class Serial_
{
public:
	void begin(uint16_t baud_count);
	void end(void);

	virtual int available(void);
	virtual int peek(void);
	virtual int read(void);
	virtual void flush(void);
	virtual size_t write(uint8_t);
};
extern Serial_ Serial;

//================================================================================
//================================================================================
//	Low level API

typedef struct
{
	uint8_t bmRequestType;
	uint8_t bRequest;
	uint8_t wValueL;
	uint8_t wValueH;
	uint16_t wIndex;
	uint16_t wLength;
} Setup;

//================================================================================
//================================================================================
//	CDC 'Driver'

int		CDC_GetInterface(uint8_t* interfaceNum);
int		CDC_GetDescriptor(int i);
bool	CDC_Setup(Setup& setup);

//================================================================================
//================================================================================

#define TRANSFER_PGM		0x80
#define TRANSFER_RELEASE	0x40
#define TRANSFER_ZERO		0x20

int USB_SendControl(uint8_t flags, const void* d, int len);
int USB_RecvControl(void* d, int len);

uint8_t	USB_Available(uint8_t ep);
int USB_Send(uint8_t ep, const void* data, int len);	// blocking
int USB_Recv(uint8_t ep, void* data, int len);		// non-blocking
void USB_Recv_block(u8 ep, u8* dst, int len);		// blocking
int USB_Recv(uint8_t ep);							// non-blocking
void USB_Flush(uint8_t ep);

#endif

#endif /* if defined(USBCON) */