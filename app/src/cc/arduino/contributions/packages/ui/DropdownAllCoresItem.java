package cc.arduino.contributions.packages.ui;

import cc.arduino.contributions.filters.NoopPredicate;
import cc.arduino.contributions.packages.ContributedPlatform;
import cc.arduino.contributions.ui.DropdownItem;
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
