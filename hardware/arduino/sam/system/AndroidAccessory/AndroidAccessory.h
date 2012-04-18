/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __AndroidAccessory_h__
#define __AndroidAccessory_h__

#include "Arduino.h"
#include "Stream.h"

#define DATA_BUFFER_SIZE 64

class AndroidAccessory : public Stream {
private:
    const char *manufacturer;
    const char *model;
    const char *description;
    const char *version;
    const char *uri;
    const char *serial;

    MAX3421E max;
    USB usb;
    bool connected;
    uint8_t in;
    uint8_t out;

    EP_RECORD epRecord[8];

    // TODO: Reuse `descBuff` after connection and/or stream descriptor?
    uint8_t descBuff[256];

    byte dataBuff[DATA_BUFFER_SIZE];
    unsigned int numBytesInDataBuff;
    unsigned int nextByteInDataBuffOffset;

    bool isAccessoryDevice(USB_DEVICE_DESCRIPTOR *desc)
    {
        return desc->idVendor == 0x18d1 &&
            (desc->idProduct == 0x2D00 || desc->idProduct == 0x2D01);
    }

    int getProtocol(byte addr);
    void sendString(byte addr, int index, const char *str);
    bool switchDevice(byte addr);
    bool findEndpoints(byte addr, EP_RECORD *inEp, EP_RECORD *outEp);
    bool configureAndroid(void);

    bool dataBufferIsEmpty();
    void refillDataBuffer();

    // Private because it bypasses the data buffer.
    int read(void *buff, int len, unsigned int nakLimit = USB_NAK_LIMIT);

public:
    AndroidAccessory(const char *manufacturer,
                     const char *model,
                     const char *description,
                     const char *version,
                     const char *uri,
                     const char *serial);

    void powerOn(void);

    bool isConnected(void);
    virtual size_t write(uint8_t *buff, size_t len);

    virtual int available(void);
    virtual int peek(void);
    virtual int read(void);

    virtual void flush();
    virtual size_t write(uint8_t);

    using Print::write; // pull in write(str) and write(buf, size) from Print
};

#endif /* __AndroidAccessory_h__ */
