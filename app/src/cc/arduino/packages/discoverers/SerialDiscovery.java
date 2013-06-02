package cc.arduino.packages.discoverers;

import gnu.io.CommPortIdentifier;

import java.util.ArrayList;
import java.util.Enumeration;
import java.util.List;

import processing.app.Base;
import processing.app.Platform;
import processing.app.helpers.PreferencesMap;
import cc.arduino.packages.BoardPort;
import cc.arduino.packages.Discovery;

public class SerialDiscovery implements Discovery {

  @Override
  public List<BoardPort> discovery() {
    Platform os = Base.getPlatform();
    String devicesListOutput = os.preListAllCandidateDevices();

    List<BoardPort> res = new ArrayList<BoardPort>();
    @SuppressWarnings("unchecked")
    Enumeration<CommPortIdentifier> ports = CommPortIdentifier
        .getPortIdentifiers();
    while (ports.hasMoreElements()) {
      CommPortIdentifier commPort = ports.nextElement();
      if (commPort.getPortType() != CommPortIdentifier.PORT_SERIAL)
        continue;

      String address = commPort.getName();
      String boardName = os.resolveDeviceAttachedTo(address, Base.packages,
                                                    devicesListOutput);
      BoardPort port = new BoardPort();
      port.setAddress(address);
      port.setProtocol("serial");
      port.setBoardName(boardName);
      res.add(port);
    }
    return res;
  }

  @Override
  public void setPreferences(PreferencesMap options) {
  }

  @Override
  public void start() {
  }

  @Override
  public void stop() {
  }

}
