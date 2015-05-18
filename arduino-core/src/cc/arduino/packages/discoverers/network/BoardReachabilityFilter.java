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

package cc.arduino.packages.discoverers.network;

import cc.arduino.packages.BoardPort;
import cc.arduino.packages.discoverers.NetworkDiscovery;
import processing.app.helpers.NetUtils;

import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.*;

public class BoardReachabilityFilter extends TimerTask {

  private final NetworkDiscovery networkDiscovery;

  public BoardReachabilityFilter(NetworkDiscovery networkDiscovery) {
    this.networkDiscovery = networkDiscovery;
  }

  public void start(Timer timer) {
    timer.schedule(this, 0, 3000);
  }

  @Override
  public void run() {
    List<BoardPort> boardPorts = networkDiscovery.getBoardPortsDiscoveredWithJmDNS();

    Iterator<BoardPort> boardPortIterator = boardPorts.iterator();
    while (boardPortIterator.hasNext()) {
      try {
        BoardPort board = boardPortIterator.next();

        InetAddress inetAddress = InetAddress.getByName(board.getAddress());
        int broadcastedPort = Integer.valueOf(board.getPrefs().get("port"));

        List<Integer> ports = new LinkedList<Integer>();
        ports.add(broadcastedPort);

        //dirty code: allows non up to date yuns to be discovered. Newer yuns will broadcast port 22
        if (broadcastedPort == 80) {
          ports.add(0, 22);
        }

        boolean reachable = NetUtils.isReachable(inetAddress, ports);
        if (!reachable) {
          boardPortIterator.remove();
        }
      } catch (UnknownHostException e) {
        boardPortIterator.remove();
      }
    }

    networkDiscovery.setReachableBoardPorts(boardPorts);
  }
}
