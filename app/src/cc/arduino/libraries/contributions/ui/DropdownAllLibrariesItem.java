package cc.arduino.libraries.contributions.ui;

import cc.arduino.libraries.contributions.ContributedLibrary;
import cc.arduino.ui.filters.NoopPredicate;
import cc.arduino.ui.DropdownItem;
import com.google.common.base.Predicate;

import static processing.app.I18n._;

public class DropdownAllLibrariesItem implements DropdownItem<ContributedLibrary> {

  public String toString() {
    return _("All");
  }

  @Override
  public Predicate<ContributedLibrary> getFilterPredicate() {
    return new NoopPredicate<ContributedLibrary>();
  }
}
