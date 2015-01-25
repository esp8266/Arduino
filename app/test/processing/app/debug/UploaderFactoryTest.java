package processing.app.debug;

import cc.arduino.packages.BoardPort;
import cc.arduino.packages.Uploader;
import cc.arduino.packages.UploaderFactory;
import cc.arduino.packages.uploaders.SSHUploader;
import cc.arduino.packages.uploaders.SerialUploader;
import org.junit.Before;
import org.junit.Test;
import processing.app.AbstractWithPreferencesTest;

import java.io.File;

import static org.junit.Assert.assertTrue;

public class UploaderFactoryTest extends AbstractWithPreferencesTest {

  private TargetPackage targetPackage;

  @Before
  public void setUp() throws Exception {
    targetPackage = new TargetPackage("arduino", new File(".", "hardware/arduino/"));
  }

  @Test
  public void shouldCreateAnInstanceOfSSHUploader() throws Exception {
    TargetBoard board = targetPackage.getPlatforms().get("avr").getBoards().get("yun");
    BoardPort boardPort = new BoardPort();
    boardPort.setBoardName("yun");
    boardPort.setAddress("192.168.0.1");
    boardPort.setProtocol("network");
    Uploader uploader = new UploaderFactory().newUploader(board, boardPort, false);

    assertTrue(uploader instanceof SSHUploader);
  }

  @Test
  public void shouldCreateAnInstanceOfBasicUploaderWhenSSHIsUnsupported() throws Exception {
    TargetBoard board = targetPackage.getPlatforms().get("avr").getBoards().get("uno");
    BoardPort boardPort = new BoardPort();
    boardPort.setBoardName("myyun");
    boardPort.setAddress("192.168.0.1");
    boardPort.setProtocol("network");
    Uploader uploader = new UploaderFactory().newUploader(board, boardPort, false);

    assertTrue(uploader instanceof SerialUploader);
  }

  @Test
  public void shouldCreateAnInstanceOfBasicUploaderWhenPortIsSerial() throws Exception {
    TargetBoard board = targetPackage.getPlatforms().get("avr").getBoards().get("uno");
    BoardPort boardPort = new BoardPort();
    boardPort.setBoardName("Arduino Leonardo");
    boardPort.setAddress("/dev/ttyACM0");
    boardPort.setProtocol("serial");
    Uploader uploader = new UploaderFactory().newUploader(board, boardPort, false);

    assertTrue(uploader instanceof SerialUploader);
  }
}
