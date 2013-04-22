package processing.app.debug;

import org.junit.Before;
import org.junit.Test;
import processing.app.AbstractWithPreferencesTest;
import processing.app.helpers.PreferencesMap;

import java.io.File;
import java.util.HashMap;
import java.util.Map;

import static org.junit.Assert.assertTrue;

public class UploaderFactoryTest extends AbstractWithPreferencesTest {

  private TargetPackage targetPackage;

  @Before
  public void setUp() throws Exception {
    targetPackage = new TargetPackage("arduino", new File(".", "hardware/arduino/"));
  }

  @Test
  public void shouldCreateAnInstanceOfHttpUploader() throws Exception {
    TargetBoard board = targetPackage.getPlatforms().get("avr").getBoards().get("dogstick");
    Uploader uploader = new UploaderFactory().newUploader(board, "192.168.0.1 (mydogstick)");

    assertTrue(uploader instanceof HttpUploader);
  }

  @Test
  public void shouldCreateAnInstanceOfBasicUploaderWhenHTTPIsUnsupported() throws Exception {
    TargetBoard board = targetPackage.getPlatforms().get("avr").getBoards().get("uno");
    Uploader uploader = new UploaderFactory().newUploader(board, "192.168.0.1 (mydogstick)");

    assertTrue(uploader instanceof BasicUploader);
  }

  @Test
  public void shouldCreateAnInstanceOfBasicUploaderWhenPortIsSerial() throws Exception {
    TargetBoard board = targetPackage.getPlatforms().get("avr").getBoards().get("uno");
    Uploader uploader = new UploaderFactory().newUploader(board, "/dev/ttyACM0 (Arduino Leonardo)");

    assertTrue(uploader instanceof BasicUploader);
  }
}
