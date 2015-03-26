package cc.arduino.contributions.libraries.filters;

import cc.arduino.contributions.libraries.ContributedLibrary;
import com.google.common.base.Predicate;

public class TypePredicate implements Predicate<ContributedLibrary> {

  private final String type;

  public TypePredicate(String type) {
    this.type = type;
  }

  @Override
  public boolean apply(ContributedLibrary input) {
    return input.getTypes() != null && input.getTypes().contains(type);
  }

  @Override
  public boolean equals(Object obj) {
    return obj instanceof TypePredicate && ((TypePredicate) obj).type.equals(type);
  }

}
