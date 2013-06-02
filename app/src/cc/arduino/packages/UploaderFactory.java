package cc.arduino.packages;

import processing.app.AbstractMonitor;
import processing.app.Base;
import processing.app.Constants;
import processing.app.NetworkMonitor;
import processing.app.SerialMonitor;
import processing.app.debug.TargetBoard;
import cc.arduino.packages.uploaders.HttpUploader;
import cc.arduino.packages.uploaders.SerialUploader;

public class UploaderFactory {

  public Uploader newUploader(TargetBoard board, String port) {
    if ("true".equals(board.getPreferences().get("upload.via_http")) && Constants.IPV4_ADDRESS.matcher(port).find()) {
      return new HttpUploader(port);
    }

    return new SerialUploader();
  }

  public AbstractMonitor newMonitor(String port, Base base) {
    if (Constants.IPV4_ADDRESS.matcher(port).find()) {
      return new NetworkMonitor(port, base);
    }

    return new SerialMonitor(port);
  }

}
