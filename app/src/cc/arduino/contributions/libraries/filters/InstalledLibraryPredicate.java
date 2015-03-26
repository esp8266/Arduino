package cc.arduino.contributions.libraries.filters;

import cc.arduino.contributions.libraries.ContributedLibrary;
import cc.arduino.contributions.libraries.LibrariesIndex;
import com.google.common.base.Predicate;
import com.google.common.collect.Collections2;

import java.util.Collection;

public class InstalledLibraryPredicate implements Predicate<ContributedLibrary> {

  private final LibrariesIndex index;

  public InstalledLibraryPredicate(LibrariesIndex index) {
    this.index = index;
  }

  @Override
  public boolean apply(ContributedLibrary input) {
    if (input.isInstalled()) {
      return true;
    }

    Collection<ContributedLibrary> installed = Collections2.filter(index.find(input.getName()), new Predicate<ContributedLibrary>() {
      @Override
      public boolean apply(ContributedLibrary input) {
        return input.isInstalled();
      }
    });

    return !installed.isEmpty();
  }

  @Override
  public boolean equals(Object obj) {
    return obj instanceof InstalledLibraryPredicate;
  }

}
