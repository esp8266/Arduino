package cc.arduino.libraries.contributions.ui;

import cc.arduino.libraries.contributions.ContributedLibrary;
import cc.arduino.libraries.contributions.ui.filters.BuiltInPredicate;
import cc.arduino.ui.DropdownItem;
import com.google.common.base.Predicate;

import static processing.app.I18n._;

public class DropdownBuiltInLibrariesItem implements DropdownItem<ContributedLibrary> {

  public String toString() {
    return _("Built-in");
  }

  @Override
  public Predicate<ContributedLibrary> getFilterPredicate() {
    return new BuiltInPredicate();
  }
}
