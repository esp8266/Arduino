package processing.app;

import java.io.*;

public class TestHelper {

  public static String inputStreamToString(InputStream is) throws IOException {
    StringWriter sw = new StringWriter();

    BufferedReader reader = new BufferedReader(new InputStreamReader(is));
    String line;
    try {
      while ((line = reader.readLine()) != null) {
        sw.append(line).append('\n');
      }
      return sw.toString();
    } finally {
      is.close();
    }
  }
}
