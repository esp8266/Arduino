package processing.app.windows;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.StringReader;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * Interprets the output of listComPorts.exe
 * <p/>
 * https://github.com/todbot/usbSearch/
 */
public class ListComPortsParser {

  private final Pattern vidRegExp;
  private final Pattern pidRegExp;

  public ListComPortsParser() {
    vidRegExp = Pattern.compile("VID_(\\w\\w\\w\\w)");
    pidRegExp = Pattern.compile("PID_(\\w\\w\\w\\w)");
  }

  public String extractVIDAndPID(String output, String serial) throws IOException {
    BufferedReader reader = new BufferedReader(new StringReader(output));
    String line;
    while ((line = reader.readLine()) != null) {
      if (line.startsWith(serial.toUpperCase())) {
        Matcher vidMatcher = vidRegExp.matcher(line);
        Matcher pidMatcher = pidRegExp.matcher(line);
        if (vidMatcher.find() && pidMatcher.find()) {
          return ("0x" + vidMatcher.group(1) + "_0x" + pidMatcher.group(1)).toUpperCase();
        }
      }
    }

    return null;
  }

}
