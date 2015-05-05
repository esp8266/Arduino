package cc.arduino.contributions.libraries.filters;

import cc.arduino.contributions.libraries.ContributedLibrary;
import com.google.common.base.Predicate;

public class LibraryWithNamePredicate implements Predicate<ContributedLibrary> {

  private final String name;

  public LibraryWithNamePredicate(String name) {
    this.name = name;
  }

  @Override
  public boolean apply(ContributedLibrary contributedLibrary) {
    return name.equals(contributedLibrary.getName());
  }

}
