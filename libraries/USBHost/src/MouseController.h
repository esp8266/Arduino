/*
  Copyright (c) 2012 Arduino.  All right reserved.

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

#ifndef MOUSE_CONTROLLER_H
#define MOUSE_CONTROLLER_H

#include <hidboot.h>

enum MouseButton {
	LEFT_BUTTON   = 0x01,
	MIDDLE_BUTTON = 0x02,
	RIGHT_BUTTON  = 0x04
};

class MouseController : public MouseReportParser
{
public:
  MouseController(USBHost &usb) : hostMouse(&usb), dx(0), dy(0), buttons(0) {
    hostMouse.SetReportParser(0, this);
  };

  bool getButton(MouseButton button) { return (buttons & button) == button; };
  int getXChange();
  int getYChange();
  // int getWheelChange(); // Not implemented

protected:
  virtual void OnMouseMove(MOUSEINFO *mi);
  virtual void OnLeftButtonUp(MOUSEINFO *mi);
  virtual void OnLeftButtonDown(MOUSEINFO *mi);
  virtual void OnMiddleButtonUp(MOUSEINFO *mi);
  virtual void OnMiddleButtonDown(MOUSEINFO *mi);
  virtual void OnRightButtonUp(MOUSEINFO *mi);
  virtual void OnRightButtonDown(MOUSEINFO *mi);

private:
  HIDBoot<HID_PROTOCOL_MOUSE> hostMouse;
  int dx, dy;
  int buttons;
};

#endif
