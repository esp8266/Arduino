package cc.arduino.packages;

import static processing.app.I18n._;

import java.util.ArrayList;
import java.util.List;

import cc.arduino.packages.discoverers.NetworkDiscovery;
import cc.arduino.packages.discoverers.SerialDiscovery;

public class DiscoveryManager {

  private List<Discovery> discoverers = new ArrayList<Discovery>();

  public DiscoveryManager() {
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
    
    Runtime.getRuntime().addShutdownHook(closeHook);
  }

  public List<BoardPort> discovery() {
    List<BoardPort> res = new ArrayList<BoardPort>();
    for (Discovery d : discoverers)
      res.addAll(d.discovery());
    return res;
  }

  private Thread closeHook = new Thread(new Runnable() {
    @Override
    public void run() {
      for (Discovery d : discoverers)
        d.stop();
    }
  });
}
