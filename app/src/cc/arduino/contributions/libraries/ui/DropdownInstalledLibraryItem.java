package cc.arduino.contributions.libraries.ui;

import cc.arduino.contributions.libraries.ContributedLibrary;
import cc.arduino.contributions.libraries.LibrariesIndex;
import cc.arduino.contributions.libraries.filters.InstalledLibraryPredicate;
import cc.arduino.contributions.ui.DropdownItem;
import com.google.common.base.Predicate;

import static processing.app.I18n._;

public class DropdownInstalledLibraryItem implements DropdownItem<ContributedLibrary> {

  private final LibrariesIndex index;

  public DropdownInstalledLibraryItem(LibrariesIndex index) {
    this.index = index;
  }

  public String toString() {
    return _("Installed");
  }

  @Override
  public Predicate<ContributedLibrary> getFilterPredicate() {
    return new InstalledLibraryPredicate(index);
  }

  @Override
  public boolean equals(Object obj) {
    return obj instanceof DropdownInstalledLibraryItem;
  }

}
