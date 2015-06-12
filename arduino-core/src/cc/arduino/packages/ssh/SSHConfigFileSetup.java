/*
 * This file is part of Arduino.
 *
 * Copyright 2015 Arduino LLC (http://www.arduino.cc/)
 *
 * Arduino is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * As a special exception, you may use this file as part of a free software
 * library without restriction.  Specifically, if other files instantiate
 * templates or use macros or inline functions from this file, or you compile
 * this file and link it with other files to produce an executable, this
 * file does not by itself cause the resulting executable to be covered by
 * the GNU General Public License.  This exception does not however
 * invalidate any other reasons why the executable file might be covered by
 * the GNU General Public License.
 */

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
