package cc.arduino.contributions;

import com.github.zafarkhaja.semver.Version;

public class VersionHelper {

  public static Version valueOf(String ver) {
    if (ver == null) {
      return null;
    }
    try {
      String[] verParts = ver.split("\\.");
      if (verParts.length < 3) {
        if (verParts.length == 2) {
          return Version.forIntegers(Integer.valueOf(verParts[0]), Integer.valueOf(verParts[1]));
        } else {
          return Version.forIntegers(Integer.valueOf(verParts[0]));
        }
      } else {
        return Version.valueOf(ver);
      }
    } catch (Exception e) {
      System.err.println("Invalid version found: " + ver);
      return null;
    }
  }

}
