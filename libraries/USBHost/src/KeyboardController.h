/*
 Copyright (c) 2012 Arduino LLC.  All right reserved.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 See the GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef KEYBOARD_CONTROLLER_H
#define KEYBOARD_CONTROLLER_H

#include <hidboot.h>

enum KeyboardModifiers {
	LeftCtrl = 1,
	LeftShift = 2,
	Alt = 4,
	LeftCmd = 8,
	RightCtrl = 16,
	RightShift = 32,
	AltGr = 64,
	RightCmd = 128
};

class KeyboardController : public KeyboardReportParser {
public:
  KeyboardController(USBHost &usb) : hostKeyboard(&usb), key(0), keyOem(0), modifiers(0) {
    hostKeyboard.SetReportParser(0, this);
  };

  uint8_t getKey()       { return key; };
  uint8_t getModifiers() { return modifiers; };
  uint8_t getOemKey()    { return keyOem; };

protected:
  virtual void OnKeyDown(uint8_t mod, uint8_t key);
  virtual void OnKeyUp(uint8_t mod, uint8_t key);

private:
  HIDBoot<HID_PROTOCOL_KEYBOARD> hostKeyboard;
  uint8_t key, keyOem, modifiers;
};

#endif
