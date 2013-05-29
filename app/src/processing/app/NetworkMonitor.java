package processing.app;

import processing.app.debug.MessageSiphon;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.util.regex.Matcher;

public class NetworkMonitor extends AbstractMonitor {

  private static final int MAX_CONNECT_RETRIES = 3;

  private final String ipAddress;
  private final Base base;

  private Socket socket;
  private MessageSiphon consumer;

  public NetworkMonitor(String port, Base base) {
    super(port);
    this.base = base;

    Matcher matcher = Constants.IPV4_ADDRESS.matcher(port);
    matcher.find();
    this.ipAddress = matcher.group();

    onSendCommand(new ActionListener() {
      public void actionPerformed(ActionEvent event) {
        try {
          socket.getOutputStream().write(textField.getText().getBytes());
          socket.getOutputStream().write('\n');
          socket.getOutputStream().flush();
        } catch (IOException e) {
          e.printStackTrace();
        }
        textField.setText("");
      }
    });
  }

  @Override
  public void open() throws IOException {
    try {
      socket = new Socket();
      socket.connect(new InetSocketAddress(ipAddress, 6571), 5000);
      consumer = new MessageSiphon(socket.getInputStream(), this);
      return;
    } catch (IOException e) {
      socket = null;
      throw e;
    }
  }

  private void sleep(long millis) {
    try {
      Thread.sleep(millis);
    } catch (InterruptedException e) {
      // ignore
    }
  }

  @Override
  public void close() throws IOException {
    if (socket != null) {
      consumer.stop();
      socket.close();
      textArea.setText("");
    }
  }
}
