package cc.arduino.packages.ssh;

import cc.arduino.packages.BoardPort;
import com.jcraft.jsch.JSch;
import com.jcraft.jsch.JSchException;
import com.jcraft.jsch.Session;
import processing.app.Preferences;

public class SSHPwdSetup implements SSHClientSetupChainRing {

  @Override
  public Session setup(BoardPort port, JSch jSch) throws JSchException {
    String ipAddress = port.getAddress();

    Session session = jSch.getSession("root", ipAddress, 22);
    session.setPassword(Preferences.get("runtime.pwd." + ipAddress));

    return session;
  }

}
