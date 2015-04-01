package cc.arduino.contributions.libraries.ui;

import cc.arduino.contributions.libraries.ContributedLibrary;
import cc.arduino.contributions.libraries.filters.CategoryPredicate;
import cc.arduino.contributions.libraries.filters.TypePredicate;
import cc.arduino.contributions.ui.DropdownItem;
import com.google.common.base.Predicate;

public class DropdownLibraryOfTypeItem implements DropdownItem<ContributedLibrary> {

  private final String type;

  public DropdownLibraryOfTypeItem(String type) {
    this.type = type;
  }

  public String toString() {
    return type;
  }

  @Override
  public Predicate<ContributedLibrary> getFilterPredicate() {
    return new TypePredicate(type);
  }

  @Override
  public boolean equals(Object obj) {
    return obj instanceof DropdownLibraryOfTypeItem && ((DropdownLibraryOfTypeItem) obj).type.equals(type);
  }

}
