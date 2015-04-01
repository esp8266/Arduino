package cc.arduino.contributions.packages.ui;

import cc.arduino.contributions.packages.ContributedPlatform;
import cc.arduino.contributions.packages.filters.CategoryPredicate;
import cc.arduino.contributions.ui.DropdownItem;
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

  @Override
  public boolean equals(Object obj) {
    return obj instanceof DropdownCoreOfCategoryItem && ((DropdownCoreOfCategoryItem) obj).category.equals(category);
  }

}
