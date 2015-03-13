package cc.arduino.packages.contributions.ui;

import cc.arduino.packages.contributions.ContributedPlatform;
import cc.arduino.packages.contributions.ui.filters.CategoryPredicate;
import cc.arduino.ui.DropdownItem;
import com.google.common.base.Predicate;

public class DropdownCoreOfCategoryItem implements DropdownItem<ContributedPlatform> {

  private final String category;

  public DropdownCoreOfCategoryItem(String category) {
    this.category = category;
  }

  public String toString() {
    return category;
  }

  @Override
  public Predicate<ContributedPlatform> getFilterPredicate() {
    return new CategoryPredicate(category);
  }
}
