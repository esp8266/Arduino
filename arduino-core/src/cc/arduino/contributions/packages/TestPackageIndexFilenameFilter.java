package cc.arduino.contributions.packages;

import java.io.File;
import java.io.FilenameFilter;

public class TestPackageIndexFilenameFilter implements FilenameFilter {

  private final FilenameFilter parent;

  public TestPackageIndexFilenameFilter(FilenameFilter parent) {
    this.parent = parent;
  }

  public TestPackageIndexFilenameFilter() {
    this(null);
  }

  @Override
  public boolean accept(File file, String name) {
    boolean result = false;
    if (parent != null) {
      result = parent.accept(file, name);
    }
    result = result || (new File(file, name).isFile() && name.startsWith("test_package_") && name.endsWith("_index.json"));
    return result;
  }
}
