package processing.app.packages;

import static processing.app.helpers.StringMatchers.wildcardMatch;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Comparator;
import java.util.List;

import processing.app.helpers.PreferencesMap;

public class Library {

  private String name;
  private String version;
  private File folder, srcFolder;
  private List<String> architectures;
  private boolean pre15Lib;

  /**
   * Scans inside a folder and create a Library object out of it. Automatically
   * detects pre-1.5 libraries. Automatically fills metadata from
   * library.properties file if found.
   * 
   * @param libFolder
   * @return
   */
  static public Library create(File libFolder) {
    // A library is considered "new" if it contains a file called
    // "library.properties"
    File check = new File(libFolder, "library.properties");
    if (!check.exists() || !check.isFile())
      return createPre15Library(libFolder);
    else
      return createLibrary(libFolder);
  }

  private static Library createLibrary(File libFolder) {
    // Parse metadata
    File propertiesFile = new File(libFolder, "library.properties");
    PreferencesMap properties = new PreferencesMap();
    try {
      properties.load(propertiesFile);
    } catch (IOException e) {
      e.printStackTrace();
      return null;
    }

    // Library sanity checks
    // ---------------------

    // 1. Check mandatory properties
    if (!properties.containsKey("name"))
      return null;
    if (!properties.containsKey("version"))
      return null;
    if (!properties.containsKey("architectures"))
      return null;

    // 2. Check mandatory folders
    File srcFolder = new File(libFolder, "src");
    if (!srcFolder.exists() && !srcFolder.isDirectory())
      return null;
    
    // TODO: 3. check if root folder contains prohibited stuff
    
    // Extract metadata info
    // TODO: do for all metadata
    List<String> archs = new ArrayList<String>();
    for (String arch : properties.get("architectures").split(","))
      archs.add(arch.trim());

    Library res = new Library();
    res.folder = libFolder;
    res.srcFolder = srcFolder;
    res.name = properties.get("name").trim();
    res.architectures = archs;
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
    res.architectures = Arrays.asList(new String[] { "*" });
    res.pre15Lib = true;
    return res;
  }

  public List<File> getSrcFolders(String reqArch) {
    if (!supportsArchitecture(reqArch))
      return null;
    List<File> res = new ArrayList<File>();
    res.add(srcFolder);
    File archSpecificFolder = new File(srcFolder, reqArch);
    if (archSpecificFolder.exists() && archSpecificFolder.isDirectory())
      res.add(archSpecificFolder);
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
}
