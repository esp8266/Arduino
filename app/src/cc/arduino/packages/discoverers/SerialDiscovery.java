/*
  Copyright (c) 2013 Arduino LLC. All right reserved.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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
