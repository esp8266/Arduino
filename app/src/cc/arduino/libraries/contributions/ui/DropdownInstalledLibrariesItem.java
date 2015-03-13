package cc.arduino.libraries.contributions.ui;

import cc.arduino.libraries.contributions.ContributedLibrary;
import cc.arduino.libraries.contributions.ui.filters.InstalledPredicate;
import cc.arduino.ui.DropdownItem;
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
