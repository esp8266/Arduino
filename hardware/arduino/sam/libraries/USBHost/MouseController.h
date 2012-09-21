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
	LEFT_BUTTON,
	MIDDLE_BUTTON,
	RIGHT_BUTTON
};

extern int mouseX;
extern int mouseY;
extern MouseButton mouseButton;
extern bool mouseButtonPressed;

class MouseController : public MouseReportParser
{
public:
  MouseController(USBHost &usb) : hostMouse(&usb), maxX(640), maxY(480), buttons(0) {
    hostMouse.SetReportParser(0, this);
  };

  void setMaxX(int mx) { maxX = mx; };
  void setMaxY(int my) { maxY = my; };

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
  int maxX;
  int maxY;
  int buttons;
};

#endif
