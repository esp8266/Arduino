package cc.arduino.contributions.packages;

import java.util.Arrays;
import java.util.List;

public class Constants {

  public static final String DEFAULT_INDEX_FILE_NAME = "package_index.json";
  public static final List<String> PROTECTED_PACKAGE_NAMES = Arrays.asList("arduino", "Intel");
  public static final String PACKAGE_INDEX_URL;

  public static final String PREFERENCES_BOARDS_MANAGER_ADDITIONAL_URLS = "boardsmanager.additional.urls";

  static {
    String extenalPackageIndexUrl = System.getProperty("PACKAGE_INDEX_URL");
    if (extenalPackageIndexUrl != null && !"".equals(extenalPackageIndexUrl)) {
      PACKAGE_INDEX_URL = extenalPackageIndexUrl;
    } else {
      PACKAGE_INDEX_URL = "http://downloads.arduino.cc/packages/package_index.json";
    }
  }

}
