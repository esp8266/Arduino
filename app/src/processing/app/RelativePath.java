/*
 * by Shigeru KANEMOTO at SWITCHSCIENCE.
 */

package processing.app;

import java.io.File;
import java.io.IOException;

class RelativePath {
  //
  // Compute relative path to "target" from a directory "origin".
  //
  // If "origin" is not absolute, it is relative from the current directory.
  // If "target" is not absolute, it is relative from "origin".
  //
  public static String relativePath(String origin, String target) {
    try {
      origin = (new File(origin)).getCanonicalPath();
      File targetFile = new File(target);
      if (targetFile.isAbsolute())
	target = targetFile.getCanonicalPath();
      else
	target = (new File(origin, target)).getCanonicalPath();
    }
    catch (IOException e) {
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
}
