/*
 * This file is part of Arduino.
 *
 * Arduino is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * As a special exception, you may use this file as part of a free software
 * library without restriction.  Specifically, if other files instantiate
 * templates or use macros or inline functions from this file, or you compile
 * this file and link it with other files to produce an executable, this
 * file does not by itself cause the resulting executable to be covered by
 * the GNU General Public License.  This exception does not however
 * invalidate any other reasons why the executable file might be covered by
 * the GNU General Public License.
 *
 * Copyright 2013 Arduino LLC (http://www.arduino.cc/)
 */

package cc.arduino.packages.discoverers;

import gnu.io.CommPortIdentifier;

import java.util.ArrayList;
import java.util.Enumeration;
import java.util.List;

import processing.app.Base;
import processing.app.Platform;
import processing.app.helpers.PreferencesMap;
import cc.arduino.packages.BoardPort;
import cc.arduino.packages.Discovery;

public class SerialDiscovery implements Discovery {

  @Override
  public List<BoardPort> discovery() {
    Platform os = Base.getPlatform();
    String devicesListOutput = os.preListAllCandidateDevices();

    List<BoardPort> res = new ArrayList<BoardPort>();
    @SuppressWarnings("unchecked")
    Enumeration<CommPortIdentifier> ports = CommPortIdentifier
        .getPortIdentifiers();
    while (ports.hasMoreElements()) {
      CommPortIdentifier commPort = ports.nextElement();
      if (commPort.getPortType() != CommPortIdentifier.PORT_SERIAL)
        continue;

      String address = commPort.getName();
      String boardName = os.resolveDeviceAttachedTo(address, Base.packages,
                                                    devicesListOutput);
      String label = address;
      if (boardName != null)
        label += " (" + boardName + ")";

      BoardPort port = new BoardPort();
      port.setAddress(address);
      port.setProtocol("serial");
      port.setBoardName(boardName);
      port.setLabel(label);
      res.add(port);
    }
    return res;
  }

  @Override
  public void setPreferences(PreferencesMap options) {
  }

  @Override
  public void start() {
  }

  @Override
  public void stop() {
  }

}
