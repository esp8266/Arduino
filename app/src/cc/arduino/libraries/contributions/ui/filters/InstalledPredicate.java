package cc.arduino.libraries.contributions.ui.filters;

import cc.arduino.libraries.contributions.ContributedLibrary;
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
