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
 * Copyright 2015 Arduino LLC (http://www.arduino.cc/)
 */

package cc.arduino.packages.discoverers.serial;

import cc.arduino.packages.BoardPort;
import cc.arduino.packages.discoverers.SerialDiscovery;
import processing.app.BaseNoGui;
import processing.app.Platform;
import processing.app.Serial;
import processing.app.debug.TargetBoard;
import processing.app.helpers.PreferencesMap;

import java.util.*;

public class SerialBoardsLister extends TimerTask {

  private final SerialDiscovery serialDiscovery;

  public SerialBoardsLister(SerialDiscovery serialDiscovery) {
    this.serialDiscovery = serialDiscovery;
  }

  public void start(Timer timer) {
    timer.schedule(this, 0, 3000);
  }

  @Override
  public void run() {
    while (BaseNoGui.packages == null) {
      try {
        Thread.sleep(1000);
      } catch (InterruptedException e) {
        // noop
      }
    }

    Platform platform = BaseNoGui.getPlatform();
    if (platform == null) {
      return;
    }

    List<BoardPort> boardPorts = new LinkedList<BoardPort>();

    List<String> ports = Serial.list();

    String devicesListOutput = null;
    if (!ports.isEmpty()) {
      devicesListOutput = platform.preListAllCandidateDevices();
    }

    for (String port : ports) {
      Map<String, Object> boardData = platform.resolveDeviceAttachedTo(port, BaseNoGui.packages, devicesListOutput);

      BoardPort boardPort = new BoardPort();
      boardPort.setAddress(port);
      boardPort.setProtocol("serial");

      String label = port;

      PreferencesMap prefs = new PreferencesMap();

      if (boardData != null) {
        prefs.put("vid", boardData.get("vid").toString());
        prefs.put("pid", boardData.get("pid").toString());

        TargetBoard board = (TargetBoard) boardData.get("board");
        if (board != null) {
          String boardName = board.getName();
          if (boardName != null) {
            label += " (" + boardName + ")";
          }
          boardPort.setBoardName(boardName);
        }
      }

      boardPort.setLabel(label);
      boardPort.setPrefs(prefs);

      boardPorts.add(boardPort);
    }

    serialDiscovery.setSerialBoardPorts(boardPorts);
  }
}
