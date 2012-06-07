/* Copyright 2009-2011 Oleg Mazurov, Circuits At Home, http://www.circuitsathome.com */
/* USB functions */
#ifndef USB_H_INCLUDED
#define USB_H_INCLUDED

#include <stdint.h>
#include "ch9.h"

/* Common setup data constant combinations  */
#define bmREQ_GET_DESCR     USB_SETUP_DEVICE_TO_HOST|USB_SETUP_TYPE_STANDARD|USB_SETUP_RECIPIENT_DEVICE     // Get descriptor request type
#define bmREQ_SET           USB_SETUP_HOST_TO_DEVICE|USB_SETUP_TYPE_STANDARD|USB_SETUP_RECIPIENT_DEVICE     // Set request type for all but 'set feature' and 'set interface'
#define bmREQ_CL_GET_INTF   USB_SETUP_DEVICE_TO_HOST|USB_SETUP_TYPE_CLASS|USB_SETUP_RECIPIENT_INTERFACE     // Get interface request type

/* HID requests */
#define bmREQ_HIDOUT        USB_SETUP_HOST_TO_DEVICE|USB_SETUP_TYPE_CLASS|USB_SETUP_RECIPIENT_INTERFACE
#define bmREQ_HIDIN         USB_SETUP_DEVICE_TO_HOST|USB_SETUP_TYPE_CLASS|USB_SETUP_RECIPIENT_INTERFACE
#define bmREQ_HIDREPORT     USB_SETUP_DEVICE_TO_HOST|USB_SETUP_TYPE_STANDARD|USB_SETUP_RECIPIENT_INTERFACE

#define USB_XFER_TIMEOUT    5000    // USB transfer timeout in milliseconds, per section 9.2.6.1 of USB 2.0 spec
#define USB_NAK_LIMIT       32000   // NAK limit for a transfer. o means NAKs are not counted
#define USB_RETRY_LIMIT     3       // Retry limit for a transfer
#define USB_SETTLE_DELAY    200     // Settle delay in milliseconds
#define USB_NAK_NOWAIT      1       // Used in Richard's PS2/Wiimote code

#define USB_NUMDEVICES  2           // Number of USB devices

/* USB state machine states */
#define USB_STATE_DETACHED                                  0x10
#define USB_DETACHED_SUBSTATE_INITIALIZE                    0x11
#define USB_DETACHED_SUBSTATE_WAIT_FOR_DEVICE               0x12
#define USB_DETACHED_SUBSTATE_ILLEGAL                       0x13
#define USB_ATTACHED_SUBSTATE_SETTLE                        0x20
#define USB_ATTACHED_SUBSTATE_RESET_DEVICE                  0x30
#define USB_ATTACHED_SUBSTATE_WAIT_RESET_COMPLETE           0x40
#define USB_ATTACHED_SUBSTATE_WAIT_SOF                      0x50
#define USB_ATTACHED_SUBSTATE_GET_DEVICE_DESCRIPTOR_SIZE    0x60
#define USB_STATE_ADDRESSING                                0x70
#define USB_STATE_CONFIGURING                               0x80
#define USB_STATE_RUNNING                                   0x90
#define USB_STATE_ERROR                                     0xa0
#define USB_STATE_MASK                                      0xf0

/* USB Setup Packet Structure   */
typedef struct
{
    union
	{                          // offset   description
        uint8_t bmRequestType;         //   0      Bit-map of request type
        struct
		{
            uint8_t    recipient:  5;  //          Recipient of the request
            uint8_t    type:       2;  //          Type of request
            uint8_t    direction:  1;  //          Direction of data X-fer
        };
    } ReqType_u;
    uint8_t    bRequest;               //   1      Request
    union
	{
        uint16_t    wValue;             //   2      Depends on bRequest
        struct
		{
			uint8_t    wValueLo;
			uint8_t    wValueHi;
        };
    } wVal_u;
    uint16_t    wIndex;                 //   4      Depends on bRequest
    uint16_t    wLength;                //   6      Depends on bRequest
} SETUP_PKT, *PSETUP_PKT;

/* Endpoint information structure               */
/* bToggle of endpoint 0 initialized to 0xff    */
/* during enumeration bToggle is set to 00      */
typedef struct
{
    uint32_t epAddr;       // Copy from endpoint descriptor. Bit 7 indicates direction (ignored for control endpoints)
    uint8_t Attr;          // Endpoint transfer type.
    uint16_t MaxPktSize;   // Maximum packet size.
    uint8_t Interval;      // Polling interval in frames.
    uint8_t sndToggle;     // Last toggle value, bitmask for HCTL toggle bits
    uint8_t rcvToggle;     // Last toggle value, bitmask for HCTL toggle bits
    /* not sure if both are necessary */
} EP_RECORD;

/* Device record structure */
typedef struct
{
    EP_RECORD* epinfo;     // Device endpoint information
    uint8_t devclass;      // Device class
} DEV_RECORD;

/**
 * USBHost Class
 * The device table is filled during enumeration.
 * Index corresponds to device address and each entry contains pointer to endpoint structure and device class to use.
 */
class USBHost
{
    public:
		USBHost(void);
		uint32_t getUsbTaskState(void);
		void setUsbTaskState(uint32_t state);
		EP_RECORD* getDevTableEntry(uint32_t addr, uint32_t ep);
		void setDevTableEntry(uint32_t addr, EP_RECORD* eprecord_ptr);
		uint32_t ctrlReq(uint32_t addr, uint32_t ep, uint8_t bmReqType, uint8_t bRequest, uint8_t wValLo, uint8_t wValHi, uint16_t wInd, uint32_t nbytes, uint8_t* dataptr, uint32_t nak_limit = USB_NAK_LIMIT);

		/* Control requests */
		uint32_t getDevDescr(uint32_t addr, uint32_t ep, uint32_t nbytes, uint8_t* dataptr, uint32_t nak_limit = USB_NAK_LIMIT);
		uint32_t getConfDescr(uint32_t addr, uint32_t ep, uint32_t nbytes, uint8_t conf, uint8_t* dataptr, uint32_t nak_limit = USB_NAK_LIMIT);
		uint32_t getStrDescr(uint32_t addr, uint32_t ep, uint32_t nbytes, uint8_t index, uint16_t langid, uint8_t* dataptr, uint32_t nak_limit = USB_NAK_LIMIT);
		uint32_t setAddr(uint32_t oldaddr, uint32_t ep, uint32_t newaddr, uint32_t nak_limit = USB_NAK_LIMIT);
		uint32_t setConf(uint32_t addr, uint32_t ep, uint8_t conf_value, uint32_t nak_limit = USB_NAK_LIMIT);

		/* Status requests */
		uint32_t setProto(uint32_t addr, uint32_t ep, uint8_t interface, uint8_t protocol, uint32_t nak_limit = USB_NAK_LIMIT);
		uint32_t getProto(uint32_t addr, uint32_t ep, uint8_t interface, uint8_t* dataptr, uint32_t nak_limit = USB_NAK_LIMIT);
		uint32_t getReportDescr(uint32_t addr, uint32_t ep, uint32_t nbytes, uint8_t* dataptr, uint32_t nak_limit = USB_NAK_LIMIT);
		uint32_t setReport(uint32_t addr, uint32_t ep, uint32_t nbytes, uint8_t interface, uint8_t report_type, uint8_t report_id, uint8_t* dataptr, uint32_t nak_limit = USB_NAK_LIMIT);
		uint32_t getReport(uint32_t addr, uint32_t ep, uint32_t nbytes, uint8_t interface, uint8_t report_type, uint8_t report_id, uint8_t* dataptr, uint32_t nak_limit = USB_NAK_LIMIT);
		uint32_t getIdle(uint32_t addr, uint32_t ep, uint8_t interface, uint8_t reportID, uint8_t* dataptr, uint32_t nak_limit = USB_NAK_LIMIT);
		uint32_t setIdle(uint32_t addr, uint32_t ep, uint8_t interface, uint8_t reportID, uint8_t duration, uint32_t nak_limit = USB_NAK_LIMIT);

		/* Transfer requests */
		uint32_t ctrlData(uint32_t addr, uint32_t ep, uint32_t nbytes, uint8_t* dataptr, uint32_t direction, uint32_t nak_limit = USB_NAK_LIMIT);
		uint32_t ctrlStatus(uint32_t ep, uint32_t direction, uint32_t nak_limit = USB_NAK_LIMIT);
		uint32_t inTransfer(uint32_t addr, uint32_t ep, uint32_t nbytes, uint8_t* data, uint32_t nak_limit = USB_NAK_LIMIT);
		int32_t newInTransfer(uint32_t addr, uint32_t ep, uint32_t nbytes, uint8_t* data, uint32_t nak_limit  = USB_NAK_LIMIT);
		uint32_t outTransfer(uint32_t addr, uint32_t ep, uint32_t nbytes, uint8_t* data, uint32_t nak_limit = USB_NAK_LIMIT);
		uint32_t dispatchPkt(uint32_t token, uint32_t ep, uint32_t nak_limit = USB_NAK_LIMIT);
		void Task(void);

    private:
        void init();
};

// Get device descriptor
inline uint32_t USBHost::getDevDescr(uint32_t addr, uint32_t ep, uint32_t nbytes, uint8_t* dataptr, uint32_t nak_limit)
{
	return (ctrlReq(addr, ep, bmREQ_GET_DESCR, USB_REQUEST_GET_DESCRIPTOR, 0x00, USB_DESCRIPTOR_DEVICE, 0x0000, nbytes, dataptr, nak_limit));
}

// Get configuration descriptor
inline uint32_t USBHost::getConfDescr(uint32_t addr, uint32_t ep, uint32_t nbytes, uint8_t conf, uint8_t* dataptr, uint32_t nak_limit)
{
	return (ctrlReq(addr, ep, bmREQ_GET_DESCR, USB_REQUEST_GET_DESCRIPTOR, conf, USB_DESCRIPTOR_CONFIGURATION, 0x0000, nbytes, dataptr, nak_limit));
}

// Get string descriptor
inline uint32_t USBHost::getStrDescr(uint32_t addr, uint32_t ep, uint32_t nbytes, uint8_t index, uint16_t langid, uint8_t* dataptr, uint32_t nak_limit)
{
    return (ctrlReq(addr, ep, bmREQ_GET_DESCR, USB_REQUEST_GET_DESCRIPTOR, index, USB_DESCRIPTOR_STRING, langid, nbytes, dataptr, nak_limit));
}

// Set address
inline uint32_t USBHost::setAddr(uint32_t oldaddr, uint32_t ep, uint32_t newaddr, uint32_t nak_limit)
{
    return (ctrlReq(oldaddr, ep, bmREQ_SET, USB_REQUEST_SET_ADDRESS, newaddr, 0x00, 0x0000, 0x0000, 0, nak_limit));
}

// Set configuration
inline uint32_t USBHost::setConf(uint32_t addr, uint32_t ep, uint8_t conf_value, uint32_t nak_limit)
{
    return (ctrlReq(addr, ep, bmREQ_SET, USB_REQUEST_SET_CONFIGURATION, conf_value, 0x00, 0x0000, 0x0000, 0, nak_limit));
}

// Class requests
inline uint32_t USBHost::setProto(uint32_t addr, uint32_t ep, uint8_t interface, uint8_t protocol, uint32_t nak_limit)
{
	return (ctrlReq(addr, ep, bmREQ_HIDOUT, HID_REQUEST_SET_PROTOCOL, protocol, 0x00, interface, 0x0000, 0, nak_limit));
}

inline uint32_t USBHost::getProto(uint32_t addr, uint32_t ep, uint8_t interface, uint8_t* dataptr, uint32_t nak_limit)
{
	return (ctrlReq(addr, ep, bmREQ_HIDIN, HID_REQUEST_GET_PROTOCOL, 0x00, 0x00, interface, 0x0001, dataptr, nak_limit));
}

// Get HID report descriptor
inline uint32_t USBHost::getReportDescr(uint32_t addr, uint32_t ep, uint32_t nbytes, uint8_t* dataptr, uint32_t nak_limit)
{
	return (ctrlReq(addr, ep, bmREQ_HIDREPORT, USB_REQUEST_GET_DESCRIPTOR, 0x00, HID_DESCRIPTOR_REPORT, 0x0000, nbytes, dataptr, nak_limit));
}

inline uint32_t USBHost::setReport(uint32_t addr, uint32_t ep, uint32_t nbytes, uint8_t interface, uint8_t report_type, uint8_t report_id, uint8_t* dataptr, uint32_t nak_limit)
{
    return (ctrlReq(addr, ep, bmREQ_HIDOUT, HID_REQUEST_SET_REPORT, report_id, report_type, interface, nbytes, dataptr, nak_limit));
}

inline uint32_t USBHost::getReport(uint32_t addr, uint32_t ep, uint32_t nbytes, uint8_t interface, uint8_t report_type, uint8_t report_id, uint8_t* dataptr, uint32_t nak_limit)
{
	return (ctrlReq(addr, ep, bmREQ_HIDIN, HID_REQUEST_GET_REPORT, report_id, report_type, interface, nbytes, dataptr, nak_limit));
}

/* returns one uint8_t of data in dataptr */
inline uint32_t USBHost::getIdle(uint32_t addr, uint32_t ep, uint8_t interface, uint8_t reportID, uint8_t* dataptr, uint32_t nak_limit)
{
	return (ctrlReq(addr, ep, bmREQ_HIDIN, HID_REQUEST_GET_IDLE, reportID, 0, interface, 0x0001, dataptr, nak_limit));
}

inline uint32_t USBHost::setIdle(uint32_t addr, uint32_t ep, uint8_t interface, uint8_t reportID, uint8_t duration, uint32_t nak_limit)
{
	return (ctrlReq(addr, ep, bmREQ_HIDOUT, HID_REQUEST_SET_IDLE, reportID, duration, interface, 0x0000, 0, nak_limit));
}

#endif /* USB_H_INCLUDED */
