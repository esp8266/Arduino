package cc.arduino.contributions.libraries;

import java.util.Comparator;

public class LibraryOfSameTypeComparator implements Comparator<ContributedLibrary> {

  @Override
  public int compare(ContributedLibrary o1, ContributedLibrary o2) {
    if (!o1.getTypes().get(0).equals(o2.getTypes().get(0))) {
      return 0;
    }
    return o1.getName().compareTo(o2.getName());
  }

}
