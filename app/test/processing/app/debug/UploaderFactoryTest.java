package processing.app.debug;

import org.junit.Test;
import processing.app.AbstractWithPreferencesTest;

import java.util.HashMap;
import java.util.Map;

import static org.junit.Assert.assertTrue;

public class UploaderFactoryTest extends AbstractWithPreferencesTest {

  @Test
  public void shouldCreateAnInstanceOfHttpUploader() throws Exception {
    Map<String, String> prefs = new HashMap<String, String>();
    prefs.put("upload.via_http", "true");
    Uploader uploader = new UploaderFactory().newUploader(prefs, "192.168.0.1 (mydogstick)");

    assertTrue(uploader instanceof HttpUploader);
  }

  @Test
  public void shouldCreateAnInstanceOfBasicUploaderWhenHTTPIsUnsupported() throws Exception {
    Map<String, String> prefs = new HashMap<String, String>();
    Uploader uploader = new UploaderFactory().newUploader(prefs, "192.168.0.1 (mydogstick)");

    assertTrue(uploader instanceof BasicUploader);
  }

  @Test
  public void shouldCreateAnInstanceOfBasicUploaderWhenPortIsSerial() throws Exception {
    Map<String, String> prefs = new HashMap<String, String>();
    Uploader uploader = new UploaderFactory().newUploader(prefs, "/dev/ttyACM0 (Arduino Leonardo)");

    assertTrue(uploader instanceof BasicUploader);
  }
}
