package processing.app.macosx;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.StringReader;
import java.util.HashMap;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class SystemProfilerParser {

  private final Pattern vidRegex;
  private final Pattern serialNumberRegex;
  private final Pattern locationRegex;
  private final Pattern pidRegex;

  public SystemProfilerParser() {
    serialNumberRegex = Pattern.compile("^Serial Number: (.+)$");
    locationRegex = Pattern.compile("^Location ID: (.+)$");
    pidRegex = Pattern.compile("^Product ID: (.+)$");
    vidRegex = Pattern.compile("^Vendor ID: (.+)$");
  }

  public String extractVIDAndPID(String output, String serial) throws IOException {
    BufferedReader reader = new BufferedReader(new StringReader(output));

    String devicePrefix;
    if (serial.startsWith("/dev/tty.")) {
      devicePrefix = "/dev/tty.usbmodem";
    } else {
      devicePrefix = "/dev/cu.usbmodem";
    }

    Map<String, String> device = new HashMap<String, String>();

    String line;
    Matcher matcher;
    while ((line = reader.readLine()) != null) {
      line = line.trim();
      line = line.replaceAll("\\s+", " ");

      if ((matcher = serialNumberRegex.matcher(line)).matches()) {
        device.put("serial_number", matcher.group(1));
      } else if ((matcher = locationRegex.matcher(line)).matches()) {
        device.put("device_path", devicePrefix + matcher.group(1).substring(2, 6) + "1");
      } else if ((matcher = pidRegex.matcher(line)).matches()) {
        device.put("pid", matcher.group(1));
      } else if ((matcher = vidRegex.matcher(line)).matches()) {
        device.put("vid", matcher.group(1));
      } else if (line.equals("")) {
        if (device.containsKey("serial_number") && device.get("device_path").equals(serial)) {
          return device.get("vid").substring(2).toUpperCase() + "_" + device.get("pid").substring(2).toUpperCase();
        }
        device = new HashMap<String, String>();
      }
    }

    return null;
  }

}
