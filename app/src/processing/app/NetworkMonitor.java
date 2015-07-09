package processing.app;

import cc.arduino.packages.BoardPort;
import cc.arduino.packages.ssh.NoInteractionUserInfo;
import cc.arduino.packages.ssh.SSHClientSetupChainRing;
import cc.arduino.packages.ssh.SSHConfigFileSetup;
import cc.arduino.packages.ssh.SSHPwdSetup;

import com.jcraft.jsch.*;

import processing.app.debug.MessageConsumer;
import processing.app.debug.MessageSiphon;

import javax.swing.*;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import static processing.app.I18n._;

@SuppressWarnings("serial")
public class NetworkMonitor extends AbstractMonitor implements MessageConsumer {

  private static final int MAX_CONNECTION_ATTEMPTS = 5;

  private final BoardPort port;
  private final String ipAddress;

  private MessageSiphon inputConsumer;
  private Session session;
  private Channel channel;
  private int connectionAttempts;

  public NetworkMonitor(BoardPort port) {
    super(port.getLabel());
    this.port = port;
    this.ipAddress = port.getAddress();

    onSendCommand(new ActionListener() {
      public void actionPerformed(ActionEvent event) {
        try {
          OutputStream out = channel.getOutputStream();
          out.write(textField.getText().getBytes());
          out.write('\n');
          out.flush();
        } catch (IOException e) {
          e.printStackTrace();
        }
        textField.setText("");
      }
    });
  }

  @Override
  public boolean requiresAuthorization() {
    return true;
  }

  @Override
  public String getAuthorizationKey() {
    return "runtime.pwd." + ipAddress;
  }

  @Override
  public void open() throws Exception {
    this.connectionAttempts = 0;

    JSch jSch = new JSch();
    SSHClientSetupChainRing sshClientSetupChain = new SSHConfigFileSetup(new SSHPwdSetup());
    session = sshClientSetupChain.setup(port, jSch);

    session.setUserInfo(new NoInteractionUserInfo(PreferencesData.get(getAuthorizationKey())));
    session.connect(30000);

    tryConnect();
  }

  private void tryConnect() throws JSchException, IOException {
    connectionAttempts++;

    if (connectionAttempts > 1) {
      try {
        Thread.sleep(2000);
      } catch (InterruptedException e) {
        // ignored
      }
    }

    if (!session.isConnected()) {
      return;
    }
    channel = session.openChannel("exec");
    ((ChannelExec) channel).setCommand("telnet localhost 6571");

    InputStream inputStream = channel.getInputStream();
    InputStream errStream = ((ChannelExec) channel).getErrStream();

    channel.connect();

    inputConsumer = new MessageSiphon(inputStream, this);
    new MessageSiphon(errStream, this);

    if (connectionAttempts > 1) {
      SwingUtilities.invokeLater(new Runnable() {

        @Override
        public void run() {
          try {
            Thread.sleep(1000);
          } catch (InterruptedException e) {
            // ignore
          }
          if (channel.isConnected()) {
            NetworkMonitor.this.message(_("connected!") + '\n');
          }
        }

      });
    }
  }

  @Override
  public synchronized void message(String s) {
    if (s.contains("can't connect")) {
      while (!channel.isClosed()) {
        try {
          Thread.sleep(100);
        } catch (InterruptedException e) {
          // ignore
        }
      }
      if (connectionAttempts < MAX_CONNECTION_ATTEMPTS) {
        s = "\n" + _("Unable to connect: retrying") + " (" + connectionAttempts + ")... ";

        SwingUtilities.invokeLater(new Runnable() {
          @Override
          public void run() {
            try {
              NetworkMonitor.this.tryConnect();
            } catch (JSchException e) {
              e.printStackTrace();
            } catch (IOException e) {
              e.printStackTrace();
            }
          }
        });
      } else {
        s = "\n" + _("Unable to connect: is the sketch using the bridge?");
      }
    }
    super.message(s);
  }

  @Override
  public void close() throws Exception {
    if (channel != null) {
      inputConsumer.stop();
      channel.disconnect();
      textArea.setText("");
    }

    if (session != null) {
      session.disconnect();
    }
  }

}
