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
