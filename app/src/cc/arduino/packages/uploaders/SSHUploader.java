/*
 * This file is part of Arduino.
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
 *
 * Copyright 2013 Arduino LLC (http://www.arduino.cc/)
 */

package cc.arduino.packages.uploaders;

import cc.arduino.packages.BoardPort;
import cc.arduino.packages.Uploader;
import cc.arduino.packages.ssh.*;
import com.jcraft.jsch.JSch;
import com.jcraft.jsch.JSchException;
import com.jcraft.jsch.Session;
import processing.app.BaseNoGui;
import processing.app.I18n;
import processing.app.PreferencesData;
import processing.app.debug.RunnerException;
import processing.app.debug.TargetPlatform;
import processing.app.helpers.PreferencesMap;
import processing.app.helpers.StringUtils;

import java.io.File;
import java.io.IOException;
import java.util.Arrays;
import java.util.List;

import static processing.app.I18n._;

public class SSHUploader extends Uploader {

  private static final List<String> FILES_NOT_TO_COPY = Arrays.asList(".DS_Store", ".Trash", "Thumbs.db", "__MACOSX");

  private final BoardPort port;

  public SSHUploader(BoardPort port) {
    this.port = port;
  }

  public boolean requiresAuthorization() {
    return true;
  }

  @Override
  public String getAuthorizationKey() {
    return "runtime.pwd." + port.getAddress();
  }

  @Override
  public boolean uploadUsingPreferences(File sourcePath, String buildPath, String className, boolean usingProgrammer, List<String> warningsAccumulator) throws RunnerException {
    if (usingProgrammer) {
      throw new RunnerException(_("Network upload using programmer not supported"));
    }

    Session session = null;
    SCP scp = null;
    try {
      JSch jSch = new JSch();
      SSHClientSetupChainRing sshClientSetupChain = new SSHConfigFileSetup(new SSHPwdSetup());
      session = sshClientSetupChain.setup(port, jSch);

      session.setUserInfo(new NoInteractionUserInfo(PreferencesData.get("runtime.pwd." + port.getAddress())));
      session.connect(30000);

      scp = new SCP(session);
      SSH ssh = new SSH(session);

      scpFiles(scp, ssh, sourcePath, buildPath, className, warningsAccumulator);

      return runAVRDude(ssh);
    } catch (JSchException e) {
      String message = e.getMessage();
      if ("Auth cancel".equals(message) || "Auth fail".equals(message)) {
        return false;
      }
      if (e.getMessage().contains("Connection refused")) {
        throw new RunnerException(I18n.format("Unable to connect to {0}", port.getAddress()));
      }
      throw new RunnerException(e);
    } catch (Exception e) {
      throw new RunnerException(e);
    } finally {
      if (scp != null) {
        try {
          scp.close();
        } catch (IOException e) {
          throw new RunnerException(e);
        }
      }
      if (session != null) {
        session.disconnect();
      }
    }
  }

  private boolean runAVRDude(SSH ssh) throws IOException, JSchException {
    TargetPlatform targetPlatform = BaseNoGui.getTargetPlatform();
    PreferencesMap prefs = PreferencesData.getMap();
    prefs.putAll(BaseNoGui.getBoardPreferences());
    prefs.putAll(targetPlatform.getTool(prefs.get("upload.tool")));

    String additionalParams = verbose ? prefs.get("upload.params.verbose") : prefs.get("upload.params.quiet");

    boolean success = ssh.execSyncCommand("merge-sketch-with-bootloader.lua /tmp/sketch.hex", System.out, System.err);
    ssh.execSyncCommand("kill-bridge");
    success = success && ssh.execSyncCommand("run-avrdude /tmp/sketch.hex '" + additionalParams + "'", System.out, System.err);
    return success;
  }

  private void scpFiles(SCP scp, SSH ssh, File sourcePath, String buildPath, String className, List<String> warningsAccumulator) throws JSchException, IOException {
    try {
      scp.open();
      scp.startFolder("tmp");
      scp.sendFile(new File(buildPath, className + ".hex"), "sketch.hex");
      scp.endFolder();

      if (canUploadWWWFiles(sourcePath, ssh, warningsAccumulator)) {
        scp.startFolder("www");
        scp.startFolder("sd");
        scp.startFolder(sourcePath.getName());
        recursiveSCP(new File(sourcePath, "www"), scp);
        scp.endFolder();
        scp.endFolder();
        scp.endFolder();
      }
    } finally {
      scp.close();
    }
  }

  private boolean canUploadWWWFiles(File sourcePath, SSH ssh, List<String> warningsAccumulator) throws IOException, JSchException {
    File www = new File(sourcePath, "www");
    if (!www.exists() || !www.isDirectory()) {
      return false;
    }
    if (!www.canExecute()) {
      warningsAccumulator.add(_("Problem accessing files in folder ") + www);
      return false;
    }
    if (!ssh.execSyncCommand("special-storage-available")) {
      warningsAccumulator.add(_("Problem accessing board folder /www/sd"));
      return false;
    }
    return true;
  }

  private void recursiveSCP(File from, SCP scp) throws IOException {
    File[] files = from.listFiles();
    if (files == null) {
      return;
    }

    for (File file : files) {
      if (!StringUtils.stringContainsOneOf(file.getName(), FILES_NOT_TO_COPY)) {
        if (file.isDirectory() && file.canExecute()) {
          scp.startFolder(file.getName());
          recursiveSCP(file, scp);
          scp.endFolder();
        } else if (file.isFile() && file.canRead()) {
          scp.sendFile(file);
        }
      }
    }
  }

  @Override
  public boolean burnBootloader() throws RunnerException {
    throw new RunnerException("Can't burn bootloader via SSH");
  }

}
