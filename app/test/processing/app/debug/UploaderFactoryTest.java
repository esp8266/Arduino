package processing.app.debug;

import org.junit.Test;
import processing.app.AbstractWithPreferencesTest;
import processing.app.helpers.PreferencesMap;

import java.util.HashMap;
import java.util.Map;

import static org.junit.Assert.assertTrue;

public class UploaderFactoryTest extends AbstractWithPreferencesTest {

  @Test
  public void shouldCreateAnInstanceOfHttpUploader() throws Exception {
    Map<String, String> prefs = new HashMap<String, String>();
    prefs.put("upload.via_http", "true");
    TargetBoard board = new TargetBoard("dummy", new PreferencesMap(prefs), null);
    Uploader uploader = new UploaderFactory().newUploader(board, "192.168.0.1 (mydogstick)");

    assertTrue(uploader instanceof HttpUploader);
  }

  @Test
  public void shouldCreateAnInstanceOfBasicUploaderWhenHTTPIsUnsupported() throws Exception {
    Map<String, String> prefs = new HashMap<String, String>();
    TargetBoard board = new TargetBoard("dummy", new PreferencesMap(prefs), null);
    Uploader uploader = new UploaderFactory().newUploader(board, "192.168.0.1 (mydogstick)");

    assertTrue(uploader instanceof BasicUploader);
  }

  @Test
  public void shouldCreateAnInstanceOfBasicUploaderWhenPortIsSerial() throws Exception {
    Map<String, String> prefs = new HashMap<String, String>();
    TargetBoard board = new TargetBoard("dummy", new PreferencesMap(prefs), null);
    Uploader uploader = new UploaderFactory().newUploader(board, "/dev/ttyACM0 (Arduino Leonardo)");

    assertTrue(uploader instanceof BasicUploader);
  }
}
