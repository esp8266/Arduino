package cc.arduino.packages;

import cc.arduino.packages.uploaders.SSHUploader;
import cc.arduino.packages.uploaders.SerialUploader;
import processing.app.*;
import processing.app.debug.TargetBoard;

public class UploaderFactory {

  public Uploader newUploader(TargetBoard board, String port) {
    if ("true".equals(board.getPreferences().get("upload.via_ssh")) && Constants.IPV4_ADDRESS.matcher(port).find()) {
      return new SSHUploader(port);
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
