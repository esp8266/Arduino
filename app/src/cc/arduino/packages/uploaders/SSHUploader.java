package cc.arduino.packages.uploaders;

import cc.arduino.packages.Uploader;
import cc.arduino.packages.uploaders.ssh.SCP;
import cc.arduino.packages.uploaders.ssh.SSH;
import com.jcraft.jsch.JSch;
import com.jcraft.jsch.JSchException;
import com.jcraft.jsch.Session;
import processing.app.Base;
import processing.app.Constants;
import processing.app.NetworkMonitor;
import processing.app.Preferences;
import processing.app.debug.RunnerException;
import processing.app.debug.TargetPlatform;
import processing.app.helpers.PreferencesMap;

import java.io.File;
import java.io.IOException;
import java.util.regex.Matcher;

import static processing.app.I18n._;

public class SSHUploader extends Uploader {

  private final String ipAddress;

  public SSHUploader(String port) {
    Matcher matcher = Constants.IPV4_ADDRESS.matcher(port);
    if (!matcher.find()) {
      throw new IllegalArgumentException(port);
    }
    ipAddress = matcher.group();
  }

  public boolean requiresAuthorization() {
    return true;
  }

  public String getAuthorizationKey() {
    return "runtime.pwd." + ipAddress;
  }

  @Override
  public boolean uploadUsingPreferences(File sourcePath, String buildPath, String className, boolean usingProgrammer) throws RunnerException {
    if (usingProgrammer) {
      System.err.println(_("Http upload using programmer not supported"));
      return false;
    }

    Session session = null;
    SCP scp = null;
    try {
      JSch jSch = new JSch();
      session = jSch.getSession("root", ipAddress, 22);
      session.setPassword(Preferences.get(getAuthorizationKey()));

      session.setUserInfo(new NetworkMonitor.NoInteractionUserInfo());
      session.connect(30000);

      scp = new SCP(session);
      SSH ssh = new SSH(session);

      scpFiles(scp, ssh, sourcePath, buildPath, className, session);

      return runAVRDude(ssh);
    } catch (JSchException e) {
      if ("Auth cancel".equals(e.getMessage())) {
        return false;
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
    TargetPlatform targetPlatform = Base.getTargetPlatform();
    PreferencesMap prefs = Preferences.getMap();
    prefs.putAll(Base.getBoardPreferences());
    prefs.putAll(targetPlatform.getTool(prefs.get("upload.tool")));

    String additionalParams = verbose ? prefs.get("upload.params.verbose") : prefs.get("upload.params.quiet");

    boolean success = ssh.execSyncCommand("merge-sketch-with-bootloader /tmp/sketch.hex", System.out, System.err);
    ssh.execSyncCommand("kill-bridge");
    success = success && ssh.execSyncCommand("run-avrdude /tmp/sketch.hex '" + additionalParams + "'", System.out, System.err);
    return success;
  }

  private void scpFiles(SCP scp, SSH ssh, File sourcePath, String buildPath, String className, Session session) throws JSchException, IOException {
    try {
      scp.open();
      scp.startFolder("tmp");
      scp.sendFile(new File(buildPath, className + ".hex"), "sketch.hex");
      scp.endFolder();

      if (canUploadWWWFiles(sourcePath, ssh)) {
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

  private boolean canUploadWWWFiles(File sourcePath, SSH ssh) throws IOException, JSchException {
    File www = new File(sourcePath, "www");
    if (!www.exists() || !www.isDirectory()) {
      return false;
    }
    if (!www.canExecute()) {
      System.out.println("Problem accessing files in folder " + www);
      return false;
    }
    if (!ssh.execSyncCommand("special-storage-available")) {
      System.out.println("Problem accessing board folder /www/sd");
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
      if (file.isDirectory() && file.canExecute()) {
        scp.startFolder(file.getName());
        recursiveSCP(file, scp);
        scp.endFolder();
      } else if (file.isFile() && file.canRead()) {
        scp.sendFile(file);
      }
    }
  }

  @Override
  public boolean burnBootloader() throws RunnerException {
    throw new RunnerException("Can't burn bootloader via SSH");
  }

}
