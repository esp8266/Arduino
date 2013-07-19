package processing.app.helpers;

import java.io.IOException;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;

public abstract class NetUtils {

  public static boolean isReachable(InetAddress address) {
    Socket socket = null;
    try {
      socket = new Socket();
      socket.connect(new InetSocketAddress(address, 80), 100);
      return true;
    } catch (IOException e) {
      return false;
    } finally {
      if (socket != null) {
        try {
          socket.close();
        } catch (IOException e) {
          // noop
        }
      }
    }
  }
}
