package processing.app;

import processing.app.debug.BasicUploader;
import processing.app.debug.HttpUploader;
import processing.app.debug.TargetBoard;
import processing.app.debug.Uploader;

import java.util.regex.Pattern;

public class PerPortObjectFactory {

  public Uploader newUploader(TargetBoard board, String port) {
    if ("true".equals(board.getPreferences().get("upload.via_http")) && Constants.IPV4_ADDRESS.matcher(port).find()) {
      return new HttpUploader(port);
    }

    return new BasicUploader();
  }

  public AbstractMonitor newMonitor(String port, Base base) {
    if (Constants.IPV4_ADDRESS.matcher(port).find()) {
      return new NetworkMonitor(port, base);
    }

    return new SerialMonitor(port);
  }

}
