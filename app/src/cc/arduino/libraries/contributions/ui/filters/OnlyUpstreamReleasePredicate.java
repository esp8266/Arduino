package cc.arduino.libraries.contributions.ui.filters;

import cc.arduino.libraries.contributions.ContributedLibrary;
import com.google.common.base.Predicate;
import processing.app.packages.UserLibrary;

public class OnlyUpstreamReleasePredicate implements Predicate<ContributedLibrary> {

  @Override
  public boolean apply(ContributedLibrary input) {
    return !(input instanceof UserLibrary);
  }

  @Override
  public boolean equals(Object obj) {
    return obj instanceof OnlyUpstreamReleasePredicate;
  }
}
