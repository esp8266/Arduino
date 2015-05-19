package cc.arduino.contributions.libraries;

import java.util.Comparator;

public class LibraryByTypeComparator implements Comparator<ContributedLibrary> {

  private final LibraryTypeComparator libraryTypeComparator;

  public LibraryByTypeComparator() {
    this(new LibraryTypeComparator());
  }

  public LibraryByTypeComparator(LibraryTypeComparator libraryTypeComparator) {
    this.libraryTypeComparator = libraryTypeComparator;
  }

  @Override
  public int compare(ContributedLibrary o1, ContributedLibrary o2) {
    return libraryTypeComparator.compare(o1.getTypes().get(0), o2.getTypes().get(0));
  }

}
