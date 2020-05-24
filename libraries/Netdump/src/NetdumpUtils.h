/*
 * NetdumpUtils.h
 *
 *  Created on: 5 mei 2020
 *      Author: Herman
 */

#ifndef LIBRARIES_NETDUMP_SRC_NETDUMPUTILS_H_
#define LIBRARIES_NETDUMP_SRC_NETDUMPUTILS_H_

#include "Arduino.h"

class NetdumpUtils
{
public:
	NetdumpUtils();
	~NetdumpUtils();
    static uint16_t ntoh16(const uint8_t data[2])
    {
        return data[1] | (((uint16_t)data[0]) << 8);
    };
    static uint32_t ntoh32(const uint8_t data[4])
    {
        return ntoh16(&data[2]) | (((uint32_t)ntoh16(&data[0])) << 16);
    };
};

#endif /* LIBRARIES_NETDUMP_SRC_NETDUMPUTILS_H_ */
