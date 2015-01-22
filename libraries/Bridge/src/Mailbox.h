/*
  Copyright (c) 2013 Arduino LLC. All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/

#ifndef _MAILBOX_CLASS_H_INCLUDED_
#define _MAILBOX_CLASS_H_INCLUDED_

#include <Bridge.h>

class MailboxClass {
  public:
    MailboxClass(BridgeClass &b = Bridge) : bridge(b) { }

    void begin() { }
    void end() { }

    // Receive a message and store it inside a buffer
    unsigned int readMessage(uint8_t *buffer, unsigned int size);
    // Receive a message and store it inside a String
    void readMessage(String &str, unsigned int maxLength = 128);

    // Send a message
    void writeMessage(const uint8_t *buffer, unsigned int size);
    // Send a message
    void writeMessage(const String& str);
    // Send a JSON message
    void writeJSON(const String& str);

    // Return the size of the next available message, 0 if there are
    // no messages in queue.
    unsigned int messageAvailable();

  private:
    BridgeClass &bridge;
};

extern MailboxClass Mailbox;

#endif // _MAILBOX_CLASS_H_INCLUDED_
