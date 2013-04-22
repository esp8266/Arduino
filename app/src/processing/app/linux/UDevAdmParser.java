package processing.app.linux;

import java.io.IOException;
import java.io.StringReader;
import java.util.Properties;

public class UDevAdmParser {

  public String extractVIDAndPID(String output) throws IOException {
    Properties properties = new Properties();
    properties.load(new StringReader(output));

    return ("0x" + properties.get("ID_VENDOR_ID").toString() + "_0x" + properties.get("ID_MODEL_ID").toString()).toUpperCase();
  }

}
