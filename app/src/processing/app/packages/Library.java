package processing.app.packages;

import static processing.app.helpers.StringUtils.wildcardMatch;

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
  private String email;
  private String url;
  private String sentence;
  private String paragraph;
  private List<String> coreDependencies;
  private List<String> dependencies;
  private File folder, srcFolder, archFolder;
  private List<String> architectures;
  private boolean pre15Lib;

  private static final List<String> MANDATORY_PROPERTIES = Arrays
      .asList(new String[] { "architectures", "author", "core-dependencies",
          "dependencies", "email", "name", "paragraph", "sentence", "url",
          "version" });

  /**
   * Scans inside a folder and create a Library object out of it. Automatically
   * detects pre-1.5 libraries. Automatically fills metadata from
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
      return createPre15Library(libFolder);
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

    // 1. Check mandatory properties
    for (String p : MANDATORY_PROPERTIES)
      if (!properties.containsKey(p))
        throw new IOException("Missing '" + p + "' from library");

    // 2. Check mandatory folders
    File srcFolder = new File(libFolder, "src");
    if (!srcFolder.exists() || !srcFolder.isDirectory())
      throw new IOException("Missing 'src' folder");

    // 3. Warn if root folder contains development leftovers
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
    List<String> archs = new ArrayList<String>();
    for (String arch : properties.get("architectures").split(","))
      archs.add(arch.trim());

    List<String> coreDeps = new ArrayList<String>();
    for (String dep : properties.get("core-dependencies").split(","))
      coreDeps.add(dep.trim());

    List<String> dependencies = new ArrayList<String>();
    for (String dependency : properties.get("dependencies").split(",")) {
      dependency = dependency.trim();
      if (!dependency.equals("")) {
        dependencies.add(dependency);
      }
    }

    Library res = new Library();
    res.folder = libFolder;
    res.srcFolder = srcFolder;
    res.archFolder = new File(libFolder, "arch");
    res.name = properties.get("name").trim();
    res.author = properties.get("author").trim();
    res.email = properties.get("email").trim();
    res.sentence = properties.get("sentence").trim();
    res.paragraph = properties.get("paragraph").trim();
    res.url = properties.get("url").trim();
    res.architectures = archs;
    res.coreDependencies = coreDeps;
    res.dependencies = dependencies;
    res.version = properties.get("version").trim();
    res.pre15Lib = false;
    return res;
  }

  private static Library createPre15Library(File libFolder) {
    // construct an old style library
    Library res = new Library();
    res.folder = libFolder;
    res.srcFolder = libFolder;
    res.name = libFolder.getName();
    res.architectures = Arrays.asList("*");
    res.pre15Lib = true;
    return res;
  }

  public List<File> getSrcFolders(String reqArch) {
    if (!supportsArchitecture(reqArch))
      return null;
    List<File> res = new ArrayList<File>();
    res.add(srcFolder);
    File archSpecificFolder = new File(archFolder, reqArch);
    if (archSpecificFolder.exists() && archSpecificFolder.isDirectory()) {
      res.add(archSpecificFolder);
    } else {
      // If specific architecture folder is not found try with "default"
      archSpecificFolder = new File(archFolder, "default");
      if (archSpecificFolder.exists() && archSpecificFolder.isDirectory())
        res.add(archSpecificFolder);
    }
    return res;
  }

  public boolean supportsArchitecture(String reqArch) {
    for (String arch : architectures)
      if (wildcardMatch(reqArch, arch))
        return true;
    return false;
  }

  public static final Comparator<Library> CASE_INSENSITIVE_ORDER = new Comparator<Library>() {
    @Override
    public int compare(Library o1, Library o2) {
      return o1.getName().compareToIgnoreCase(o2.getName());
    }
  };

  public File getSrcFolder() {
    return srcFolder;
  }

  public String getName() {
    return name;
  }

  public boolean isPre15Lib() {
    return pre15Lib;
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

  public List<String> getCoreDependencies() {
    return coreDependencies;
  }

  public List<String> getDependencies() {
    return dependencies;
  }

  public String getEmail() {
    return email;
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

  public String getVersion() {
    return version;
  }

  @Override
  public String toString() {
    String res = "Library:";
    res += " (name=" + name + ")";
    res += " (architectures=" + architectures + ")";
    res += " (author=" + author + ")";
    res += " (core-dependencies=" + coreDependencies + ")";
    res += " (dependencies=" + dependencies + ")";
    res += " (email=" + email + ")";
    res += " (paragraph=" + paragraph + ")";
    res += " (sentence=" + sentence + ")";
    res += " (url=" + url + ")";
    res += " (version=" + version + ")";
    return res;
  }
}
