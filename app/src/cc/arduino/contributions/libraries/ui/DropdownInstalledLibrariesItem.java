package cc.arduino.contributions.libraries.ui;

import cc.arduino.contributions.libraries.ContributedLibrary;
import cc.arduino.contributions.libraries.filters.InstalledPredicate;
import cc.arduino.contributions.ui.DropdownItem;
import com.google.common.base.Predicate;

import static processing.app.I18n._;

public class DropdownInstalledLibrariesItem implements DropdownItem<ContributedLibrary> {

  public String toString() {
    return _("Installed");
  }

  @Override
  public Predicate<ContributedLibrary> getFilterPredicate() {
    return new InstalledPredicate();
  }
}
