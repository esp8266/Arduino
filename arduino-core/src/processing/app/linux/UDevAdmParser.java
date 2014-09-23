package processing.app.linux;

import java.io.IOException;
import java.io.StringReader;
import java.util.Properties;

public class UDevAdmParser {

  public String extractVIDAndPID(String output) throws IOException {
    Properties properties = new Properties();
    properties.load(new StringReader(output));

    Object vid = properties.get("ID_VENDOR_ID");
    Object pid = properties.get("ID_MODEL_ID");
    if (vid == null || pid == null)
      return null;
    return ("0x" + vid + "_0x" + pid).toUpperCase();
  }

}
