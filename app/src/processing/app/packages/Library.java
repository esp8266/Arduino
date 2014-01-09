package processing.app.packages;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Comparator;
import java.util.List;

import processing.app.helpers.FileUtils;
import processing.app.helpers.PreferencesMap;

public class Library {

  private String name;
  private String version;
  private String author;
  private String maintainer;
  private String sentence;
  private String paragraph;
  private String url;
  private String category;
  private String license;
  private List<String> architectures;
  private File folder;
  private File srcFolder;
  private boolean useRecursion;
  private boolean isLegacy;

  private static final List<String> MANDATORY_PROPERTIES = Arrays
      .asList(new String[] { "name", "version", "author", "maintainer",
          "sentence", "paragraph", "url" });

  private static final List<String> CATEGORIES = Arrays.asList(new String[] {
      "Display", "Communication", "Signal Input/Output", "Sensors",
      "Device Control", "Timing", "Data Storage", "Data Processing", "Other",
      "Uncategorized" });

  /**
   * Scans inside a folder and create a Library object out of it. Automatically
   * detects legacy libraries. Automatically fills metadata from
   * library.properties file if found.
   * 
   * @param libFolder
   * @return
   */
  static public Library create(File libFolder) throws IOException {
    // A library is considered "new" if it contains a file called
    // "library.properties"
    File check = new File(libFolder, "library.properties");
    if (!check.exists() || !check.isFile())
      return createLegacyLibrary(libFolder);
    else
      return createLibrary(libFolder);
  }

  private static Library createLibrary(File libFolder) throws IOException {
    // Parse metadata
    File propertiesFile = new File(libFolder, "library.properties");
    PreferencesMap properties = new PreferencesMap();
    properties.load(propertiesFile);

    // Library sanity checks
    // ---------------------

    // Compatibility with 1.5 rev.1 libraries:
    // "email" field changed to "maintainer"
    if (!properties.containsKey("maintainer"))
      properties.put("maintainer", properties.get("email"));

    // Compatibility with 1.5 rev.1 libraries:
    // "arch" folder no longer supported
    File archFolder = new File(libFolder, "arch");
    if (archFolder.isDirectory())
      throw new IOException("'arch' folder is no longer supported! See "
          + "http://goo.gl/gfFJzU for more information");

    // Check mandatory properties
    for (String p : MANDATORY_PROPERTIES)
      if (!properties.containsKey(p))
        throw new IOException("Missing '" + p + "' from library");

    // Check layout
    boolean useRecursion;
    File srcFolder = new File(libFolder, "src");

    if (srcFolder.exists() && srcFolder.isDirectory()) {
      // Layout with a single "src" folder and recursive compilation
      useRecursion = true;

      File utilFolder = new File(libFolder, "utility");
      if (utilFolder.exists() && utilFolder.isDirectory()) {
        throw new IOException(
            "Library can't use both 'src' and 'utility' folders.");
      }
    } else {
      // Layout with source code on library's root and "utility" folders
      srcFolder = libFolder;
      useRecursion = false;
    }

    // Warn if root folder contains development leftovers
    for (File file : libFolder.listFiles()) {
      if (file.isDirectory()) {
        if (FileUtils.isSCCSOrHiddenFile(file)) {
          System.out.println("WARNING: Spurious " + file.getName() +
              " folder in '" + properties.get("name") + "' library");
          continue;
        }
      }
    }

    // Extract metadata info
    String architectures = properties.get("architectures");
    if (architectures == null)
      architectures = "*"; // defaults to "any"
    List<String> archs = new ArrayList<String>();
    for (String arch : architectures.split(","))
      archs.add(arch.trim());

    String category = properties.get("category");
    if (category == null)
      category = "Uncategorized";
    if (!CATEGORIES.contains(category))
      category = "Uncategorized";

    String license = properties.get("license");
    if (license == null)
      license = "Unspecified";

    Library res = new Library();
    res.folder = libFolder;
    res.srcFolder = srcFolder;
    res.name = properties.get("name").trim();
    res.version = properties.get("version").trim();
    res.author = properties.get("author").trim();
    res.maintainer = properties.get("maintainer").trim();
    res.sentence = properties.get("sentence").trim();
    res.paragraph = properties.get("paragraph").trim();
    res.url = properties.get("url").trim();
    res.category = category.trim();
    res.license = license.trim();
    res.architectures = archs;
    res.useRecursion = useRecursion;
    res.isLegacy = false;
    return res;
  }

  private static Library createLegacyLibrary(File libFolder) {
    // construct an old style library
    Library res = new Library();
    res.folder = libFolder;
    res.srcFolder = libFolder;
    res.useRecursion = false;
    res.name = libFolder.getName();
    res.architectures = Arrays.asList("*");
    res.isLegacy = true;
    return res;
  }

  /**
   * Returns <b>true</b> if the library declares to support the specified
   * architecture (through the "architectures" property field).
   * 
   * @param reqArch
   * @return
   */
  public boolean supportsArchitecture(String reqArch) {
    return architectures.contains(reqArch) || architectures.contains("*");
  }

  /**
   * Returns <b>true</b> if the library declares to support at least one of the
   * specified architectures.
   * 
   * @param reqArchs
   *          A List of architectures to check
   * @return
   */
  public boolean supportsArchitecture(List<String> reqArchs) {
    if (reqArchs.contains("*"))
      return true;
    for (String reqArch : reqArchs)
      if (supportsArchitecture(reqArch))
        return true;
    return false;
  }

  public static final Comparator<Library> CASE_INSENSITIVE_ORDER = new Comparator<Library>() {
    @Override
    public int compare(Library o1, Library o2) {
      return o1.getName().compareToIgnoreCase(o2.getName());
    }
  };

  public String getName() {
    return name;
  }

  public File getFolder() {
    return folder;
  }

  public List<String> getArchitectures() {
    return architectures;
  }

  public String getAuthor() {
    return author;
  }

  public String getParagraph() {
    return paragraph;
  }

  public String getSentence() {
    return sentence;
  }

  public String getUrl() {
    return url;
  }

  public String getCategory() {
    return category;
  }

  public String getLicense() {
    return license;
  }

  public static List<String> getCategories() {
    return CATEGORIES;
  }

  public String getVersion() {
    return version;
  }

  public String getMaintainer() {
    return maintainer;
  }

  public boolean useRecursion() {
    return useRecursion;
  }

  public File getSrcFolder() {
    return srcFolder;
  }

  public boolean isLegacy() {
    return isLegacy;
  }

  @Override
  public String toString() {
    String res = "Library:";
    res += " (name=" + name + ")";
    res += " (version=" + version + ")";
    res += " (author=" + author + ")";
    res += " (maintainer=" + maintainer + ")";
    res += " (sentence=" + sentence + ")";
    res += " (paragraph=" + paragraph + ")";
    res += " (url=" + url + ")";
    res += " (architectures=" + architectures + ")";
    return res;
  }
}
