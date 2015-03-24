package processing.app.helpers;

import java.io.IOException;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.util.Arrays;
import java.util.List;

public abstract class NetUtils {

  private static boolean isReachableByEcho(InetAddress address) {
    try {
      return address.isReachable(300);
    } catch (IOException e) {
      return false;
    }
  }

  public static boolean isReachable(InetAddress address, int port) {
    return isReachable(address, Arrays.asList(port));
  }

  public static boolean isReachable(InetAddress address, List<Integer> ports) {
    if (isReachableByEcho(address)) {
      return true;
    }

    boolean reachable = false;
    for (Integer port : ports) {
      reachable = reachable || isPortOpen(address, port);
    }

    return reachable;
  }

  private static boolean isPortOpen(InetAddress address, int port) {
    Socket socket = null;
    try {
      socket = new Socket();
      socket.connect(new InetSocketAddress(address, port), 1000);
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
