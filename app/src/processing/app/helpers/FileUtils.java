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
   * @throws IOException
   *           if an IOError occured during the test.
   */
  public static boolean isSubDirectory(File base, File child) throws IOException {
    base = base.getCanonicalFile();
    child = child.getCanonicalFile();

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
