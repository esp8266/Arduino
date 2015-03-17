package cc.arduino.contributions.libraries.filters;

import cc.arduino.contributions.libraries.ContributedLibrary;
import com.google.common.base.Predicate;

public class CategoryPredicate implements Predicate<ContributedLibrary> {

  private final String category;

  public CategoryPredicate(String category) {
    this.category = category;
  }

  @Override
  public boolean apply(ContributedLibrary input) {
    return input.getCategory() != null && category.equals(input.getCategory());
  }

  @Override
  public boolean equals(Object obj) {
    return obj instanceof CategoryPredicate && ((CategoryPredicate) obj).category.equals(category);
  }

}
