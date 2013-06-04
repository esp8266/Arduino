package cc.arduino.packages.discoverers;

import java.io.IOException;
import java.net.InetAddress;
import java.util.List;
import java.util.concurrent.CopyOnWriteArrayList;

import javax.jmdns.JmDNS;
import javax.jmdns.NetworkTopologyDiscovery;
import javax.jmdns.ServiceEvent;
import javax.jmdns.ServiceInfo;
import javax.jmdns.ServiceListener;
import javax.jmdns.impl.DNSTaskStarter;

import processing.app.Base;
import processing.app.helpers.PreferencesMap;
import processing.app.zeroconf.jmdns.ArduinoDNSTaskStarter;
import cc.arduino.packages.BoardPort;
import cc.arduino.packages.Discovery;

public class NetworkDiscovery implements Discovery, ServiceListener {

  private JmDNS jmDNS;
  private List<BoardPort> ports = new CopyOnWriteArrayList<BoardPort>();

  @Override
  public List<BoardPort> discovery() {
    return ports;
  }

  @Override
  public void setPreferences(PreferencesMap options) {
  }

  @Override
  public void start() throws IOException {
    DNSTaskStarter.Factory.setClassDelegate(new ArduinoDNSTaskStarter());
    for (InetAddress addr : NetworkTopologyDiscovery.Factory.getInstance()
        .getInetAddresses()) {
      jmDNS = JmDNS.create(addr);
      jmDNS.addServiceListener("_arduino._tcp.local.", this);
    }
  }

  @Override
  public void stop() {
    // Removed cleanup: is extremely slow on closing

    // try {
    // jmDNS.close();
    // } catch (IOException e) {
    // e.printStackTrace();
    // }
  }

  @Override
  public void serviceAdded(ServiceEvent serviceEvent) {
    String type = serviceEvent.getType();
    String name = serviceEvent.getName();

    JmDNS dns = serviceEvent.getDNS();

    dns.requestServiceInfo(type, name);
    ServiceInfo serviceInfo = dns.getServiceInfo(type, name);
    if (serviceInfo != null)
      dns.requestServiceInfo(type, name);
  }

  @Override
  public void serviceRemoved(ServiceEvent serviceEvent) {
    String name = serviceEvent.getName();
    for (BoardPort port : ports) {
      if (port.getBoardName().equals(name))
        ports.remove(port);
    }
  }

  @Override
  public void serviceResolved(ServiceEvent serviceEvent) {
    ServiceInfo info = serviceEvent.getInfo();
    String address = serviceEvent.getInfo().getInet4Addresses()[0]
        .getHostAddress();
    String name = serviceEvent.getName();

    PreferencesMap prefs = null;
    if (info.hasData()) {
      prefs = new PreferencesMap();
      prefs.put("id", info.getPropertyString("board"));
      prefs.put("distro_version", info.getPropertyString("distro_version"));
    }
    String boardName = Base.getPlatform()
        .resolveDeviceByBoardID(Base.packages, prefs.get("id"));

    String label = name + " at " + address + " (" + boardName + ")";

    BoardPort port = new BoardPort();
    port.setAddress(address);
    port.setBoardName(name);
    port.setProtocol("network");
    port.setPrefs(prefs);
    port.setLabel(label);
    ports.add(port);
  }

}
