/* Copyright (C) 2011 Circuits At Home, LTD. All rights reserved.

This software may be distributed and modified under the terms of the GNU
General Public License version 2 (GPL2) as published by the Free Software
Foundation and appearing in the file GPL2.TXT included in the packaging of
this file. Please note that GPL2 Section 2[b] requires that all works based
on this software must also be made publicly available under the terms of
the GPL2 ("Copyleft").

Contact information
-------------------

Circuits At Home, LTD
Web      :  http://www.circuitsathome.com
e-mail   :  support@circuitsathome.com
*/

#ifndef ADDRESS_H_INCLUDED
#define ADDRESS_H_INCLUDED

#include <stdint.h>

/* NAK powers. To save space in endpoint data structure, amount of retries before giving up and returning 0x4 is stored in */
/* bmNakPower as a power of 2. The actual nak_limit is then calculated as nak_limit = ( 2^bmNakPower - 1) */
#define USB_NAK_MAX_POWER		16		//NAK binary order maximum value
#define USB_NAK_DEFAULT			14		//default 16K-1 NAKs before giving up
#define USB_NAK_NOWAIT			1		//Single NAK stops transfer
#define USB_NAK_NONAK			0		//Do not count NAKs, stop retrying after USB Timeout

/**
 * \brief Device endpoint definition.
 *
 * \note hostPipeNum is the allocated pipe used for the communication with
 * deviceEpNum remote device endpoint.
 * There is exactly one hostPipeNum corresponding to a deviceEpNum.
 *
 * \note The number of host pipe is limited by the hardware (10 on SAM3X).
 * Moreover hostPipeNum allocation is static, meaning that only a limited
 * amount of device endpoints can be opened at the same time, thus limitating
 * the maximum number of connected devices at the same time.
 */
struct EpInfo
{
	uint32_t		deviceEpNum;					// Device endpoint number
	uint32_t		hostPipeNum;					// Host corresponding pipe number
	uint32_t		maxPktSize;						// Maximum packet size

	union
	{
		uint8_t		epAttribs;

		struct
		{
			uint8_t		bmSndToggle	:	1;		// Send toggle, when zero bmSNDTOG0, bmSNDTOG1 otherwise
			uint8_t		bmRcvToggle	:	1;		// Send toggle, when zero bmRCVTOG0, bmRCVTOG1 otherwise
			uint8_t		bmNakPower	:	6;		// Binary order for NAK_LIMIT value
		};
	};
};

/**
 * \brief USB device address definition.
 *
 * \note The 8 bits USB address is defined like this:
 *
 *	  7   6   5   4   3   2   1   0
 *  ---------------------------------
 *  |   | H | P | P | P | A | A | A |
 *  ---------------------------------
 *
 * H - if 1 the address is a hub address
 * P - parent hub address
 * A - device address / port number in case of hub
 */
struct UsbDeviceAddress
{
	union
	{
		struct
		{
			uint32_t		bmAddress	: 3;	// device address/port number
			uint32_t		bmParent	: 3;	// parent hub address
			uint32_t		bmHub		: 1;	// hub flag
			uint32_t		bmReserved	: 25;	// reserved, must be zerro
		};
		uint32_t	devAddress;
	};
};

#define bmUSB_DEV_ADDR_ADDRESS		0x07
#define bmUSB_DEV_ADDR_PARENT		0x38
#define bmUSB_DEV_ADDR_HUB			0x40

/**
 * \brief USB device definition.
 *
 * \note epinfo is used to store the list of device endpoints currently used
 * by the USBHost stack. This field is set during enumeration process when
 * a supported USB class is found. See any USB classes implementing
 * USBDeviceConfig and init() method for reference.
 */
struct UsbDevice
{
	EpInfo			*epinfo;		// endpoint info pointer
	uint32_t		address;		// address
	uint32_t		epcount;		// number of endpoints
	uint32_t		lowspeed;		// indicates if a device is the low speed one
};

/**
 * \class Abstract AddressPool definition.
 */
class AddressPool
{
public:
	virtual UsbDevice* GetUsbDevicePtr(uint32_t addr) = 0;
	virtual uint32_t AllocAddress(uint32_t parent, uint32_t is_hub = 0, uint32_t port = 0) = 0;
	virtual void FreeAddress(uint32_t addr) = 0;
};

typedef void (*UsbDeviceHandleFunc)(UsbDevice *pdev);

#define ADDR_ERROR_INVALID_INDEX		0xFF
#define ADDR_ERROR_INVALID_ADDRESS		0xFF

/**
 * \class AddressPoolImpl definition.
 * Used to store the list of connected devices and to keep track of free USB
 * addresses.
 */
template <const uint32_t MAX_DEVICES_ALLOWED>
class AddressPoolImpl : public AddressPool
{
private:

	EpInfo dev0ep;						// Endpoint data structure used during enumeration for uninitialized device

	uint32_t hubCounter;				// hub counter is kept
										// in order to avoid hub address duplication

	UsbDevice thePool[MAX_DEVICES_ALLOWED];

	/**
	 * \brief Initialize the specified address pool entry.
	 *
	 * \param index Index pointing to a UsbDevice instance in the address pool.
	 */
	void InitEntry(uint32_t index)
	{
		thePool[index].address = 0;
		thePool[index].epcount = 1;
		thePool[index].lowspeed = 0;
		thePool[index].epinfo = &dev0ep;
	};

	/**
	 * \brief Return an address pool index for a given  address. This index can
	 * further be used to retrieve the corresponding USB device instance
	 * UsbDevice.
	 *
	 * \param index Index pointing to a UsbDevice instance in the address pool.
	 *
	 * \return Index number if found, 0 otherwise.
	 * \note Index 0 is reserved for address 0 and shall never be used.
	 */
	uint32_t FindAddressIndex(uint32_t address = 0)
	{
		for (uint8_t i = 1; i < MAX_DEVICES_ALLOWED; i++)
		{
			if (thePool[i].address == address)
				return i;
		}
		return 0;
	};

	/**
	 * \brief Return address pool child index for a given parent. This index can
	 * further be used to retrieve the corresponding USB device instance
	 * UsbDevice.
	 *
	 * \param addr Parent USB address.
	 * \param start Search in the pool from this index. Calling multiple time
	 * this function with the returned index + 1 can be used to walk through
	 * all children.
	 *
	 * \return Child index number if found, 0 otherwise.
	 * \note Index 0 is reserved for address 0 and shall never be used.
	 */
	uint32_t FindChildIndex(UsbDeviceAddress addr, uint32_t start = 1)
	{
		for (uint32_t i = (start < 1 || start >= MAX_DEVICES_ALLOWED) ? 1 : start; i < MAX_DEVICES_ALLOWED; ++i)
		{
			if (((UsbDeviceAddress*)&thePool[i].address)->bmParent == addr.bmAddress)
				return i;
		}

		return 0;
	};

	/**
	 * \brief Free address entry specified by index parameter.
	 *
	 * \note Calling FreeAddressByIndex only frees the USB address for possible
	 * further assignement. However, it does not free the actual USB resources
	 * used by the device. This can be made by calling the release() method
	 * from any UsbDevice class implementing USBDeviceConfig.
	 *
	 * \param index Index pointing to a UsbDevice instance in the address pool.
	 *
	 * \note Calling FreeAddressByIndex with a 0 index param has no effect.
	 */
	void FreeAddressByIndex(uint32_t index)
	{
		// Zero field is reserved and should not be affected
		if (index == 0)
			return;

		// If a hub was switched off all port addresses should be freed
		if (((UsbDeviceAddress*)&thePool[index].address)->bmHub == 1)
		{
			for (uint32_t i = 1; (i = FindChildIndex(*((UsbDeviceAddress*)&thePool[index].address), i) > 0); )
				FreeAddressByIndex(i);

			// If the hub had the last allocated address, hubCounter should be decremented
			if (hubCounter == ((UsbDeviceAddress*)&thePool[index].address)->bmAddress)
				hubCounter --;
		}

		InitEntry(index);
	}

	/**
	 * \brief Initialize all address poll entries at once.
	 */
	void InitAllAddresses()
	{
		for (uint32_t i = 1; i < MAX_DEVICES_ALLOWED; i++)
			InitEntry(i);

		hubCounter = 0;
	};

public:
	/**
	 * \brief AddressPoolImpl class constructor.
	 */
	AddressPoolImpl() : hubCounter(0)
	{
		// Init address zero (reserved)
		InitEntry(0);

		// Init all remaing addresses
		InitAllAddresses();

		// Configure ep0 used for enumeration
		dev0ep.deviceEpNum = 0;
		dev0ep.hostPipeNum = 0;
		dev0ep.maxPktSize = 8;
	};

	/**
	 * \brief Get a UsbDevice pointer from a USB device address.
	 *
	 * \param addr USB device address.
	 *
	 * \return UsbDevice pointer on success, 0 otherwise.
	 */
	virtual UsbDevice* GetUsbDevicePtr(uint32_t addr)
	{
		if (!addr)
			return thePool;

		uint32_t index = FindAddressIndex(addr);

		return (!index) ? 0 : (thePool + index);
	};

	/**
	 * \brief Perform an operation specified by pfunc for each addressed
	 * USB device.
	 *
	 * \param pfunc Any function pointer with type UsbDeviceHandleFunc.
	 */
	void ForEachUsbDevice(UsbDeviceHandleFunc pfunc)
	{
		if (!pfunc)
			return;

		for (uint32_t i = 1; i < MAX_DEVICES_ALLOWED; i++)
			if (thePool[i].address)
				pfunc(thePool + i);
	};

	/**
	 * \brief Allocate a new USB device address.
	 *
	 * \note See UsbDeviceAddress definition for better understanding.
	 *
	 * \param parent USB device address of the Parent device.
	 * \param is_hub Set to true if the corresponding device is a Hub, false
	 * otherwise.
	 * \param port USB device base address.
	 *
	 * \return UsbDevice pointer on success, 0 otherwise.
	 */
	virtual uint32_t AllocAddress(uint32_t parent, uint32_t is_hub = 0, uint32_t port = 0)
	{
		if (parent > 127 || port > 7)
			return 0;

		if (is_hub && hubCounter == 7)
			return 0;

		// Finds first empty address entry starting from one
		uint32_t index = FindAddressIndex(0);

		// If empty entry is not found
		if (!index)
			return 0;

		if (parent == 0)
		{
			if (is_hub)
			{
				thePool[index].address = 0x41;
				hubCounter++;
			}
			else
				thePool[index].address = 1;

			return thePool[index].address;
		}

		UsbDeviceAddress addr;

		addr.bmParent = ((UsbDeviceAddress*)&parent)->bmAddress;

		if (is_hub)
		{
			addr.bmHub		= 1;
			addr.bmAddress	= hubCounter++;
		}
		else
		{
			addr.bmHub		= 0;
			addr.bmAddress	= port;
		}

		thePool[index].address = *((uint8_t*)&addr);

		return thePool[index].address;
	};

	/**
	 * \brief Free the specified USB device address.
	 *
	 * \param addr USB device address to free.
	 */
	virtual void FreeAddress(uint32_t addr)
	{
		// If the root hub is disconnected all the addresses should be initialized
		if (addr == 0x41)
		{
			InitAllAddresses();
			return;
		}

		uint32_t index = FindAddressIndex(addr);
		FreeAddressByIndex(index);
	};

	// Returns number of hubs attached
	// It can be rather helpfull to find out if there are hubs attached than getting the exact number of hubs.
	/*uint32_t GetNumHubs()
	{
		return hubCounter;
	};

	uint32_t GetNumDevices()
	{
		uint32_t counter = 0;

		for (uint32_t i = 1; i < MAX_DEVICES_ALLOWED; ++i)
			if (thePool[i].address != 0);
				counter++;

		return counter;
	};*/

};

#endif /* ADDRESS_H_INCLUDED */
