package processing.app.helpers;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.Random;

public class FileUtils {

  /**
   * Checks, whether the child directory is a subdirectory of the base directory.
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

  public static void copy(File sourceFolder, File destFolder) throws IOException {
    for (File file : sourceFolder.listFiles()) {
      File destFile = new File(destFolder, file.getName());
      if (file.isDirectory()) {
        if (!destFile.mkdir()) {
          throw new IOException("Unable to create folder: " + destFile);
        }
        copy(file, destFile);
      } else {
        FileInputStream fis = null;
        FileOutputStream fos = null;
        try {
          fis = new FileInputStream(file);
          fos = new FileOutputStream(destFile);
          byte[] buf = new byte[4096];
          int readBytes = -1;
          while ((readBytes = fis.read(buf, 0, buf.length)) != -1) {
            fos.write(buf, 0, readBytes);
          }
        } finally {
          if (fis != null) {
            fis.close();
          }
          if (fos != null) {
            fos.close();
          }
        }
      }
    }
  }

  public static void recursiveDelete(File file) {
    if (file == null) {
      return;
    }
    if (file.isDirectory()) {
      for (File current : file.listFiles()) {
        if (current.isDirectory()) {
          recursiveDelete(current);
        } else {
          current.delete();
        }
      }
    }
    file.delete();
  }

  public static File createTempFolder() throws IOException {
    File tmpFolder = new File(System.getProperty("java.io.tmpdir"), "arduino_" + new Random().nextInt(1000000));
    if (!tmpFolder.mkdir()) {
      throw new IOException("Unable to create temp folder " + tmpFolder);
    }
    return tmpFolder;
  }

}
