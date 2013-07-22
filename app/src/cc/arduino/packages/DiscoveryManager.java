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
    for (Discovery d : discoverers)
      res.addAll(d.discovery());
    return res;
  }

}
