package cc.arduino.contributions.libraries;

import java.util.Arrays;
import java.util.Comparator;
import java.util.List;

public class LibraryTypeComparator implements Comparator<String> {

  private final List<String> types;

  public LibraryTypeComparator() {
    this("Arduino", "Recommended", "Contributed");
  }

  public LibraryTypeComparator(String... types) {
    this.types = Arrays.asList(types);
  }

  @Override
  public int compare(String o1, String o2) {
    if (types.contains(o1) && types.contains(o2)) {
      return types.indexOf(o1) - types.indexOf(o2);
    } else if (types.contains(o1)) {
      return -1;
    } else if (types.contains(o2)) {
      return 1;
    }
    return o1.compareTo(o2);
  }

}
