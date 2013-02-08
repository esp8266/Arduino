package processing.app.packages;

import java.io.File;
import java.util.Comparator;
import java.util.List;

public class Library {

  private String name;
  private File folder;
  private List<String> architectures;
  private boolean oldLib = true;

  /**
   * Scans inside a library folder to see if it contains a version suitable for
   * the actual selected architecture. If a suitable version is found the folder
   * containing that version is selected, otherwise <b>null</b> is selected.<br />
   * <br />
   * If an old-style library is detected, we assume that the library is suitable
   * for the current architecture and the libFolder parameter is used.<br />
   * 
   * @param libFolder
   * @param arch
   *          Currently selected architecture
   * @return
   */
  static public Library fromFolder(File libFolder, String arch) {
    // A library is considered "new" if it contains a file called
    // "library.properties"
    File libraryPropFile = new File(libFolder, "library.properties");
    if (!libraryPropFile.exists() || !libraryPropFile.isFile()) {
      // construct an old style library
      Library res = new Library();
      res.folder = libFolder;
      res.name = libFolder.getName();
      res.oldLib = true;
      return res;
    }

    // Search for a subfolder for actual architecture, return null if not found
    File archSubfolder = new File(libFolder, arch);
    if (!archSubfolder.exists() || !archSubfolder.isDirectory())
      return null;

    Library res = new Library();
    res.folder = archSubfolder;
    res.name = libFolder.getName();
    res.oldLib = false;
    return res;
  }

  public File getRootFolder() {
    return folder;
  }

  public String getName() {
    return name;
  }

  public void setName(String _name) {
    name = _name;
  }

  public static final Comparator<Library> CASE_INSENSITIVE_ORDER = new Comparator<Library>() {
    @Override
    public int compare(Library o1, Library o2) {
      return o1.getName().compareToIgnoreCase(o2.getName());
    }
  };

  public boolean isOldLib() {
    return oldLib;
  }

  public boolean isNewLib() {
    return !oldLib;
  }

}
