package cc.arduino.libraries.contributions.ui;

import cc.arduino.libraries.contributions.ContributedLibrary;
import cc.arduino.libraries.contributions.ui.filters.CategoryPredicate;
import cc.arduino.ui.DropdownItem;
import com.google.common.base.Predicate;

public class DropdownLibraryOfCategoryItem implements DropdownItem<ContributedLibrary> {

  private final String category;

  public DropdownLibraryOfCategoryItem(String category) {
    this.category = category;
  }

  public String toString() {
    return category;
  }

  @Override
  public Predicate<ContributedLibrary> getFilterPredicate() {
    return new CategoryPredicate(category);
  }
}
