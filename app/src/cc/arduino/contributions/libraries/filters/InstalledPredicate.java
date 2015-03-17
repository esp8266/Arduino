package cc.arduino.contributions.libraries.filters;

import cc.arduino.contributions.libraries.ContributedLibrary;
import com.google.common.base.Predicate;

public class InstalledPredicate implements Predicate<ContributedLibrary> {
  @Override
  public boolean apply(ContributedLibrary input) {
    return input.isInstalled();
  }

  @Override
  public boolean equals(Object obj) {
    return obj instanceof InstalledPredicate;
  }

}
