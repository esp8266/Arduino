package processing.app.packages;

import java.util.ArrayList;

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

}
