package cc.arduino.contributions.libraries.ui;

import cc.arduino.contributions.libraries.ContributedLibrary;
import cc.arduino.contributions.filters.NoopPredicate;
import cc.arduino.contributions.ui.DropdownItem;
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
