package cc.arduino.packages.ssh;

import cc.arduino.packages.BoardPort;
import com.jcraft.jsch.*;

import java.io.File;
import java.io.IOException;

public class SSHConfigFileSetup implements SSHClientSetupChainRing {

  private final SSHClientSetupChainRing nextChainRing;

  public SSHConfigFileSetup(SSHClientSetupChainRing nextChainRing) {
    this.nextChainRing = nextChainRing;
  }

  public Session setup(BoardPort port, JSch jSch) throws JSchException, IOException {
    String ipAddress = port.getAddress();
    String hostname = port.getBoardName().contains(".local") ? port.getBoardName() : port.getBoardName() + ".local";

    File sshFolder = new File(System.getProperty("user.home"), ".ssh");
    File sshConfig = new File(sshFolder, "config");

    if (!sshFolder.exists() || !sshConfig.exists()) {
      if (nextChainRing != null) {
        return nextChainRing.setup(port, jSch);
      }
      throw new JSchException("Unable to find a way to connect");
    }

    OpenSSHConfig configRepository = OpenSSHConfig.parseFile(sshConfig.getAbsolutePath());

    jSch.setConfigRepository(new OpenSSHConfigWrapper(configRepository, ipAddress));

    return jSch.getSession(hostname);
  }

  public static class OpenSSHConfigWrapper implements ConfigRepository {

    private final OpenSSHConfig config;
    private final String ipAddress;

    public OpenSSHConfigWrapper(OpenSSHConfig config, String ipAddress) {
      this.config = config;
      this.ipAddress = ipAddress;
    }

    @Override
    public Config getConfig(String host) {
      return new ConfigWrapper(config.getConfig(host), ipAddress);
    }
  }

  public static class ConfigWrapper implements ConfigRepository.Config {

    private final ConfigRepository.Config config;
    private final String ipAddress;

    public ConfigWrapper(OpenSSHConfig.Config config, String ipAddress) {
      this.config = config;
      this.ipAddress = ipAddress;
    }

    @Override
    public String getHostname() {
      return ipAddress;
    }

    @Override
    public String getUser() {
      String user = config.getUser();
      if (user != null) {
        return user;
      }
      return "root";
    }

    @Override
    public int getPort() {
      return config.getPort();
    }

    @Override
    public String getValue(String key) {
      return config.getValue(key);
    }

    @Override
    public String[] getValues(String key) {
      return config.getValues(key);
    }
  }
}
