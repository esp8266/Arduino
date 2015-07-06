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
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import static processing.app.I18n._;

@SuppressWarnings("serial")
public class NetworkMonitor extends AbstractMonitor implements MessageConsumer {

  private static final int MAX_CONNECTION_ATTEMPTS = 5;

  private MessageSiphon inputConsumer;
  private Session session;
  private Channel channel;
  private int connectionAttempts;
  private Socket socket;
  private int remotePort;
  private boolean useSSH;

  public NetworkMonitor(BoardPort port) {
    super(port);
    remotePort = Integer.parseInt(port.getPrefs().get("port"));
    useSSH = (remotePort == 22 || remotePort == 80);
    
    onSendCommand(new ActionListener() {
      public void actionPerformed(ActionEvent event) {
        try {
          OutputStream out;
          if(useSSH){
            out = channel.getOutputStream();
          } else {
            out = socket.getOutputStream();
          }
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
    return "runtime.pwd." + getBoardPort().getAddress();
  }

  @Override
  public void open() throws Exception {
    super.open();
    
    if(useSSH){
      this.connectionAttempts = 0;
    
      JSch jSch = new JSch();
      SSHClientSetupChainRing sshClientSetupChain = new SSHConfigFileSetup(new SSHPwdSetup());
      session = sshClientSetupChain.setup(getBoardPort(), jSch);

      session.setUserInfo(new NoInteractionUserInfo(PreferencesData.get(getAuthorizationKey())));
      session.connect(30000);
    }
    
    tryConnect();
  }
  
  private void tryConnect() throws JSchException, IOException {
    if(useSSH){
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
    } else {
      socket = new Socket();
      socket.connect(new InetSocketAddress(getBoardPort().getAddress(), remotePort), 1000);
      socket.setTcpNoDelay(true);
      inputConsumer = new MessageSiphon(socket.getInputStream(), this);
      String password = PreferencesData.get(getAuthorizationKey());
      if(password.length() > 0){
        OutputStream out = socket.getOutputStream();
        out.write(password.getBytes());
        out.write('\n');
        out.flush();
      }
    }
  }

  @Override
  public synchronized void message(String s) {
    if(useSSH){
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
    }
    super.message(s);
  }

  @Override
  public void close() throws Exception {
    super.close();
    
    if(useSSH){
      if (channel != null) {
        inputConsumer.stop();
        channel.disconnect();
        textArea.setText("");
      }

      if (session != null) {
        session.disconnect();
      }
    } else {
      if (socket != null) {
        socket.close();
        textArea.setText("");
      }
    }
  }
}
