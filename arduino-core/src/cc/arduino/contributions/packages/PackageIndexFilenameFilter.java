package cc.arduino.contributions.packages;

import java.io.File;
import java.io.FilenameFilter;

public class PackageIndexFilenameFilter implements FilenameFilter {

  private final String defaultPackageIndexFileName;

  public PackageIndexFilenameFilter(String defaultPackageIndexFileName) {
    this.defaultPackageIndexFileName = defaultPackageIndexFileName;
  }

  @Override
  public boolean accept(File file, String name) {
    return !defaultPackageIndexFileName.equals(name) && name.startsWith("package_") && name.endsWith("_index.json");
  }
}
