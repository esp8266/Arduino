package processing.app.javax.swing.filechooser;

import javax.swing.filechooser.FileFilter;
import java.io.File;
import java.util.Locale;

public class FileNameExtensionFilter extends FileFilter {

  private final String description;
  private final String[] extensions;

  public FileNameExtensionFilter(String description, String... exts) {
    this.description = description;
    this.extensions = new String[exts.length];
    for (int i = 0; i < exts.length; i++) {
      this.extensions[i] = exts[i].toLowerCase(Locale.ENGLISH);
    }
  }

  @Override
  public boolean accept(File f) {
    if (f == null) {
      return false;
    }

    if (f.isDirectory()) {
      return true;
    }

    String fileName = f.getName();
    int i = fileName.lastIndexOf('.');
    if (i > 0 && i < fileName.length() - 1) {
      String fileExtension = fileName.substring(i + 1).toLowerCase(Locale.ENGLISH);
      for (String extension : extensions) {
        if (extension.equals(fileExtension)) {
          return true;
        }
      }
    }

    return false;
  }

  @Override
  public String getDescription() {
    return description;
  }
}
