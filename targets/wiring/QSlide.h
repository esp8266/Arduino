/*
  QSlide.h - QProx 401/501 library for Arduino & Wiring
  Copyright (c) 2006 Nicholas Zambetti & Massimo Banzi.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef QSlide_h
#define QSlide_h

#include "WProgram.h"

class QSlide
{
  private:
    byte _drd;
    byte _din;
    byte _ss;
    byte _clk;
    byte _dout;
    byte _det;
    byte _prx;
    byte _prevResult;
    void calibrate(void);
    void waitForReady(void);
    byte transfer(byte);
    byte driftCompensate(void);
  public:
    QSlide(byte, byte, byte, byte, byte, byte, byte);
    void setProximityThreshold(byte);
    void setTouchThreshold(byte);
    byte isTouching(void);
    byte isNear(void);
    byte read(void);
};

#endif

