package processing.app.debug;

import org.junit.Before;
import org.junit.Test;

import cc.arduino.packages.Uploader;
import cc.arduino.packages.UploaderFactory;
import cc.arduino.packages.uploaders.HttpUploader;
import cc.arduino.packages.uploaders.SerialUploader;
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
  public void shouldCreateAnInstanceOfHttpUploader() throws Exception {
    TargetBoard board = targetPackage.getPlatforms().get("avr").getBoards().get("yun");
    Uploader uploader = new UploaderFactory().newUploader(board, "192.168.0.1 (yun)");

    assertTrue(uploader instanceof HttpUploader);
  }

  @Test
  public void shouldCreateAnInstanceOfBasicUploaderWhenHTTPIsUnsupported() throws Exception {
    TargetBoard board = targetPackage.getPlatforms().get("avr").getBoards().get("uno");
    Uploader uploader = new UploaderFactory().newUploader(board, "192.168.0.1 (myyun)");

    assertTrue(uploader instanceof SerialUploader);
  }

  @Test
  public void shouldCreateAnInstanceOfBasicUploaderWhenPortIsSerial() throws Exception {
    TargetBoard board = targetPackage.getPlatforms().get("avr").getBoards().get("uno");
    Uploader uploader = new UploaderFactory().newUploader(board, "/dev/ttyACM0 (Arduino Leonardo)");

    assertTrue(uploader instanceof SerialUploader);
  }
}
