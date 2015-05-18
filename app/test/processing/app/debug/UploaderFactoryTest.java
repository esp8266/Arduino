/*
 * This file is part of Arduino.
 *
 * Copyright 2015 Arduino LLC (http://www.arduino.cc/)
 *
 * Arduino is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * As a special exception, you may use this file as part of a free software
 * library without restriction.  Specifically, if other files instantiate
 * templates or use macros or inline functions from this file, or you compile
 * this file and link it with other files to produce an executable, this
 * file does not by itself cause the resulting executable to be covered by
 * the GNU General Public License.  This exception does not however
 * invalidate any other reasons why the executable file might be covered by
 * the GNU General Public License.
 */

package processing.app.debug;

import cc.arduino.packages.BoardPort;
import cc.arduino.packages.Uploader;
import cc.arduino.packages.UploaderFactory;
import cc.arduino.packages.uploaders.SSHUploader;
import cc.arduino.packages.uploaders.SerialUploader;
import org.junit.Test;
import processing.app.AbstractWithPreferencesTest;
import processing.app.helpers.PreferencesMap;

import java.util.HashMap;

import static org.junit.Assert.assertTrue;

public class UploaderFactoryTest extends AbstractWithPreferencesTest {

  @Test
  public void shouldCreateAnInstanceOfSSHUploader() throws Exception {
    TargetBoard board = new LegacyTargetBoard("yun", new PreferencesMap(new HashMap<String, String>()), new TargetPlatformStub("id", new TargetPackageStub("id")));
    board.getPreferences().put("upload.via_ssh", "true");

    BoardPort boardPort = new BoardPort();
    boardPort.setBoardName("yun");
    boardPort.setAddress("192.168.0.1");
    boardPort.setProtocol("network");
    Uploader uploader = new UploaderFactory().newUploader(board, boardPort, false);

    assertTrue(uploader instanceof SSHUploader);
  }

  @Test
  public void shouldCreateAnInstanceOfBasicUploaderWhenSSHIsUnsupported() throws Exception {
    TargetBoard board = new LegacyTargetBoard("uno", new PreferencesMap(new HashMap<String, String>()), new TargetPlatformStub("id", new TargetPackageStub("id")));
    board.getPreferences().put("upload.via_ssh", "false");

    BoardPort boardPort = new BoardPort();
    boardPort.setBoardName("myyun");
    boardPort.setAddress("192.168.0.1");
    boardPort.setProtocol("network");
    Uploader uploader = new UploaderFactory().newUploader(board, boardPort, false);

    assertTrue(uploader instanceof SerialUploader);
  }

  @Test
  public void shouldCreateAnInstanceOfBasicUploaderWhenPortIsSerial() throws Exception {
    TargetBoard board = new LegacyTargetBoard("uno", new PreferencesMap(new HashMap<String, String>()), new TargetPlatformStub("id", new TargetPackageStub("id")));
    board.getPreferences().put("upload.via_ssh", "false");

    BoardPort boardPort = new BoardPort();
    boardPort.setBoardName("Arduino Leonardo");
    boardPort.setAddress("/dev/ttyACM0");
    boardPort.setProtocol("serial");
    Uploader uploader = new UploaderFactory().newUploader(board, boardPort, false);

    assertTrue(uploader instanceof SerialUploader);
  }
}
