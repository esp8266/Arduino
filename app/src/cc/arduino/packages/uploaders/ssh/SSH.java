package cc.arduino.packages.uploaders.ssh;

import com.jcraft.jsch.Channel;
import com.jcraft.jsch.ChannelExec;
import com.jcraft.jsch.JSchException;
import com.jcraft.jsch.Session;

import java.io.IOException;
import java.io.InputStream;
import java.io.PrintStream;

public class SSH {

  protected final Session session;

  public SSH(Session session) {
    this.session = session;
  }

  public boolean execSyncCommand(String command) throws JSchException, IOException {
    return execSyncCommand(command, null, null);
  }

  public boolean execSyncCommand(String command, PrintStream stdoutConsumer, PrintStream stderrConsumer) throws JSchException, IOException {
    InputStream stdout = null;
    InputStream stderr = null;
    Channel channel = null;
    try {
      channel = session.openChannel("exec");
      ((ChannelExec) channel).setCommand(command);

      channel.setInputStream(null);

      stdout = channel.getInputStream();
      stderr = ((ChannelExec) channel).getErrStream();

      channel.connect();

      int exitCode = consumeOutputSyncAndReturnExitCode(channel, stdout, stdoutConsumer, stderr, stderrConsumer);

      return exitCode == 0;

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

  protected void consumeStream(byte[] buffer, InputStream in, PrintStream out) throws IOException {
    while (in.available() > 0) {
      int length = in.read(buffer, 0, buffer.length);
      if (length < 0) {
        break;
      }
      if (out != null) {
        out.print(new String(buffer, 0, length));
      }
    }
  }

}
