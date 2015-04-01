package cc.arduino.contributions.libraries.ui;

import cc.arduino.contributions.libraries.ContributedLibrary;
import cc.arduino.contributions.libraries.filters.CategoryPredicate;
import cc.arduino.contributions.ui.DropdownItem;
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

  @Override
  public boolean equals(Object obj) {
    return obj instanceof DropdownLibraryOfCategoryItem && ((DropdownLibraryOfCategoryItem) obj).category.equals(category);
  }

}
