package processing.app.helpers;

import java.io.*;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Random;
import java.util.regex.Pattern;

public class FileUtils {

  private static final List<String> SOURCE_CONTROL_FOLDERS = Arrays.asList("CVS", "RCS", ".git", ".svn", ".hg", ".bzr");
  private static final Pattern BACKSLASH = Pattern.compile("\\\\");

  /**
   * Checks, whether the child directory is a subdirectory of the base directory.
   *
   * @param base  the base directory.
   * @param child the suspected child directory.
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

  public static void copyFile(File source, File dest) throws IOException {
    FileInputStream fis = null;
    FileOutputStream fos = null;
    try {
      fis = new FileInputStream(source);
      fos = new FileOutputStream(dest);
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

  public static void copy(File sourceFolder, File destFolder) throws IOException {
    for (File file : sourceFolder.listFiles()) {
      File destFile = new File(destFolder, file.getName());
      if (file.isDirectory()) {
        if (!destFile.mkdir()) {
          throw new IOException("Unable to create folder: " + destFile);
        }
        copy(file, destFile);
      } else {
        copyFile(file, destFile);
      }
    }
  }

  public static void recursiveDelete(File file) {
    if (file == null) {
      return;
    }
    if (file.isDirectory()) {
      File[] files = file.listFiles();
      if (files == null) {
        return;
      }
      for (File current : files) {
        recursiveDelete(current);
      }
    }
    file.delete();
  }

  public static File createTempFolder() throws IOException {
    return createTempFolderIn(new File(System.getProperty("java.io.tmpdir")));
  }

  public static File createTempFolderIn(File parent) throws IOException {
    File tmpFolder = new File(parent, "arduino_"
                                      + new Random().nextInt(1000000));
    if (!tmpFolder.mkdir()) {
      throw new IOException("Unable to create temp folder " + tmpFolder);
    }
    return tmpFolder;
  }

  //
  // Compute relative path to "target" from a directory "origin".
  //
  // If "origin" is not absolute, it is relative from the current directory.
  // If "target" is not absolute, it is relative from "origin".
  //
  // by Shigeru KANEMOTO at SWITCHSCIENCE.
  //
  public static String relativePath(String origin, String target) {
    try {
      origin = (new File(origin)).getCanonicalPath();
      File targetFile = new File(target);
      if (targetFile.isAbsolute())
        target = targetFile.getCanonicalPath();
      else
        target = (new File(origin, target)).getCanonicalPath();
    } catch (IOException e) {
      return null;
    }

    if (origin.equals(target)) {
      // origin and target is identical.
      return ".";
    }

    if (origin.equals(File.separator)) {
      // origin is root.
      return "." + target;
    }

    String prefix = "";
    String root = File.separator;

    if (System.getProperty("os.name").indexOf("Windows") != -1) {
      if (origin.startsWith("\\\\") || target.startsWith("\\\\")) {
        // Windows UNC path not supported.
        return null;
      }

      char originLetter = origin.charAt(0);
      char targetLetter = target.charAt(0);
      if (Character.isLetter(originLetter) && Character.isLetter(targetLetter)) {
        // Windows only
        if (originLetter != targetLetter) {
          // Drive letters differ
          return null;
        }
      }

      prefix = "" + originLetter + ':';
      root = prefix + File.separator;
    }

    String relative = "";
    while (!target.startsWith(origin + File.separator)) {
      origin = (new File(origin)).getParent();
      if (origin.equals(root))
        origin = prefix;
      relative += "..";
      relative += File.separator;
    }

    return relative + target.substring(origin.length() + 1);
  }

  public static String getLinuxPathFrom(File file) {
    return BACKSLASH.matcher(file.getAbsolutePath()).replaceAll("/");
  }

  public static boolean isSCCSOrHiddenFile(File file) {
    return file.isHidden() || file.getName().charAt(0) == '.' || (file.isDirectory() && SOURCE_CONTROL_FOLDERS.contains(file.getName()));
  }

  public static String readFileToString(File file) throws IOException {
    BufferedReader reader = null;
    try {
      reader = new BufferedReader(new InputStreamReader(new FileInputStream(file), "UTF-8"));
      StringBuilder sb = new StringBuilder();
      String line;
      while ((line = reader.readLine()) != null) {
        sb.append(line).append("\n");
      }
      return sb.toString();
    } finally {
      if (reader != null) {
        try {
          reader.close();
        } catch (IOException e) {
          // noop
        }
      }
    }
  }

  /**
   * Returns true if the given file has any of the given extensions.
   * @param file
   *          File whose name to look at
   * @param extensions
   *          Extensions to consider (just the extension, without the
   *          dot). Should all be lowercase, case insensitive matching
   *          is used.
   */
  public static boolean hasExtension(File file, String... extensions) {
    return hasExtension(file, Arrays.asList(extensions));
  }

  public static boolean hasExtension(File file, List<String> extensions) {
      String pieces[] = file.getName().split("\\.");
      if (pieces.length < 2)
        return false;

      String extension = pieces[pieces.length - 1];

      return extensions.contains(extension.toLowerCase());

  }

  /**
   * Recursively find all files in a folder with the specified
   * extension. Excludes hidden files and folders and
   * source control folders.
   *
   * @param folder
   *          Folder to look into
   * @param recursive
   *          <b>true</b> will recursively find all files in sub-folders
   * @param extensions
   *          A list of file extensions to search (just the extension,
   *          without the dot). Should all be lowercase, case
   *          insensitive matching is used. If no extensions are
   *          passed, all files are returned.
   * @return
   */
  public static List<File> listFiles(File folder, boolean recursive,
                                     String... extensions) {
    return listFiles(folder, recursive, Arrays.asList(extensions));
  }

  public static List<File> listFiles(File folder, boolean recursive,
                                     List<String> extensions) {
    List<File> result = new ArrayList<File>();

    for (File file : folder.listFiles()) {
      if (isSCCSOrHiddenFile(file))
        continue;

      if (file.isDirectory()) {
        if (recursive)
          result.addAll(listFiles(file, true, extensions));
        continue;
      }

      if (extensions.isEmpty() || hasExtension(file, extensions))
        result.add(file);
    }
    return result;
  }

  public static File newFile(File parent, String... parts) {
    File result = parent;
    for (String part : parts) {
      result = new File(result, part);
    }

    return result;
  }

  public static boolean deleteIfExists(File file) {
    if (file == null) {
      return true;
    }

    return file.delete();
  }

}
