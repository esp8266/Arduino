package cc.arduino.packages.contributions.ui;

import cc.arduino.ui.filters.NoopPredicate;
import cc.arduino.packages.contributions.ContributedPlatform;
import cc.arduino.ui.DropdownItem;
import com.google.common.base.Predicate;

import static processing.app.I18n._;

public class DropdownAllCoresItem implements DropdownItem<ContributedPlatform> {

  public String toString() {
    return _("All");
  }

  @Override
  public Predicate<ContributedPlatform> getFilterPredicate() {
    return new NoopPredicate<ContributedPlatform>();
  }
}
