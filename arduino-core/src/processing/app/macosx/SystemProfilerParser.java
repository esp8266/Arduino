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
  private static final String DEV_TTY_USBSERIAL = "/dev/tty.usbserial-";
  private static final String DEV_CU_USBSERIAL = "/dev/cu.usbserial-";
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
    if (serial.startsWith(DEV_TTY_USBSERIAL)) {
      devicePrefix = DEV_TTY_USBSERIAL;
    } else if (serial.startsWith(DEV_CU_USBSERIAL)) {
      devicePrefix = DEV_CU_USBSERIAL;
    } else if (serial.startsWith(DEV_TTY_USBMODEM)) {
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
        if ((serial.startsWith(DEV_TTY_USBSERIAL) || serial.startsWith(DEV_CU_USBSERIAL))) {
          String devicePath = devicePrefix + matcher.group(1);
          device.put(DEVICE_PATH, devicePath);
        }
      } else if ((serial.startsWith(DEV_TTY_USBMODEM) || serial.startsWith(DEV_CU_USBMODEM)) && (matcher = locationRegex.matcher(line)).matches()) {
        String suffix = matcher.group(1).substring(2, 6).replaceAll("0", "");
        String devicePath = devicePrefix + suffix + "1";
        device.put(DEVICE_PATH, devicePath);
      } else if ((matcher = pidRegex.matcher(line)).matches()) {
        String pid = matcher.group(1);
        if (pid.indexOf(" ") > 0)
          pid = pid.substring(0, pid.indexOf(" ")); // Remove any text after the hex number
        device.put(PID, pid);
      } else if ((matcher = vidRegex.matcher(line)).matches()) {
        String vid = matcher.group(1);
        if (vid.indexOf(" ") > 0)
          vid = vid.substring(0, vid.indexOf(" ")); // Remove any text after the hex number
        device.put(VID, vid);
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
