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

package cc.arduino.packages;

import cc.arduino.packages.discoverers.NetworkDiscovery;
import cc.arduino.packages.discoverers.SerialDiscovery;

import java.util.ArrayList;
import java.util.List;

import static processing.app.I18n._;

public class DiscoveryManager {

  private final List<Discovery> discoverers;

  public DiscoveryManager() {
    discoverers = new ArrayList<Discovery>();
    discoverers.add(new SerialDiscovery());
    discoverers.add(new NetworkDiscovery());

    // Start all discoverers
    for (Discovery d : discoverers) {
      try {
        d.start();
      } catch (Exception e) {
        System.err.println(_("Error starting discovery method: ") + d.getClass());
        e.printStackTrace();
      }
    }

    Thread closeHook = new Thread(new Runnable() {
      @Override
      public void run() {
        for (Discovery d : discoverers) {
          try {
            d.stop();
          } catch (Exception e) {
            e.printStackTrace();  //To change body of catch statement use File | Settings | File Templates.
          }
        }
      }
    });
    Runtime.getRuntime().addShutdownHook(closeHook);
  }

  public List<BoardPort> discovery() {
    List<BoardPort> res = new ArrayList<BoardPort>();
    for (Discovery d : discoverers) {
      res.addAll(d.discovery());
    }
    return res;
  }

  public BoardPort find(String address) {
    for (BoardPort boardPort : discovery()) {
      if (boardPort.getAddress().equals(address)) {
        return boardPort;
      }
    }
    return null;
  }

}
