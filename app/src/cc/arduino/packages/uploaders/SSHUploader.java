package cc.arduino.packages.uploaders;

import cc.arduino.packages.Uploader;
import com.jcraft.jsch.*;
import processing.app.Base;
import processing.app.Constants;
import processing.app.NetworkMonitor;
import processing.app.Preferences;
import processing.app.debug.RunnerException;
import processing.app.debug.TargetPlatform;
import processing.app.helpers.FileUtils;
import processing.app.helpers.PreferencesMap;

import java.io.*;
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
    try {
      JSch jSch = new JSch();
      session = jSch.getSession("root", ipAddress, 22);
      session.setPassword(Preferences.get(getAuthorizationKey()));

      session.setUserInfo(new NetworkMonitor.NoInteractionUserInfo());
      session.connect(30000);

      SCP scp = new SCP(session);
      SSH ssh = new SSH(session);

      File www = new File(sourcePath, "www");
      if (www.exists() && www.isDirectory() && www.canExecute() && canUploadWebFiles(ssh)) {
        File destination = new File("/www/sd/" + www.getParentFile().getName());
        ssh.execSyncCommand("mkdir -p " + FileUtils.getLinuxPathFrom(destination), System.out);
        copyWebFiles(scp, ssh, www, destination);
      }

      String uploadedSketchFile = scp.scpHexToBoard(buildPath, className);

      TargetPlatform targetPlatform = Base.getTargetPlatform();
      PreferencesMap prefs = Preferences.getMap();
      prefs.putAll(Base.getBoardPreferences());
      prefs.putAll(targetPlatform.getTool(prefs.get("upload.tool")));

      return new SSHAVRDude(session).runAVRDude(uploadedSketchFile, verbose ? prefs.get("upload.params.verbose") : prefs.get("upload.params.quiet"));
    } catch (JSchException e) {
      if ("Auth cancel".equals(e.getMessage())) {
        return false;
      }
      throw new RunnerException(e);
    } catch (Exception e) {
      throw new RunnerException(e);
    } finally {
      if (session != null) {
        session.disconnect();
      }
    }
  }

  private void copyWebFiles(SCP scp, SSH ssh, File from, File destination) throws IOException, JSchException {
    File[] files = from.listFiles();
    if (files == null) {
      throw new IOException("Cannot list files in " + from);
    }
    for (File file : files) {
      if (file.isDirectory() && file.canExecute()) {
        File newDestination = new File(destination, file.getName());
        ssh.execSyncCommand("mkdir -p " + FileUtils.getLinuxPathFrom(newDestination), System.out);
        copyWebFiles(scp, ssh, file, newDestination);
      } else {
        scp.scpFile(file, FileUtils.getLinuxPathFrom(new File(destination, file.getName())));
      }
    }
  }

  private boolean canUploadWebFiles(SSH ssh) throws JSchException, IOException {
    ByteArrayOutputStream baos = new ByteArrayOutputStream();
    PrintStream ps = new PrintStream(baos);
    ssh.execSyncCommand("if [ -L /www/sd ] && [ -d /www/sd ]; then echo 1; else echo 0; fi", ps);
    String output = new String(baos.toByteArray());
    return "1".equals(output.trim());
  }

  @Override
  public boolean burnBootloader() throws RunnerException {
    throw new RunnerException("Can't burn bootloader via SSH");
  }

  private static class SSH {

    protected final Session session;

    public SSH(Session session) {
      this.session = session;
    }

    protected boolean execSyncCommand(String command, PrintStream stdoutConsumer) throws JSchException, IOException {
      return execSyncCommand(command, stdoutConsumer, null);
    }

    protected boolean execSyncCommand(String command, PrintStream stdoutConsumer, PrintStream stderrConsumer) throws JSchException, IOException {
      InputStream stdout = null;
      InputStream stderr = null;
      Channel channel = null;
      try {
        channel = session.openChannel("exec");
        ((ChannelExec) channel).setCommand(command);

        channel.setInputStream(null);

        stdout = channel.getInputStream();
        if (stderrConsumer != null) {
          stderr = ((ChannelExec) channel).getErrStream();
        }

        channel.connect();

        int exitCode = consumeOutputSyncAndReturnExitCode(channel, stdout, stdoutConsumer, stderr, stderrConsumer);

        return stderrConsumer == null || exitCode == 0;

      } finally {
        if (stdout != null) {
          stdout.close();
        }
        if (stderr != null) {
          stderr.close();
        }
        if (channel != null) {
          channel.disconnect();
        }
      }
    }

    protected int consumeOutputSyncAndReturnExitCode(Channel channel, InputStream stdout, PrintStream stdoutConsumer, InputStream stderr, PrintStream stderrConsumer) throws IOException {
      byte[] tmp = new byte[102400];
      while (true) {
        consumeStream(tmp, stdout, stdoutConsumer);
        consumeStream(tmp, stderr, stderrConsumer);

        if (channel.isClosed()) {
          return channel.getExitStatus();
        }
        try {
          Thread.sleep(100);
        } catch (Exception ee) {
          // noop
        }
      }
    }

    private void consumeStream(byte[] buffer, InputStream in, PrintStream out) throws IOException {
      while (in != null && in.available() > 0) {
        int length = in.read(buffer, 0, buffer.length);
        if (length < 0) {
          break;
        }
        out.print(new String(buffer, 0, length));
      }
    }

  }

  private static class SSHAVRDude extends SSH {

    public SSHAVRDude(Session session) {
      super(session);
    }

    public boolean runAVRDude(String sketchFile, String additionalParams) throws IOException, JSchException {
      boolean success = execSyncCommand("merge-sketch-with-bootloader " + sketchFile, System.out, System.err);
      success = success && execSyncCommand("kill-bridge", System.out);
      success = success && execSyncCommand("run-avrdude " + sketchFile + " '" + additionalParams + "'", System.out, System.err);
      return success;
    }

  }

  private static class SCP extends SSH {

    private static final String SKETCH_FILE = "/tmp/sketch.hex";

    public SCP(Session session) {
      super(session);
    }

    public void scpFile(File from, String absolutePathToDestination) throws JSchException, IOException {
      Channel channel = null;
      OutputStream out = null;
      InputStream in = null;
      try {
        channel = session.openChannel("exec");
        ((ChannelExec) channel).setCommand("scp -t " + absolutePathToDestination);

        out = channel.getOutputStream();
        in = channel.getInputStream();

        channel.connect();

        ensureAcknowledged(out, in);

        sendFileSizeAndName(out, in, from);
        ensureAcknowledged(out, in);

        sendFileContents(out, from);
        ensureAcknowledged(out, in);
      } finally {
        if (out != null) {
          out.close();
        }
        if (in != null) {
          in.close();
        }
        if (channel != null) {
          channel.disconnect();
        }
      }
    }

    public String scpHexToBoard(String buildPath, String className) throws JSchException, IOException {
      scpFile(new File(buildPath, className + ".hex"), SKETCH_FILE);
      return SKETCH_FILE;
    }

    private void ensureAcknowledged(OutputStream out, InputStream in) throws IOException {
      out.flush();

      int b = in.read();

      if (b == 0) return;
      if (b == -1) return;

      if (b == 1 || b == 2) {
        StringBuilder sb = new StringBuilder();
        sb.append("SCP error: ");

        int c;
        do {
          c = in.read();
          sb.append((char) c);
        } while (c != '\n');

        throw new IOException(sb.toString());
      }

      throw new IOException("Uknown SCP error: " + b);
    }

    private void sendFileContents(OutputStream out, File hex) throws IOException {
      FileInputStream fis = null;
      try {
        fis = new FileInputStream(hex);
        byte[] buf = new byte[4096];
        while (true) {
          int len = fis.read(buf, 0, buf.length);
          if (len <= 0) break;
          out.write(buf, 0, len);
        }

        // \0 terminates file
        buf[0] = 0;
        out.write(buf, 0, 1);
      } finally {
        if (fis != null) {
          fis.close();
        }
      }
    }

    private void sendFileSizeAndName(OutputStream out, InputStream in, File hex) throws IOException {
      long filesize = hex.length();
      String command = "C0644 " + filesize + " " + hex.getName() + "\n";
      out.write(command.getBytes());
    }

  }

}
