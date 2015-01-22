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

package cc.arduino.packages.discoverers.network;

import javax.jmdns.NetworkTopologyDiscovery;
import java.net.InetAddress;
import java.util.*;

public class NetworkChecker extends TimerTask {

  private final NetworkTopologyListener topologyListener;
  private final NetworkTopologyDiscovery topology;

  private Set<InetAddress> knownAddresses;

  public NetworkChecker(NetworkTopologyListener topologyListener, NetworkTopologyDiscovery topology) {
    super();
    this.topologyListener = topologyListener;
    this.topology = topology;
    this.knownAddresses = Collections.synchronizedSet(new HashSet<InetAddress>());
  }

  public void start(Timer timer) {
    timer.schedule(this, 0, 3000);
  }

  @Override
  public void run() {
    try {
      InetAddress[] curentAddresses = topology.getInetAddresses();
      Set<InetAddress> current = new HashSet<InetAddress>(curentAddresses.length);
      for (InetAddress address : curentAddresses) {
        current.add(address);
        if (!knownAddresses.contains(address)) {
          topologyListener.inetAddressAdded(address);
        }
      }
      for (InetAddress address : knownAddresses) {
        if (!current.contains(address)) {
          topologyListener.inetAddressRemoved(address);
        }
      }
      knownAddresses = current;
    } catch (Exception e) {
      e.printStackTrace();
    }
  }
}
