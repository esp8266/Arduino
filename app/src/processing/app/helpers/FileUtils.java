package processing.app.helpers;

import java.io.File;
import java.io.IOException;

public class FileUtils {
  
  /**
   * Checks, whether the child directory is a subdirectory of the base
   * directory.
   * 
   * @param base
   *          the base directory.
   * @param child
   *          the suspected child directory.
   * @return true, if the child is a subdirectory of the base directory.
   */
  public static boolean isSubDirectory(File base, File child) {
    try {
      base = base.getCanonicalFile();
      child = child.getCanonicalFile();
    } catch (IOException e) {
      return false;
    }

    File parentFile = child;
    while (parentFile != null) {
      if (base.equals(parentFile)) {
        return true;
      }
      parentFile = parentFile.getParentFile();
    }
    return false;
  }
}
