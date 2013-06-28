package cc.arduino.packages.uploaders.ssh;

import com.jcraft.jsch.Channel;
import com.jcraft.jsch.ChannelExec;
import com.jcraft.jsch.JSchException;
import com.jcraft.jsch.Session;

import java.io.*;

public class SCP extends SSH {

  private Channel channel;
  private OutputStream out;
  private InputStream in;

  public SCP(Session session) {
    super(session);
  }

  public void open() throws JSchException, IOException {
    try {
      channel = session.openChannel("exec");
      ((ChannelExec) channel).setCommand("scp -t -r -d /");

      out = channel.getOutputStream();
      in = channel.getInputStream();

      channel.connect();
      ensureAcknowledged();
    } catch (Exception e) {
      close();
    }
  }

  public void close() throws IOException {
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

  protected void ensureAcknowledged() throws IOException {
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

  public void sendFile(File localFile) throws IOException {
    sendFile(localFile, localFile.getName());
  }

  public void sendFile(File localFile, String remoteFile) throws IOException {
    out.write(("C0644 " + localFile.length() + " " + remoteFile + "\n").getBytes());
    ensureAcknowledged();

    FileInputStream fis = null;
    try {
      fis = new FileInputStream(localFile);
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

    ensureAcknowledged();
  }

  public void startFolder(String folder) throws IOException {
    out.write(("D0755 0 " + folder + "\n").getBytes());
    ensureAcknowledged();
  }

  public void endFolder() throws IOException {
    out.write("E\n".getBytes());
    ensureAcknowledged();
  }

}
