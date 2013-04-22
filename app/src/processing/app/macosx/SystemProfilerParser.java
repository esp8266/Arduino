package processing.app.macosx;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.StringReader;
import java.util.HashMap;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class SystemProfilerParser {

  private static final String DEVICE_PATH = "device_path";
  private static final String VID = "vid";
  private static final String PID = "pid";
  private static final String SERIAL_NUMBER = "serial_number";
  private static final String DEV_TTY = "/dev/tty.";
  private static final String DEV_TTY_USBMODEM = "/dev/tty.usbmodem";
  private static final String DEV_CU_USBMODEM = "/dev/cu.usbmodem";

  private final Pattern vidRegex;
  private final Pattern serialNumberRegex;
  private final Pattern locationRegex;
  private final Pattern pidRegex;

  public SystemProfilerParser() {
    this.serialNumberRegex = Pattern.compile("^Serial Number: (.+)$");
    this.locationRegex = Pattern.compile("^Location ID: (.+)$");
    this.pidRegex = Pattern.compile("^Product ID: (.+)$");
    this.vidRegex = Pattern.compile("^Vendor ID: (.+)$");
  }

  public String extractVIDAndPID(String output, String serial) throws IOException {
    BufferedReader reader = new BufferedReader(new StringReader(output));

    String devicePrefix;
    if (serial.startsWith(DEV_TTY)) {
      devicePrefix = DEV_TTY_USBMODEM;
    } else {
      devicePrefix = DEV_CU_USBMODEM;
    }

    Map<String, String> device = new HashMap<String, String>();

    String line;
    Matcher matcher;
    while ((line = reader.readLine()) != null) {
      line = line.trim();
      line = line.replaceAll("\\s+", " ");

      if ((matcher = serialNumberRegex.matcher(line)).matches()) {
        device.put(SERIAL_NUMBER, matcher.group(1));
      } else if ((matcher = locationRegex.matcher(line)).matches()) {
        device.put(DEVICE_PATH, devicePrefix + matcher.group(1).substring(2, 6) + "1");
      } else if ((matcher = pidRegex.matcher(line)).matches()) {
        device.put(PID, matcher.group(1));
      } else if ((matcher = vidRegex.matcher(line)).matches()) {
        device.put(VID, matcher.group(1));
      } else if (line.equals("")) {
        if (device.containsKey(DEVICE_PATH) && device.get(DEVICE_PATH).equals(serial)) {
          return (device.get(VID) + "_" + device.get(PID)).toUpperCase();
        }
        device = new HashMap<String, String>();
      }
    }

    return null;
  }

}
