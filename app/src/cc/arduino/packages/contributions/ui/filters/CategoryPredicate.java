package cc.arduino.packages.contributions.ui.filters;

import cc.arduino.packages.contributions.ContributedPlatform;
import com.google.common.base.Predicate;

public class CategoryPredicate implements Predicate<ContributedPlatform> {

  private final String category;

  public CategoryPredicate(String category) {
    this.category = category;
  }

  @Override
  public boolean apply(ContributedPlatform input) {
    return input.getCategory() != null && category.equals(input.getCategory());
  }

  @Override
  public boolean equals(Object obj) {
    return obj instanceof CategoryPredicate && ((CategoryPredicate) obj).category.equals(category);
  }

}
