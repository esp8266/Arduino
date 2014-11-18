package cc.arduino.packages;

import processing.app.AbstractMonitor;
import processing.app.NetworkMonitor;
import processing.app.SerialMonitor;

public class MonitorFactory {

  public AbstractMonitor newMonitor(BoardPort port) {
    if ("network".equals(port.getProtocol())) {
      return new NetworkMonitor(port);
    }

    return new SerialMonitor(port);
  }

}
