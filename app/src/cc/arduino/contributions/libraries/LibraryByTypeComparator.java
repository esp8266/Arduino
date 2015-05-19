package cc.arduino.contributions.libraries;

import java.util.Arrays;
import java.util.Comparator;
import java.util.List;

public class LibraryByTypeComparator implements Comparator<ContributedLibrary> {

  private final List<String> types;

  public LibraryByTypeComparator() {
    this("Arduino", "Recommended", "Contributed");
  }

  public LibraryByTypeComparator(String... types) {
    this.types = Arrays.asList(types);
  }

  @Override
  public int compare(ContributedLibrary o1, ContributedLibrary o2) {
    return types.indexOf(o1.getTypes().get(0)) - types.indexOf(o2.getTypes().get(0));
  }

}
