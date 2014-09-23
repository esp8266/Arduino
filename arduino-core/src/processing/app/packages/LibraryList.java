package processing.app.packages;

import java.io.File;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;

import processing.app.helpers.FileUtils;

@SuppressWarnings("serial")
public class LibraryList extends ArrayList<Library> {

  public LibraryList(LibraryList libs) {
    super(libs);
  }

  public LibraryList() {
    super();
  }

  public Library getByName(String name) {
    for (Library l : this)
      if (l.getName().equals(name))
        return l;
    return null;
  }

  public void addOrReplace(Library lib) {
    Library l = getByName(lib.getName());
    if (l != null)
      remove(l);
    add(lib);
  }

  public void addOrReplaceAll(Collection<? extends Library> c) {
    for (Library l : c)
      addOrReplace(l);
  }

  public void sort() {
    Collections.sort(this, Library.CASE_INSENSITIVE_ORDER);
  }

  public Library search(String name, String arch) {
    for (Library lib : this) {
      if (!lib.getName().equals(name))
        continue;
      if (!lib.supportsArchitecture(arch))
        continue;
      return lib;
    }
    return null;
  }

  public LibraryList filterByArchitecture(String reqArch) {
    LibraryList res = new LibraryList();
    for (Library lib : this)
      if (lib.supportsArchitecture(reqArch))
        res.add(lib);
    return res;
  }

  public LibraryList filterLibrariesInSubfolder(File subFolder) {
    LibraryList res = new LibraryList();
    for (Library lib : this)
      if (FileUtils.isSubDirectory(subFolder, lib.getFolder()))
        res.add(lib);
    return res;
  }
}
