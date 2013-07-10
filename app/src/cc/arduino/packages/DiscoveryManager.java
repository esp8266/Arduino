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
