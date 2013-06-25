package cc.arduino.packages.discoverers.network;

import java.net.InetAddress;

public interface NetworkTopologyListener {

  void inetAddressAdded(InetAddress address);

  void inetAddressRemoved(InetAddress address);

}
