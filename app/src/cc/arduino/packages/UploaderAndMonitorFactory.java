/*
  Copyright (c) 2013 Arduino LLC. All right reserved.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

package cc.arduino.packages;

import cc.arduino.packages.uploaders.SSHUploader;
import cc.arduino.packages.uploaders.SerialUploader;
import processing.app.*;
import processing.app.debug.TargetBoard;

public class UploaderAndMonitorFactory {

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
