package processing.app.zeroconf;

import javax.jmdns.ServiceEvent;

public interface BoardListener {

  void boardOffline(ServiceEvent serviceEvent);

  void boardOnline(ServiceEvent serviceEvent);

}
