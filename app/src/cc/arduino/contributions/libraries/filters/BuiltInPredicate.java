package cc.arduino.contributions.libraries.filters;

import cc.arduino.contributions.libraries.ContributedLibrary;
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
