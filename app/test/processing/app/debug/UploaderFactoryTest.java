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
    prefs.put("upload.tool", "http");
    Uploader uploader = new UploaderFactory().newUploader(prefs);

    assertTrue(uploader instanceof HttpUploader);
  }

  @Test
  public void shouldCreateAnInstanceOfBasicUploader() throws Exception {
    Map<String, String> prefs = new HashMap<String, String>();
    prefs.put("upload.tool", "whatever");
    Uploader uploader = new UploaderFactory().newUploader(prefs);

    assertTrue(uploader instanceof BasicUploader);
  }
}
