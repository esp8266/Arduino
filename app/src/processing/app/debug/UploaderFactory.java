package processing.app.debug;

import processing.app.Base;
import processing.app.Preferences;
import processing.app.helpers.Maps;

import java.util.Map;

public class UploaderFactory {

  public Uploader newUploader(Map<String, String> preferences) {
    if ("http".equals(preferences.get("upload.tool"))) {
      return new HttpUploader();
    }

    return new BasicUploader();
  }

}
