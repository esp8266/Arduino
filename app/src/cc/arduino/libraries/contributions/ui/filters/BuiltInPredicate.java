package cc.arduino.libraries.contributions.ui.filters;

import cc.arduino.libraries.contributions.ContributedLibrary;
import com.google.common.base.Predicate;

public class BuiltInPredicate implements Predicate<ContributedLibrary> {

  @Override
  public boolean apply(ContributedLibrary input) {
    return input.isReadOnly();
  }

  @Override
  public boolean equals(Object obj) {
    return obj instanceof BuiltInPredicate;
  }
}
